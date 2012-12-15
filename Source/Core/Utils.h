#ifndef __UTILS_H__
#define __UTILS_H__

#include <Source/HwLayer/Types.h>

#define UTILS CUtils()

class CUtils
{
public:
	static char* itoa(si16 n);
	static char* itoa2(ui8 n);
	static char* ftoa(float f);
	static char tohex(ui8 n);
	static char* clrhex(ui16 c);
	static char* MidiNote(int n);
	static char* FormatVoltage( float fV, int nChars=8 );
	static char* FormatFrequency( float fF, int nChars=8 );

	static int Printf (int x, int y, unsigned short clrf, unsigned short clrb, int scale, const char* format, ...);
	static int Print (int x, int y, unsigned short clrf, unsigned short clrb, int scale, char* str);
	static int Print (int x, int y, unsigned short clrf, unsigned short clrb, int scale, const char* str);

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
	static unsigned int Random()
	{
		// our initial starting seed is 5323
		static unsigned int nSeed = 5323;
		static unsigned int nX = 0;

		// Take the current seed and generate a new value from it
		// Due to our use of large constants and overflow, it would be
		// very hard for someone to predict what the next number is
		// going to be from the previous one.
		nSeed = (8253729 * nSeed + 2396403);
		nSeed += nX++;

		// Take the seed and return a value between 0 and 32767
		return nSeed & 32767;
	}

  static ui16 InterpolateColor( ui16 clrA, ui16 clrB, int nLevel );
};


#endif