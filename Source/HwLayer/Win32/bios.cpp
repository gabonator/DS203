#pragma once

#include <Windows.h>
#include <stdio.h>
#include <crtdbg.h>
#include "device.h"
#include <Source/HwLayer/Types.h>
#include <Source/HwLayer/bios.h>
#define DEVICE CDevice::m_pInstance
#include "font.h"
#include <math.h>
//u16 *Get_TAB_8x14(u8 Code)
#include "sprintf\spf.h"

CRect m_rcBuffer;
CPoint m_cpBuffer;
int m_nKeys;
int g_nBufferLen = 4096;

#define RGB565RGB(r, g, b) (((r)>>3)|(((g)>>2)<<5)|(((b)>>3)<<11))

int _DrawChar(int x, int y, unsigned short clrf, unsigned short clrb, char ch);
DWORD FROM_565_TO_RGB(unsigned short clr565);
WORD FROM_RGB_TO_565(unsigned int clrrgb);
ui8 _Round(int x, int y);

void Assert(const char *msg, int n)
{
	_ASSERT_EXPR((0), NULL);
}

/*static*/ void BIOS::SYS::Init()
{
}

/*static*/ int BIOS::LCD::Printf (int x, int y, unsigned short clrf, unsigned short clrb, const char * format, ...)
{
	char buffer[256];
	/*
	va_list args;
	va_start (args, format);
	vsnprintf_s (buffer, 256, 255, format, args);
	int nAux = Print (x, y, clrf, clrb, buffer);
	va_end (args);
	*/

	char* bbuf = buffer; 
    va_list args;
    va_start( args, format );
	int nAux = print( &bbuf, format, args );
	Print( x, y, clrf, clrb, buffer );

	return nAux;
}
       
/*static*/ int BIOS::LCD::Print (int x, int y, unsigned short clrf, unsigned short clrb, char *str)
{
	if (!str || !*str)
		return 0;
	int nAux = 0;
	int _x = x;
	for (;*str; str++)
	{
		if (*str == '\n')
		{
			x = _x;
			y += 16;
			continue;
		}
		int nW = _DrawChar(x, y, clrf, clrb, *str);
		x += nW;
		nAux += nW;
	}
	return nAux;
}

/*static*/ int BIOS::LCD::Print (int x, int y, unsigned short clrf, unsigned short clrb, const char *str)
{
	/*
#ifdef _WIN32
	_ASSERT( (int)str > 0x10000 );
	const char *xx = str;
	xx=xx;
#endif*/
	if (!str || !*str)
		return 0;
	int nAux = 0;
	int _x = x;
	for (;*str; str++)
	{
		if (*str == '\n')
		{
			x = _x;
			y += 16;
			continue;
		}
		int nW = _DrawChar(x, y, clrf, clrb, *str);
		x += nW;
		nAux += nW;
	}
	return nAux;
}

/*static*/ int BIOS::LCD::Print (const CPoint& cp, unsigned short clrf, char *str)
{
	return Print(cp.x, cp.y, clrf, RGBTRANS, str);
}

/*static*/ void BIOS::LCD::PutPixel(int x, int y, unsigned short clr)
{
	if ( clr == RGBTRANS )
		return;
	DWORD *pBuf = (DWORD*)DEVICE->display.GetBuffer();
	pBuf[y*CFrameBuffer::Width+x] = FROM_565_TO_RGB(clr);
}

/*static*/ ui16 BIOS::LCD::GetPixel(int x, int y)
{
	DWORD *pBuf = (DWORD*)DEVICE->display.GetBuffer();
	return FROM_RGB_TO_565( pBuf[y*CFrameBuffer::Width+x] );
}

/*static*/ void BIOS::LCD::PutPixel(const CPoint& cp, unsigned short clr)
{
	PutPixel( cp.x, cp.y, clr );
}

/*static*/ void BIOS::LCD::Clear(unsigned short clr)
{
	DWORD *pBuf = (DWORD*)DEVICE->display.GetBuffer();
	COLORREF c = FROM_565_TO_RGB(clr);
	for (int i=0; i<CFrameBuffer::Width*CFrameBuffer::Height; i++, pBuf++)
		*pBuf = c;
}

/*static*/ void BIOS::LCD::RoundRect(int x1, int y1, int x2, int y2, unsigned short clr)
{
	for (int x=x1; x<x2; x++)
		for (int y=y1; y<y2; y++)
			if ( !_Round(min(x-x1, x2-x-1), min(y-y1, y2-y-1)) )
				PutPixel(x, y, clr);
}

/*static*/ void BIOS::LCD::RoundRect(const CRect& rc, unsigned short clr)
{
	RoundRect(rc.left, rc.top, rc.right, rc.bottom, clr);
}

/*static*/ void BIOS::LCD::Bar(int x1, int y1, int x2, int y2, unsigned short clr)
{
	for (int x=x1; x<x2; x++)
		for (int y=y1; y<y2; y++)
			PutPixel(x, y, clr);
}

/*static*/ void BIOS::LCD::Bar(const CRect& rc, unsigned short clr)
{
	Bar(rc.left, rc.top, rc.right, rc.bottom, clr);
}

/*static*/ void BIOS::LCD::Rectangle(const CRect& rc, unsigned short clr)
{
	BIOS::LCD::Bar(rc.left, rc.top, rc.right, rc.top+1, clr);
	BIOS::LCD::Bar(rc.left, rc.bottom-1, rc.right, rc.bottom, clr);
	BIOS::LCD::Bar(rc.left, rc.top, rc.left+1, rc.bottom, clr);
	BIOS::LCD::Bar(rc.right-1, rc.top, rc.right, rc.bottom, clr);
}

/*static*/ void BIOS::LCD::Pattern(int x1, int y1, int x2, int y2, const ui16 *pat, int l)
{
	const ui16* patb = pat;
	const ui16* pate = patb + l;
	
	for (int x=x1; x<x2; x++)
		//for (int y=y1; y<y2; y++)
		for (int y=y2-1; y>=y1; y--)
		{
			PutPixel(x, y, *pat);
			if (++pat == pate) 
				pat = patb;
		}
}

/*static*/ void BIOS::LCD::GetImage(const CRect& rcRect, ui16* pBuffer )
{
	DWORD *pBuf = (DWORD*)DEVICE->display.GetBuffer();
	int x1=rcRect.left, x2=rcRect.right, y1=rcRect.top, y2=rcRect.bottom;
	for (int x=x1; x<x2; x++)
		for (int y=y2-1; y>=y1; y--)
		{
			*pBuffer++ = FROM_RGB_TO_565( pBuf[y*CFrameBuffer::Width+x] );

		}
}

/*static*/ void BIOS::LCD::PutImage(const CRect& rcRect, ui16* pBuffer )
{
	int x1=rcRect.left, x2=rcRect.right, y1=rcRect.top, y2=rcRect.bottom;
	for (int x=x1; x<x2; x++)
		for (int y=y2-1; y>=y1; y--)
		{
			PutPixel(x, y, *pBuffer++);
		}
}


/*static*/ int BIOS::LCD::Draw(int x, int y, unsigned short clrf, unsigned short clrb, const char *p)
{
	int h = *p++;
	for (int _x=0; _x<80; _x++)
		for (int _y=0; _y<h; _y++)
		{
			if (!p[_x*h+_y])
				return _x;
			PutPixel(x+_x, y+_y, (p[_x*h+_y]&4) ? clrf : clrb);
		}
	return 8;
}

/*static*/ void BIOS::LCD::BufferBegin(const CRect& rc, ui8 nMode)
{
	m_rcBuffer = rc;
	m_cpBuffer = rc.TopLeft();
}

/*static*/ void BIOS::LCD::BufferPush(ui16 clr)
{
	PutPixel( m_cpBuffer, clr );
	if ( ++m_cpBuffer.x >= m_rcBuffer.right )
	{
		m_cpBuffer.x = m_rcBuffer.left;
		m_cpBuffer.y++;
	}		
}

/*static*/ void BIOS::LCD::BufferEnd()
{
}

/*static*/ void BIOS::LCD::Shadow(int x1, int y1, int x2, int y2, unsigned int nColor)
{
	// rrggbbaa
	int nA_ = nColor >> 24;
	int nR_ = (nColor >> 16) & 0xff;
	int nG_ = (nColor >> 8) & 0xff;
	int nB_ = nColor & 0xff;

	for (int x=x1; x<x2; x++)
		for (int y=y1; y<y2; y++)
			if ( !_Round(min(x-x1, x2-x-1), min(y-y1, y2-y-1)) )
			{
				ui16 nOld = GetPixel(x, y);
				int nR = Get565R( nOld );
				int nG = Get565G( nOld );
				int nB = Get565B( nOld );

				nR += ( (nR_ - nR) * nA_ ) >> 8;
				nG += ( (nG_ - nG) * nA_ ) >> 8;
				nB += ( (nB_ - nB) * nA_ ) >> 8;

				PutPixel(x, y, RGB565RGB(nR, nG, nB));
			}
}


/*static*/ ui16 BIOS::KEY::GetKeys()
{
	int *pKeys = DEVICE->GetKeys();
	ui16 nKeys = 0;
	if ( pKeys[VK_LEFT] )
		nKeys |= KeyLeft;
	if ( pKeys[VK_RIGHT] )
		nKeys |= KeyRight;
	if ( pKeys[VK_UP] )
		nKeys |= KeyUp;
	if ( pKeys[VK_DOWN] )
		nKeys |= KeyDown;
	if ( pKeys[VK_RETURN] )
		nKeys |= KeyEnter;
	if ( pKeys[VK_BACK] )
		nKeys |= KeyEscape;
	if ( pKeys[VK_SPACE] )
		nKeys |= KeyFunction;
	if ( pKeys[VK_DELETE] )
		nKeys |= KeyFunction2;
	if ( pKeys[VK_F1] )
		nKeys |= KeyS1;
	if ( pKeys[VK_F2] )
		nKeys |= KeyS2;
	return nKeys;
}

/*static*/ ui8 _Round(int x, int y)
{
	const static ui8 r[] = 
	{
		1, 1, 1, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
	};
	if (x<8 && y<8)
		return r[y*8+x];
	return 0;
}


/*static*/ int _DrawChar(int x, int y, unsigned short clrf, unsigned short clrb, char ch)
{
	const unsigned char *pFont = BIOS::LCD::GetFont(ch);
	if (clrb == RGBTRANS)
	{
		for (ui8 _y=0; _y<14; _y++)
		{
			ui8 col = ~*pFont++;
	
			for (ui8 _x=0; _x<8; _x++, col <<= 1)
				if ( col & 128 )
					BIOS::LCD::PutPixel(x+_x, y+_y, clrf);
		}
	} else if (clrf == RGBTRANS)
	{
		for (ui8 _y=0; _y<14; _y++)
		{
			ui8 col = ~*pFont++;
	
			for (ui8 _x=0; _x<8; _x++, col <<= 1)
				if ( (col & 128) == 0 )
					BIOS::LCD::PutPixel(x+_x, y+_y, clrb);
		}
	} else
	{
		for (ui8 _y=0; _y<14; _y++)
		{
			ui8 col = ~*pFont++;
	
			for (ui8 _x=0; _x<8; _x++, col <<= 1)
				if ( col & 128 )
					BIOS::LCD::PutPixel(x+_x, y+_y, clrf);
				else
					BIOS::LCD::PutPixel(x+_x, y+_y, clrb);
		}
	}
	return 8;
}

/*static*/ DWORD FROM_565_TO_RGB(unsigned short clr565)
{
	unsigned char b = clr565 & 0x1F;
	clr565 >>= 5;
	unsigned char g = clr565 & 0x3F;
	clr565 >>= 6;
	unsigned char r = (unsigned char)clr565;
	r = r*255/0x1f;
	g = g*255/0x3f;
	b = b*255/0x1f;
	return RGB(r, g, b);
}

/*static*/ WORD FROM_RGB_TO_565(unsigned int clrrgb)
{
	int b = clrrgb&0xff;
	clrrgb >>= 8;
	int g = clrrgb&0xff;
	clrrgb >>= 8;
	int r = clrrgb&0xff;

	return RGB565RGB(r, g, b);
}

/*static*/ void BIOS::LCD::Buffer(int x, int y, unsigned short* pBuffer, int n)
{
	y += n;
	while (n--)
		PutPixel(x, --y, *pBuffer++);
}

/*static*/ ui32 BIOS::SYS::GetTick()
{
	return GetTickCount();
}

/*static*/ void BIOS::LCD::Line(int x1, int y1, int x2, int y2, unsigned short clr)
{
    // if x1 == x2 or y1 == y2, then it does not matter what we set here
    si16 delta_x(x2 - x1);
    si8 ix((delta_x > 0) - (delta_x < 0));
    delta_x = abs(delta_x) << 1;
 
    si16 delta_y(y2 - y1);
    si8 iy((delta_y > 0) - (delta_y < 0));
    delta_y = abs(delta_y) << 1;
 
	PutPixel( x1, y1, clr );
 
    if (delta_x >= delta_y)
    {
        // error may go below zero
        si16 error(delta_y - (delta_x >> 1));
 
        while (x1 != x2)
        {
            if (error >= 0)
            {
                if (error || (ix > 0))
                {
                    y1 += iy;
                    error -= delta_x;
                }
                // else do nothing
            }
            // else do nothing
 
            x1 += ix;
            error += delta_y;
 
			PutPixel( x1, y1, clr );
        }
    }
    else
    {
        // error may go below zero
        si16 error(delta_x - (delta_y >> 1));
 
        while (y1 != y2)
        {
            if (error >= 0)
            {
                if (error || (iy > 0))
                {
                    x1 += ix;
                    error -= delta_y;
                }
                // else do nothing
            }
            // else do nothing
 
            y1 += iy;
            error += delta_x;
			PutPixel( x1, y1, clr );
        }
    }
}

// ADC
/*static*/ void BIOS::ADC::Init()
{
}

bool bADCEnabled = false;

/*static*/ void BIOS::ADC::Enable(bool bEnable)
{
	bADCEnabled = bEnable;
}

/*static*/ bool BIOS::ADC::Enabled()
{
	return bADCEnabled;
}

/*static*/ void BIOS::ADC::Configure(ui8 nACouple, ui8 nARange, ui16 nAOffset, ui8 nBCouple, ui8 nBRange, ui16 nBPosition, float fTimePerDiv)
{
}

BOOL bADCReady = FALSE;
/*static*/ unsigned char BIOS::ADC::Ready()
{
	static long lLast = 0;
	if ( !bADCEnabled )
		return false;
	if ( bADCReady )
		return bADCReady;
	long lTick = GetTickCount();
	if (lLast == 0)
		lLast = GetTickCount();
	bADCReady = (GetTickCount()-lLast) > 100;
	if (bADCReady)
		lLast = lTick;
	return bADCReady;
}

/*static*/ unsigned long BIOS::ADC::Get()
{
	static long lCounter = 0;
	if (lCounter == 4096)
		lCounter = 0;

	FLOAT fa = 0.035f + (GetTickCount()%20000)/20000.0f*0.04f;
	fa = lCounter*0.0001f;
	FLOAT a = cos(lCounter*(fa)*10)*0.8f+0.2f;
	FLOAT b = sin(lCounter*0.011f+1)*0.5f;
	unsigned long da = (ui32)((a+1.0f)*127);
	unsigned long db = (ui32)((b+1.0f)*127);
	da |= rand()&3;
	lCounter++;
	return da | (db<<8);
}

#define ADCSIZE 4096
unsigned long g_ADCMem[ADCSIZE];

/*static*/ void BIOS::ADC::Copy(int nCount)
{
	_ASSERT( nCount <= ADCSIZE );
	for ( int i = 0; i < ADCSIZE; i++ )
	{
		ui32 nOld = g_ADCMem[i] & 0xff000000;
		g_ADCMem[i] = ( Get() & 0x00ffffff ) | nOld;
	}
}

/*static*/ unsigned long BIOS::ADC::GetCount()
{
	return ADCSIZE;
}

/*static*/ BIOS::ADC::TSample& BIOS::ADC::GetAt(int i)
{
	_ASSERT( i >= 0 && i < ADCSIZE );
	return g_ADCMem[i];
}


/*static*/ void BIOS::ADC::Restart()
{
	bADCReady = FALSE;
}

/*static*/ void BIOS::GEN::ConfigureDc(ui16 nData)
{
}

/*static*/ void BIOS::GEN::ConfigureSq(ui16 psc, ui16 arr, ui16 ccr)
{
}

/*static*/ void BIOS::GEN::ConfigureWave(ui16* pData, ui16 cnt)
{
}

/*static*/ void BIOS::GEN::ConfigureWaveRate(ui16 arr)
{
}

/*static*/ void BIOS::ADC::ConfigureTrigger(ui16 nTThreshold, ui16 nVThreshold, ui8 nSource, ui8 nType)
{
}

///*static*/ void ADC::Configure(ui8 nACouple, ui8 nARange, ui16 nAOffset, ui8 nBCouple, ui8 nBRange, ui16 nBPosition, ui16 nTimePsc, ui16 nTimeArr)
//{
//}

/*static*/ PVOID BIOS::DSK::GetSharedBuffer()
{
	static ui8 pSectorBuffer[FILEINFO::SectorSize];
	return (PVOID)pSectorBuffer;
}

/*static*/ BOOL BIOS::DSK::Open(FILEINFO* pFileInfo, const char* strName, ui8 nIoMode)
{
	char name[32];
	memcpy(name, strName, 8);
	name[8] = 0;
	while ( name[strlen(name)-1] == ' ' )
		name[strlen(name)-1] = 0;
	strcat(name, ".");
	strcat(name, strName+8);

	if ( nIoMode == BIOS::DSK::IoRead )
		pFileInfo->f = fopen(name, "rb");
	if ( nIoMode == BIOS::DSK::IoWrite )
		pFileInfo->f = fopen(name, "wb");
	pFileInfo->nSectors = 0;
	return pFileInfo->f != NULL && pFileInfo->f != INVALID_HANDLE_VALUE;
}

/*static*/ BOOL BIOS::DSK::Read(FILEINFO* pFileInfo, ui8* pSectorData)
{
	fread( pSectorData, 512, 1, pFileInfo->f );
	return TRUE;
}

/*static*/ BOOL BIOS::DSK::Write(FILEINFO* pFileInfo, ui8* pSectorData)
{
	fwrite( pSectorData, 512, 1, pFileInfo->f );
	pFileInfo->nSectors++;
	return TRUE;
}

/*static*/ BOOL BIOS::DSK::Close(FILEINFO* pFileInfo, int nSize /*=-1*/)
{
	if ( pFileInfo->nMode == BIOS::DSK::IoWrite )
	{
		if ( nSize != -1 )
		{
			// not supported...
		}
	}
	fclose( pFileInfo->f );
	return TRUE;
}

void BIOS::SYS::Beep(int)
{
}

/*static*/ void BIOS::DBG::Print(const char * format, ...)
{
	static int px = 0;
	static int py = 0;

	char buf[128];
	char* bbuf = buf; 

        va_list args;
        
        va_start( args, format );
        print( &bbuf, format, args );

	OutputDebugString( buf );
	for ( bbuf = buf; *bbuf; bbuf++ )
	{
		if ( *bbuf == '\n' || px >= 400-8 )
		{
			px = 0;
			py+=14;
			if (py > 240-16)
				py = 0;
			continue;
		}
		px += _DrawChar(px, py, RGB565(ffffff), RGB565(0000B0), *bbuf);
	}
}

#if 0
/*static*/ int BIOS::LCD::Print (int x, int y, unsigned short clrf, unsigned short clrb, const char *str)
{
	if (!str || !*str)
		return 0;
	int _x = x;
	for (;*str; str++)
	{
		if (*str == '\n')
		{
			x = _x;
			y += 16;
			continue;
		}
		x += _DrawChar(x, y, clrf, clrb, *str);
	}
	return x - _x;
}

#endif
/*static*/ int BIOS::DBG::sprintf(char* buf, const char * format, ...)
{
	char* bbuf = buf; 

        va_list args;
        
        va_start( args, format );
	return print( &bbuf, format, args );
}

/*static*/ void BIOS::SYS::DelayMs(unsigned short l)
{
	Sleep(l);
}


  /*static*/ void BIOS::FFT::Window(si16* arrSignal, const ui16*arrWindowHalf, int n)
  {
    short *p = arrSignal;
    ui32 tmp;
    for (int i=0; i<n; i++)
    {
      tmp = *p;
      if (i<n/2)
        tmp *= arrWindowHalf[i];
      else
        tmp *= arrWindowHalf[n-i-1];
      tmp >>= 16;
      *p = (short)tmp;
      p+= 2;
    }
    
  }

/*static*/ void BIOS::FFT::Convert( si16* arrOutput, si16* arrInput, int n )
  {
_ASSERT(!!!"Not implemented");
  }

/*static*/ ui32 BIOS::FFT::Sqrt( ui32 value )
{
int nv, v = value>>1, c = 0;
if (!v)
return value;
do
{
nv = (v + value/v)>>1;
if (abs(v - nv) <= 1) // I have an available fast absolute value in this forum. If you have it. use the next one.
return nv;
v = nv;
}
while (c++ < 25);
return nv;
}


/*static*/ int BIOS::SYS::GetBattery()
{
	return 85;
}


/*static*/ void BIOS::SERIAL::Init()
{

}

/*static*/ void BIOS::SERIAL::Send(const char* strBuf)
{
	OutputDebugString( "Serial:" );
	OutputDebugString( strBuf );
	OutputDebugString( "\n" );
}

BOOL Is64BitWindows()
{
#if defined(_WIN64)
 return TRUE;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
 // 32-bit programs run on both 32-bit and 64-bit Windows
 // so must sniff
 BOOL f64 = FALSE;
 return IsWow64Process(GetCurrentProcess(), &f64) && f64;
#else
 return FALSE; // Win64 does not support Win16
#endif
}

/*static*/ int BIOS::SERIAL::Getch()
{
	return -1;
}
/*static*/ void BIOS::SERIAL::Putch(char ch)
{
}
		
const char* BIOS::VER::GetHardwareVersion()
{
	return Is64BitWindows() ? "x64" : "x86";
}

const char* BIOS::VER::GetSystemVersion()
{
	return "WIN32";
}

const char* BIOS::VER::GetFpgaVersion()
{
	return "0.00";
}

const char* BIOS::VER::GetDfuVersion()
{
	return "0.00";
}

ui32 BIOS::VER::GetSerialNumber()
{
	return 0x0006ab0;
}

ui32 BIOS::VER::GetDisplayType()
{
	return ToDword('v', 'g', 'a', 0);
}

void BIOS::VER::DrawLogo(int x, int y)
{
}

/*static*/ void BIOS::SYS::SetBacklight(int nLevel) // 0..100
{
}

/*static*/ void BIOS::SYS::SetVolume(int nLevel) // 0..100
{
}
/*
void BIOS::ADC::ConfigureBuffer(int nLength)
{
	g_nBufferLen = nLength;
}

void BIOS::ADC::GetBufferRange(int& nBegin, int& nEnd)
{
	nBegin = 8;
	nEnd = g_nBufferLen;
}*/

/*static*/ BIOS::ADC::EState BIOS::ADC::GetState()
{
	int nAux = 0;
	if ( 0 )
		nAux |= BIOS::ADC::Start;
	if ( 0 )
		nAux |= BIOS::ADC::Empty;
	if ( 0 )
		nAux |= BIOS::ADC::Full;

	return (BIOS::ADC::EState)(nAux);
}

int BIOS::SYS::Get(int, int)
{
	return 0;
}

void BIOS::SYS::Set(int, int)
{
}

int BIOS::ADC::GetPointer()
{
	return 2048; //4096;
}

int BIOS::SYS::GetTemperature()
{
	return 22;
}

void BIOS::SYS::Standby(bool)
{
}

const void* BIOS::LCD::GetCharRom()
{
	return (void*)font;
}