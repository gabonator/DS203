#ifndef __UTILS_H__
#define __UTILS_H__

#include <Source/HwLayer/Types.h>

#define UTILS CUtils()

class CUtils
{
public:
	static char* itoa(si16 n);
	static char* ftoa(float f);
	static char tohex(ui8 n);
	static char* clrhex(ui16 c);
	static char* FormatVoltage( float fV, int nChars=8 );
	static char* FormatFrequency( float fF, int nChars=8 );
	template <class T>
	inline void Clamp(T& nVariable, T nMin, T nMax)
	{
		if ( nVariable < nMin )
		{
			nVariable = nMin;
			return;
		}
		if ( nVariable > nMax )
		{
			nVariable = nMax;
			return;
		}
	}
};


#endif