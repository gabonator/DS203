class CBitmap {
	int naccum;
	const ui8* m_p;
	int base_x;
	int base_y;
	int x;
	int y;
	int w;
	int h;

public:
	void Blit( int _x, int _y, const ui8* pData )
	{
		base_x = _x;
		base_y = _y;
		x = 0;
		y = 0;
		naccum = -1;
		if ( pData[0] != 'G' || pData[1] != 'B' )
			return;

		m_p = pData+2;
		w = getvlc8();
		h = getvlc8();
		ui16 arrPal[16];
		for (int i=0; i<16; i++)
		{
			int nHigh = *m_p++;
			int nLow = *m_p++;
			arrPal[i] = (nHigh<<8) | nLow;
		}
		int nPixels = w*h;
		bool bSame = true;
		while ( nPixels > 0 )
		{
			int nCount;
			if ( bSame )
			{
				nCount = getvlc4()+1;
				int nColor = getfix4();
				for (int i=0; i<nCount; i++)
					Pixel( arrPal[nColor] );
				bSame = false;
			} else
			{	// unique
				nCount = getfix4()+1;
				for (int i=0; i<nCount; i++)
				{
					int nColor = getfix4();
					Pixel( arrPal[nColor] );
				}
				bSame = true;
			}
			nPixels -= nCount;
			_ASSERT( nPixels >= 0 );
		}
	}

private:
	void Pixel(ui16 clr)
	{
		BIOS::LCD::PutPixel(base_x+x, base_y+y, clr);
		if ( y & 1 )
		{
			if ( --x < 0 )
			{
				x++;
				y++;
			}
		} else
		{
			if ( ++x >= w )
			{
				x--;
				y++;
			}
		}
	}

	int getvlc8()
	{
		ui8 b;
		int nBit = 0, i = 0;
		do 
		{
			b = *m_p++;
			i |= (b & 127) << nBit;
			nBit += 7;
		} while (b & 128);
		return i;
	}

	int getfix4()
	{
		if ( naccum == -1 )
		{
			naccum = *m_p++;
			return naccum>>4;
		}
		int nX = naccum&0x0f;
		naccum = -1;
		return nX;
	}

	int getvlc4()
	{
		ui8 b;
		int nBit = 0, i = 0;
		do 
		{
			b = getfix4();
			i |= (b & 7) << nBit;
			nBit += 3;
		} while (b & 8);
		return i;
	}
};
