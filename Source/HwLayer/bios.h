#ifndef __BIOS_H__
#define __BIOS_H__

#include <Source/Framework/Classes.h>
#include "Types.h"

class BIOS {
public:
	static void DelayMs(unsigned short ms);
	static ui32 GetTick();
	static void Init();

	class LCD {
	public:
		enum {
			// Screen resolution
			LcdWidth = 400,
			LcdHeight = 240,

			// BufferBegin mode
			BfXY = 1,
			BfYX = 2
		};

	public:
		static void Init();
		static int Printf (int x, int y, unsigned short clrf, unsigned short clrb, const char* format, ...);
		static int Print (int x, int y, unsigned short clrf, unsigned short clrb, char* str);
		static int Print (int x, int y, unsigned short clrf, unsigned short clrb, const char* str);
		static int Print (const CPoint& cp, unsigned short clrf, char *str);
		static void Line(int x1, int y1, int x2, int y2, unsigned short clr);
		static void PutPixel(int x, int y, unsigned short clr);
		static void PutPixel(const CPoint& cp, unsigned short clr);
		static void Clear(unsigned short clr);
		static void RoundRect(int x1, int y1, int x2, int y2, unsigned short clr);
		static void RoundRect(const CRect& rc, unsigned short clr);
		static void Bar(int x1, int y1, int x2, int y2, unsigned short clr);
		static void Bar(const CRect& rc, unsigned short clr);
		static void Rectangle(const CRect& rc, unsigned short clr);
		static void Pattern(int x1, int y1, int x2, int y2, const ui16 *pat, int l);
		static int Draw(int x, int y, unsigned short clrf, unsigned short clrb, const char *p);
		static void BufferBegin(const CRect& rc, ui8 nMode);
		static void BufferPush(ui16 clr);
		static void BufferEnd();
		static void Buffer(int x, int y, unsigned short* pBuffer, int n);
	};

	class KEY {
	public:
		enum {
			// Key definitions
			KeyLeft = 1,
			KeyRight = 2,
			KeyUp = 4,
			KeyDown = 8,
			KeyEnter = 16,
			KeyEscape = 32,
		};

	public:
		static ui16 GetKeys();
	};

	class ADC {
	public:
		static void Init();
		static unsigned char Ready();
		static unsigned long Get();
		static void Restart();
		static void Configure(ui8 nACouple, ui8 nARange, ui16 nAOffset, ui8 nBCouple, ui8 nBRange, ui16 nBOffset, ui16 nTimePsc, ui16 nTimeArr);
		static void ConfigureTrigger(ui16 nTThreshold, ui16 nVThreshold, ui8 nSource, ui8 nType);
	};

	class GEN {
	public:
		static void ConfigureSq(ui16 psc, ui16 arr, ui16 ccr);
		static void ConfigureWave(ui16* pData, ui16 cnt);
		static void ConfigureWaveRate(ui16 arr);
	};

};

#endif