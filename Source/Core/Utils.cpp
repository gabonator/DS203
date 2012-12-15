#include "Utils.h"
#include <Source\HwLayer\bios.h>
#include <stdarg.h>

char tmp[16];
/*static */ const char hex[16] = {'0', '1', '2', '3', '4',
			'5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

/*static*/ char* CUtils::itoa2(ui8 n)
{
	for (int i=0; i<8; i++)
		if (n & (128>>i) )
			tmp[i] = '1';
		else
			tmp[i] = '0';
	tmp[8] = 0;
	return tmp;
}

/*static*/ char* CUtils::itoa(si16 n)
{
	int i = 0;
	ui8 bSignum = 0;
	if (n<0)
	{
		n = -n;
		bSignum = 1;
	} else
	if (n==0)
	{
		tmp[i++] = '0';
	}
	while (n > 0)
	{
		tmp[i++] = '0'+(n%10);
		n /= 10;
	}
	if (bSignum)
		tmp[i++] = '-';
	tmp[i] = 0;
	for (int j=0; j<i/2;j++)
	{
		char t = tmp[j];
		tmp[j] = tmp[i-j-1];
		tmp[i-j-1] = t;
	}
	return tmp;
}
/*static*/ char CUtils::tohex(ui8 n)
{
	_ASSERT( n >= 0 && n < 16 );
	return hex[n];
}
/*static*/ char* CUtils::clrhex(ui16 c)
{
	tmp[0] = tohex((c&0x1f)>>1);
	tmp[1] = tohex(((c>>5)&0x3f)>>2);
	tmp[2] = tohex((c>>11)>>1);
	tmp[3] = 0;
	return tmp;
}
/*static*/ char* CUtils::FormatVoltage( float fV, int nChars )
{
	BIOS::DBG::sprintf(tmp, "%f", fV );
	_ASSERT( strlen(tmp) < 15 );
	int nLen = strlen(tmp);
	while ( nLen > nChars-1 )
		tmp[--nLen] = 0;
	tmp[nLen++] = ' ';
	tmp[nLen++] = 'V';
	tmp[nLen] = 0;
	return tmp;
}
	
/*static*/ char* CUtils::FormatFrequency( float fF, int nChars )
{
	char* strUnits = (char*)" Hz";
	if (fF >= 1000)
	{
		strUnits = (char*)" kHz";
		fF *= 0.001f;
	}
	if (fF >= 1000)
	{
		strUnits = (char*)" MHz";
		fF *= 0.001f;
	}
	BIOS::DBG::sprintf( tmp, "%f", fF );
	int nLen = strlen(tmp);
	int nLenUnits = strlen(strUnits);
	while ( nLen + nLenUnits > nChars )
		tmp[--nLen] = 0;
	if ( tmp[nLen-1] == '.' )
		tmp[--nLen] = 0;
	strcat( tmp, strUnits );
	return tmp;
}
/*static*/ char* CUtils::ftoa(float f)
{
	BIOS::DBG::sprintf( tmp, "%f", f );
	return tmp;
}

/*static*/ char* CUtils::MidiNote(int n)
{
	const static char notes[] = "C-" "C#" "D-" "D#" "E-" "F-" "F#" "G-" "G#" "A-" "A#" "B-";
	if ( n < 12 || n >= 78 )
	{
		tmp[0] = '?';
		tmp[1] = 0;
		return tmp;
	}
	int nOctave = (n / 12)-1;
	int nNote = n % 12;
	tmp[0] = notes[nNote*2];
	tmp[1] = notes[nNote*2+1];
	tmp[2] = '0' + nOctave;
	tmp[3] = 0;
	return tmp;
}

ui16 CUtils::InterpolateColor( ui16 clrA, ui16 clrB, int nLevel )
{
	int br = Get565R(clrB)*nLevel;
	int bg = Get565G(clrB)*nLevel;
	int bb = Get565B(clrB)*nLevel;
	nLevel = 255-nLevel;
	int ar = Get565R(clrA)*nLevel;
	int ag = Get565G(clrA)*nLevel;
	int ab = Get565B(clrA)*nLevel;
	ar = (ar+br) / 256;
	ag = (ag+bg) / 256;
	ab = (ab+bb) / 256;
	return RGB565RGB(ar, ag, ab);
}


/*static*/ int _DrawChar(int x, int y, unsigned short clrf, unsigned short clrb, char ch, int scale)
{
	const unsigned char *pFont = BIOS::LCD::GetFont(ch);
	if (clrb == RGBTRANS)
	{
		for (ui8 _y=0; _y<(14*scale); _y+=scale)
		{
			ui8 col = ~*pFont++;
	
			for (ui8 _x=0; _x<(8*scale); _x+=scale, col <<= 1)
			{
				if ( col & 128 )
				{
					for(int i=0 ; i<=scale ; i++)
					{
						for(int j=0 ; j<=scale ; j++)
						{
							BIOS::LCD::PutPixel(x+_x+i, y+_y+j, clrf);
						}
					}
				}
			}
		}
	} else if (clrf == RGBTRANS)
	{
		for (ui8 _y=0; _y<(14*scale); _y+=scale)
		{
			ui8 col = ~*pFont++;
	
			for (ui8 _x=0; _x<(8*scale); _x+=scale, col <<= 1)
			{
				if ( (col & 128) == 0 )
				{
					for(int i=0 ; i<=scale ; i++)
					{
						for(int j=0 ; j<=scale ; j++)
						{
							BIOS::LCD::PutPixel(x+_x+i, y+_y+j, clrb);
						}
					}
				}
			}
		}
	} else
	{
		for (ui8 _y=0; _y<(14*scale); _y+=scale)
		{
			ui8 col = ~*pFont++;
	
			for (ui8 _x=0; _x<(8*scale); _x+=scale, col <<= 1)
			{
				for(int i=0 ; i<=scale ; i++)
				{
					for(int j=0 ; j<=scale ; j++)
					{
						if ( col & 128 )
						{
							BIOS::LCD::PutPixel(x+_x+i, y+_y+j, clrf);
						}
						else
						{
							BIOS::LCD::PutPixel(x+_x+i, y+_y+j, clrb);
						}
					}
				}
			}
		}
	}
	return (8*scale);
}

/*static*/ int CUtils::Printf (int x, int y, unsigned short clrf, unsigned short clrb, int scale, const char * format, ...)
{
	char buf[128];
	char* bbuf = buf; 

        va_list args;
        
        va_start( args, format );
        int aux = BIOS::DBG::sprintf( bbuf, format, args );
	Print(x, y, clrf, clrb, scale, buf);
	return aux;
}
       
/*static*/ int CUtils::Print (int x, int y, unsigned short clrf, unsigned short clrb, int scale, char *str)
{
	return CUtils::Print (x, y, clrf, clrb, scale, (const char*)str);
}

/*static*/ int CUtils::Print (int x, int y, unsigned short clrf, unsigned short clrb, int scale, const char *str)
{
	if (!str || !*str)
		return 0;
	int _x = x;
	for (;*str; str++)
	{
		if (*str == '\n')
		{
			x = _x;
			y += (16*scale);
			continue;
		}
		x += _DrawChar(x, y, clrf, clrb, *str, scale);
	}
	return x - _x;
}

