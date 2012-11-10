#include "Utils.h"
#include <Source\HwLayer\bios.h>

char tmp[16];
/*static */ const char hex[16] = {'0', '1', '2', '3', '4',
			'5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

	/*static*/ char* CUtils::itoa(si16 n)
	{
//		static char tmp[8];
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
