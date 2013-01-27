#define ENABLE_MODULE_GPIOTEST
ADD_MODULE( "OneWire", CWndGpioTest )

#if !defined(__GPIO_H__) && defined(ENABLE_MODULE_GPIOTEST)
#define __GPIO_H__

#include <Source/Framework/Wnd.h>

class CPin
{
	int m_nPort;
	int m_nPin;

public:
	void Create( int nPort, int nPin )
	{
		m_nPort = nPort;
		m_nPin = nPin;
	}

	void High()
	{
		BIOS::GPIO::SetState( m_nPort, m_nPin, BIOS::GPIO::StateSimpleOutput );
		BIOS::GPIO::SetPin( m_nPort, m_nPin, true ); 
	}

	void Low()
	{
		BIOS::GPIO::SetState( m_nPort, m_nPin, BIOS::GPIO::StateSimpleOutput );
		BIOS::GPIO::SetPin( m_nPort, m_nPin, false ); 
	}

	void Float()
	{
		// with pull up
		BIOS::GPIO::SetState( m_nPort, m_nPin, BIOS::GPIO::StateInput | BIOS::GPIO::StateInputPull );
		BIOS::GPIO::SetPin( m_nPort, m_nPin, true ); 
	}

	bool Get()
	{
		return BIOS::GPIO::GetPin( m_nPort, m_nPin );
	}
};

class CDelay
{
public:
	void DelayUs(ui32 us)
	{
		us = us*12;
		while (us--)
		{
			__asm__("");
		}
	}

	void DelayMs(ui32 ms)
	{
		while (ms--)
		{
			ui32 us = 12000;
			while (us--)
			{
				__asm__("");
			}
		}
	}
};

class CDS1820 : public CPin, CDelay
{
	ui8 m_arrScratchpad[9];

public:
	enum {
		DS18S20_FAMILY_CODE = 0x10,
		DS18B20_FAMILY_CODE = 0x28,
		DS1822_FAMILY_CODE = 0x22
	};

public:
	bool Init()
	{
		High();
		DelayUs(50);	
		Low();
		DelayUs(500);
		High();
		DelayUs(100);
		if ( Get() )
			return false;
		DelayUs(400);
		return true;
	}

	bool ReadRom()
	{
		if ( !Init() )
			return false;

		Out(0x33);
		for (int n = 0; n < 8; n++)
			m_arrScratchpad[n] = In();
		return true;
	}

	bool SetConfig(ui8 nTh, ui8 nTl, ui8 nConf )
	{
		if ( !Init() )
			return false;

		Out(0xcc);
		Out(0x4e);	// write scratchpad
		Out(nTh);	// Th/user
		Out(nTl);	// Tl/user
		Out(nConf);	// config -> 12 bit resolution 750ms / conv
		return true;
	}

	bool Convert(int nFamilyCode)
	{
		if ( !Init() )
			return false;

		Out(0xcc);	// skip ROM
		Out(0x44);	// perform temperature conversion

		if ( nFamilyCode == DS18B20_FAMILY_CODE )
			Power();

		// conversion takes max. 750ms
		DelayMs(750+20);

		if ( !Init() )
			return false;	
	
		Out(0xcc);
		Out(0xbe);	// read result
	
		for (int n = 0; n < 9; n++)
			m_arrScratchpad[n] = In();

		return true;
	}

	bool Temperature(float& fTemp, int nFamilyCode)
	{
		if ( nFamilyCode == DS18S20_FAMILY_CODE )
		{
			si16 stemp = (m_arrScratchpad[1] << 8) | m_arrScratchpad[0];
			int temp = stemp;
			temp >>= 1; // cut LSB
			temp <<= 4;
			if ( m_arrScratchpad[/*DS_REG_CntPerDeg*/ 7] != 0x10 )
				return false;
			temp -= 4;
			temp += m_arrScratchpad[/*DS_REG_CntPerDeg*/ 7];
			temp -= m_arrScratchpad[/*DS_REG_CntRemain*/ 6];
			fTemp = temp / 16.0f;
			return true;
		}
		if ( nFamilyCode == DS18B20_FAMILY_CODE )
		{
			si16 temp = (m_arrScratchpad[1] << 8) | m_arrScratchpad[0];
			fTemp = temp / 16.0f;
			return true;
		}

		return false;
	}

	bool Crc(int nLen)
	{
		int shift_reg=0, data_bit, sr_lsb, fb_bit;
   	
   	for (int i=0; i< nLen; i++) /* for each byte */
   	{
   	   for(int j=0; j<8; j++)   /* for each bit */
   	   {
   	      data_bit = (m_arrScratchpad[i]>>j)&0x01;
   	      sr_lsb = shift_reg & 0x01;
   	      fb_bit = (data_bit ^ sr_lsb) & 0x01;
   	      shift_reg = shift_reg >> 1;
   	      if (fb_bit)
   	      {
   	         shift_reg = shift_reg ^ 0x8c;
   	      }
   	   }
		}
		return ( shift_reg == 0 ) ? true : false;
	}

	ui8* GetScratchpad()
	{
		return m_arrScratchpad;
	}

private:
	// override High method to HighZ + pullup
	void High()
	{
		CPin::Float();
	}
	void Power()
	{
		CPin::High();
	}
	ui8 In(void)
	{
		ui8 i_byte = 0;
		for (int n=0; n<8; n++)
		{
			Low();	
			DelayUs(1);
			High();
			DelayUs(1);

      i_byte >>= 1;
      if ( Get() )
        i_byte |= 0x80;	// least sig bit first

			DelayUs(60);
		}
		return i_byte;
	}
	
	void Out(ui8 d)
	{
		for(int n=0; n<8; n++)
		{
			Low();
			if (d & 1)
			{
				High();
				DelayUs(60);
			} else {
				DelayUs(60);
				High();
			}
      d >>= 1;
		}
	}

};

class CWndGpioTest : public CWnd
{
	CDS1820	m_Thermometer;
	bool		m_bFocus;

public:

	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndGpioTest", dwFlags | CWnd::WsNoActivate, CRect(0, 16, 400, 240), pParent);
		m_Thermometer.Create( BIOS::GPIO::PortA, 9 );
	}

	void PrintBits(int x, int y, ui32 value)
	{
		for (int i=0; i<16; i++)
		{
			char str[2] = {'0', 0};
			value <<= 1;
			if ( value & 0x10000 )
				str[0] = '1';
			x += BIOS::LCD::Print(x, y, RGB565(ffffff), RGB565(000000), str);
		}
	}

	void Update()
	{
		bool bInit = false, bConv = false, bCrc = false, bTemp = false, bRom = false;
		float fTemp = 0;
		ui8* pSP = m_Thermometer.GetScratchpad();
		ui16 clr;
		ui8 nFamilyCode = 0;
		bInit = m_Thermometer.Init();
		memset( pSP, 0, 9 );

		if ( bInit )
			bRom = m_Thermometer.ReadRom();

		if ( bRom )
		{
			if ( m_Thermometer.Crc(8) )
				nFamilyCode = pSP[0];
		}

		clr = (bRom && nFamilyCode) ? RGB565(ffff00) : RGB565(808080);
		BIOS::LCD::Printf(20, 0xA0, clr, 0, "ROM = ");
		for (int i=0; i<8; i++)
			BIOS::LCD::Printf(20+6*8+i*24, 0xA0, clr, 0, "%c%c", 
				CUtils::tohex(pSP[i]>>4), CUtils::tohex(pSP[i]&0xf));

		const char* strFamily = "Unknown";
		switch ( nFamilyCode )
		{
			case CDS1820::DS18S20_FAMILY_CODE: strFamily = "DS18S20"; break;
			case CDS1820::DS18B20_FAMILY_CODE: strFamily = "DS18B20"; break;
			case CDS1820::DS1822_FAMILY_CODE: strFamily = "DS1822"; break;
		}

		BIOS::LCD::Printf(20, 0xB0, clr, 0, "Family code = (%c%c) %s  ", 
			CUtils::tohex(nFamilyCode>>4), CUtils::tohex(nFamilyCode&0xf), strFamily);

		memset( pSP, 0, 9 );
		if ( bInit )
			bConv = m_Thermometer.Convert(nFamilyCode);
		if ( bConv )
			bCrc = m_Thermometer.Crc(9);
		if ( bCrc && nFamilyCode )
			bTemp = m_Thermometer.Temperature(fTemp, nFamilyCode);

		const char* strStatus = "Ok!";
		if (!bInit)
			strStatus = "Err:Init";
		else if (!bConv)        
			strStatus = "Err:Conv";
		else if (!bCrc)
			strStatus = "Err:Crc";
		else if (!bTemp)
			strStatus = "Err:Temp";

		BIOS::LCD::Printf(20, 0x90, RGB565(ffFF00), 0, "OneWire status = %s     ", strStatus );
		
		clr = (bConv && bCrc) ? RGB565(ffff00) : RGB565(808080);

		BIOS::LCD::Printf(20, 0xC0, clr, 0, "Scratchpad = ");
		for (int i=0; i<9; i++)
			BIOS::LCD::Printf(20+13*8+i*24, 0xC0, clr, 0, "%c%c", 
				CUtils::tohex(pSP[i]>>4), CUtils::tohex(pSP[i]&0xf));

		clr = bTemp ? RGB565(ffff00) : RGB565(808080);
		BIOS::LCD::Printf(20, 0xD0, clr, 0, "Temperature = %2f" "\xf8" "C   ", fTemp );

		if ( nFamilyCode == CDS1820::DS18B20_FAMILY_CODE && pSP[4] != 0x7f ) 
		{
			m_Thermometer.SetConfig( 0, 0, 0x7f );	// set 12 bits resolution
		}
	}

	virtual void OnTimer()
	{
		if ( HasOverlay() )
			return;

		ui32* pCrl = BIOS::GPIO::GetRegister( BIOS::GPIO::PortA, BIOS::GPIO::RegCrl );
		ui32* pCrh = BIOS::GPIO::GetRegister( BIOS::GPIO::PortA, BIOS::GPIO::RegCrh );
		ui32* pIdr = BIOS::GPIO::GetRegister( BIOS::GPIO::PortA, BIOS::GPIO::RegIdr );
		ui32* pOdr = BIOS::GPIO::GetRegister( BIOS::GPIO::PortA, BIOS::GPIO::RegOdr );
//		ui32* pBsrr = BIOS::GPIO::GetRegister( BIOS::GPIO::PortA, BIOS::GPIO::RegBsrr );
//		ui32* pBrr = BIOS::GPIO::GetRegister( BIOS::GPIO::PortA, BIOS::GPIO::RegBrr );
		ui32* pLckr = BIOS::GPIO::GetRegister( BIOS::GPIO::PortA, BIOS::GPIO::RegLckr );
		                                                                   
		BIOS::LCD::Print ( 20, 0x20, RGB565(ffffff), RGB565(000000), "--- GPIOA ----------------------" );
		BIOS::LCD::Printf( 20, 0x30, RGB565(ffffff), RGB565(000000), "CRL  (0x%x) = 0x%x", pCrl, *pCrl);
		BIOS::LCD::Printf( 20, 0x40, RGB565(ffffff), RGB565(000000), "CRH  (0x%x) = 0x%x", pCrh, *pCrh);
		BIOS::LCD::Printf( 20, 0x50, RGB565(ffffff), RGB565(000000), "IDR  (0x%x) =", pIdr);
		PrintBits( 20+20*8, 0x50, *pIdr);
		BIOS::LCD::Printf( 20, 0x60, RGB565(ffffff), RGB565(000000), "ODR  (0x%x) =", pOdr);
		PrintBits( 20+20*8, 0x60, *pOdr);
		BIOS::LCD::Printf( 20, 0x70, RGB565(ffffff), RGB565(000000), "LCKR (0x%x) = 0x%x", pLckr, *pLckr);

		Update();
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		if (code == ToWord('L', 'D') )
		{
			KillTimer();
			BIOS::GPIO::SetState( BIOS::GPIO::PortA, 9, 0xA );
			return;
		}

		if (code == ToWord('L', 'E') )
		{
			m_Thermometer.Float();
			SetTimer(200);
			return;
		}
	}
};

#endif
