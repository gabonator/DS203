class CCanbusDecoder : public CBitDecoder 
{
private:
	bool m_bStartBit;
	int m_nLastBit;
	int m_nCount;
	int m_nIndex;

public:
	int m_nFrameArb;
	int m_nFrameDlc;
	int m_arrFrameData[8];
	int m_nFrameCrc;
	bool m_bFinished;
	unsigned int m_nCheckCrc;

public:
	CCanbusDecoder()
	{
		m_bStartBit = false;
		m_nFrameArb = 0;
		m_nFrameDlc = 0;
		m_nFrameCrc = 0;
		m_bFinished = false;
		memset( m_arrFrameData, 0, sizeof(m_arrFrameData) );
	}

	void StartCrc()
	{
		m_nCheckCrc = 0;
	}

	void PushCrc( unsigned char bit )
	{
		m_nCheckCrc = m_nCheckCrc << 1;        // leftshift 1 bit
        if ( bit ^ ((m_nCheckCrc>>15)&1) )
	        m_nCheckCrc = m_nCheckCrc ^ 0x4599;
        m_nCheckCrc = m_nCheckCrc & 0x7fff; // only 15 bits
	}

	virtual bool operator << (int nBit)
	{
		if ( !m_bStartBit )
		{
			if ( nBit == 0 )
			{
				BIOS::DBG::Print("<");

				m_bStartBit = true;
				m_nIndex = 1;
				m_nLastBit = 0;
				m_nCount = 1;

				StartCrc();
				PushCrc( nBit );
			}
			return true;
		}

		if ( m_nIndex > 22+8*m_nFrameDlc+15 )
		{
			return true; // idle bits continue
		}

		if ( m_nCount == 5 )
		{
			BIOS::DBG::Print("(%d)", nBit);
			// bit stuffing
			if ( nBit != 1-m_nLastBit )
			{
				//_ASSERT( 0 );
				//return false;
				if ( !m_bFinished )
					m_bStartBit = false;
				return true;
			}
			m_nLastBit = nBit;
			m_nCount = 1;
			return true;
		} else
			BIOS::DBG::Print("%d", nBit);


		if ( nBit == m_nLastBit )
		{ 
			m_nCount++;
		} else
		{
			m_nCount = 1;
			m_nLastBit = nBit;
		}
		
		m_nIndex++;

		if ( m_nIndex >= 2 && m_nIndex < 20+8*m_nFrameDlc )
			PushCrc( nBit );

		if ( m_nIndex >= 2 && m_nIndex < 2+11 )
		{
			if ( m_nIndex == 2 )
				BIOS::DBG::Print("arb=");
			m_nFrameArb <<= 1;
			m_nFrameArb |= nBit;
		}

		if ( m_nIndex >= 13 && m_nIndex < 13+3 )
		{
			if ( m_nIndex == 13 )
				BIOS::DBG::Print(" flags=");

			// RTR, IDE, RB0
		}

		if ( m_nIndex >= 16 && m_nIndex < 16+4 )
		{
			if ( m_nIndex == 16 )
				BIOS::DBG::Print(" dlc=");
			m_nFrameDlc <<= 1;
			m_nFrameDlc |= nBit;
			_ASSERT( m_nFrameDlc <= 8 );
		}

		if ( m_nIndex >= 20 && m_nIndex < 20+8*m_nFrameDlc )
		{
			int nByte = (m_nIndex - 20)/8;
			if ( (m_nFrameDlc - 20) % 8 == 0)
				BIOS::DBG::Print(" D%d=", nByte);
			_ASSERT( nByte < 8 );
			m_arrFrameData[nByte] <<= 1;
			m_arrFrameData[nByte] |= nBit;
		}

		//if ( m_nIndex >= 22+8*m_nFrameDlc+1 && m_nIndex <= 22+8*m_nFrameDlc+15 )
		if ( m_nIndex >= 20+8*m_nFrameDlc && m_nIndex < 20+8*m_nFrameDlc+15 )
		{
			//m_bFinished = true; // data bytes were received successfully

			if ( m_nIndex == 20+8*m_nFrameDlc )
				BIOS::DBG::Print(" crc=");

			m_nFrameCrc <<= 1;
			m_nFrameCrc |= nBit;

			if ( m_nIndex == 20+8*m_nFrameDlc+15-1 )
			{
				BIOS::DBG::Print(">");
				BIOS::DBG::Print( "finished, crc=%x, check=%x", m_nFrameCrc, m_nCheckCrc );
				m_bFinished = true;
			}
		}
				
		return true;
	}

	void Visualize()
	{
		int x = 20;
		int y = 30;
		x += BIOS::LCD::Print(x, y, RGB565(ff0000), 0x0101, "CanBus: ");
		x += 8*BIOS::LCD::Printf(x, y, RGB565(ffffff), 0x0101, "ID=%03x", m_nFrameArb );
		x += 2;
		if ( m_nFrameDlc == 0 )
		{
			x += 8*BIOS::LCD::Printf(x, y, RGB565(ffffff), 0x0101, "DLC=0");
		} else 
		{
			x += 8*BIOS::LCD::Printf(x, y, RGB565(ffffff), 0x0101, "(", m_nFrameArb );
			for (int i=0; i<m_nFrameDlc; i++)
				x += 8*BIOS::LCD::Printf(x, y, RGB565(b0b0b0), 0x0101, "%02x", m_arrFrameData[i])+2;
			x -= 2;
			x += 8*BIOS::LCD::Printf(x, y, RGB565(ffffff), 0x0101, ")" );
		}
		x += 2;
		if ( m_bFinished )
		{
			if ( (int)m_nFrameCrc == (int)m_nCheckCrc )
				x += 8*BIOS::LCD::Printf(x, y, RGB565(ffffff), 0x0101, "CRC ok" );
			else
				x += 8*BIOS::LCD::Printf(x, y, RGB565(ff0000), 0x0101, "CRC error" );
		} else
			x += 8*BIOS::LCD::Printf(x, y, RGB565(ff0000), 0x0101, "error" );
	}

	void Test()
	{
		// http://www.valky.eu/data/software/!can.html
		// id=0x320, dlc=8,	0x12, 0x00, 0x7f, 0xff, 0xff, 0xc1, 0xff, 0x00
		//const char* ptest = "11001100100000100010000010100100000100000111110111110111110111110111110100000111110111110000010000011011001110110011111111111";
		//const char* ptest = "1100010010100000001010101010111011110";
		const char* ptest =  "1100010010100000100101010101011101";
		for (const char* p=ptest; *p; p++)
			*this << *p - '0';

		Visualize();
	}
};

/*
struct CI {
	CI()
	{
		CCanbusDecoder c;
		c.Test();
		BIOS::DBG::Print("finished=%d", c.m_bFinished);
	}
} instance;
*/