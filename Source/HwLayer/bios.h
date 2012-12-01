#ifndef __BIOS_H__
#define __BIOS_H__

#include <Source/Framework/Classes.h>
#include "Types.h"

class BIOS {
public:
	class DBG {
	public:
		static void Print (const char* format, ...);
		// move to utils?
		static int sprintf(char* buf, const char * format, ...);
	};

	class SYS {
	public:
		enum {
			EApp1,
			EApp2,
			EApp3,
			EApp4,
			ESys,
			EDfu
		};

		static void DelayMs(unsigned short ms);
		static ui32 GetTick();
		static void Init();
		static void Beep( int ms );
		static int GetBattery();
		static void SetBacklight(int nLevel); // 0..100
		static void SetVolume(int nLevel); // 0..100
		static void Execute( int nCode );
	};

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
		static ui16 GetPixel(int x, int y);
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
		
		static void GetImage(const CRect& rcRect, ui16* pBuffer );
		static void PutImage(const CRect& rcRect, ui16* pBuffer );
		static void Shadow(int x1, int y1, int x2, int y2, unsigned int nColor);

		static const ui8* GetFont(ui8 Code);

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
			KeyFunction = 64,
			KeyFunction2 = 128,
			KeyS1 = 256,
			KeyS2 = 512
		};

	public:
		static ui16 GetKeys();
	};

	class ADC {
	public:
		typedef unsigned long TSample;
		union SSample {
			struct
			{
				ui8 CH1 : 8;
				ui8 CH2 : 8;
				ui8 CH3 : 1;
				ui8 CH4 : 1;
				ui8 dummy0 : 6;
				ui8 dummy1 : 8;
			};
			ui8 CH[4];
			ui32 nValue;
		};

	public:
		static void Init();
		static unsigned char Ready();
	
	private:
		static unsigned long Get();
	public:

		static void Restart();
		static void Enable(bool bEnable);
		static bool Enabled();
		static void Configure(ui8 nACouple, ui8 nARange, ui16 nAOffset, ui8 nBCouple, ui8 nBRange, ui16 nBOffset, ui16 nTimePsc, ui16 nTimeArr);
		static void ConfigureTrigger(ui16 nTThreshold, ui16 nVThreshold, ui8 nSource, ui8 nType);
		static void ConfigureBuffer(int nLength);
		static void GetBufferRange(int& nBegin, int& nEnd);

		static void Copy(int nCount);
		static unsigned long GetCount();
		static TSample& GetAt(int i);
	};

	class GEN {
	public:
		static void ConfigureSq(ui16 psc, ui16 arr, ui16 ccr);
		static void ConfigureWave(ui16* pData, ui16 cnt);
		static void ConfigureWaveRate(ui16 arr);
		static void ConfigureDc(ui16 nData);
	};

	class DSK {
	public:
		enum {
			IoRead = 1,
			IoWrite = 2,
			IoClosed = 3
		};
		static PVOID GetSharedBuffer();
		static BOOL Open(FILEINFO* pFileInfo, const char* strName, ui8 nIoMode);
		static BOOL Read(FILEINFO* pFileInfo, ui8* pSectorData);
		static BOOL Write(FILEINFO* pFileInfo, ui8* pSectorData);
		static BOOL Close(FILEINFO* pFileInfo, int nSize = -1);
	};

	class FFT
	{	
	public:	
		static void Window( si16* arrSignal, const ui16* arrWindowHalf, int n );
		static void Convert( si16* arrOutput, si16* arrInput, int n );	
		static ui32 Sqrt( ui32 value );
	};

	class SERIAL
	{
	public:
		static void Init();
		static void Send(const char* strBuf);
		static int Getch();
	};

	class VER
	{
	public:
		static const char* GetHardwareVersion();
		static const char* GetSystemVersion();
		static const char* GetFpgaVersion();
		static const char* GetDfuVersion();
		static ui32 GetDisplayType();
		static ui32 GetSerialNumber();
		static void DrawLogo(int x, int y);
	};
};

#endif