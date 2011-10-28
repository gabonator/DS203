#pragma once
#include <stdio.h>
#include <crtdbg.h>
#include "device.h"
#include <Source/HwLayer/Types.h>
#include <Source/HwLayer/bios.h>
#define DEVICE CDevice::m_pInstance
#include "font.h"
#include <math.h>
//u16 *Get_TAB_8x14(u8 Code)

CRect m_rcBuffer;
CPoint m_cpBuffer;
int m_nKeys;

int _DrawChar(int x, int y, unsigned short clrf, unsigned short clrb, char ch);
DWORD FROM_565_TO_RGB(unsigned short clr565);
ui8 _Round(int x, int y);

void Assert(const char *msg, int n)
{
	_ASSERT_EXPR((0), NULL);
}

/*static*/ void BIOS::Init()
{
}

/*static*/ int BIOS::LCD::Printf (int x, int y, unsigned short clrf, unsigned short clrb, const char * format, ...)
{
	char buffer[256];
	va_list args;
	va_start (args, format);
	vsnprintf_s (buffer, 256, 255, format, args);
	int nAux = Print (x, y, clrf, clrb, buffer);
	va_end (args);
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

/*static*/ ui16 BIOS::KEY::GetKeys()
{
	DWORD dwDevKeys = DEVICE->GetKeys();
	ui16 nKeys = 0;
	if ( dwDevKeys == VK_LEFT )
		nKeys |= KeyLeft;
	if ( dwDevKeys == VK_RIGHT )
		nKeys |= KeyRight;
	if ( dwDevKeys == VK_UP )
		nKeys |= KeyUp;
	if ( dwDevKeys == VK_DOWN )
		nKeys |= KeyDown;
	if ( dwDevKeys == VK_RETURN )
		nKeys |= KeyEnter;
	if ( dwDevKeys == VK_BACK)
		nKeys |= KeyEscape;
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
	const unsigned short *pFont = Get_TAB_8x14(ch);
	for (int _y=0; _y<14; _y++)
		for (int _x=0; _x<8; _x++)
			BIOS::LCD::PutPixel(x+_x, y+_y, (pFont[_x] & (2<<(14-_y))) ? clrf : clrb);
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

/*static*/ void BIOS::LCD::Buffer(int x, int y, unsigned short* pBuffer, int n)
{
	y += n;
	while (n--)
		PutPixel(x, --y, *pBuffer++);
}

/*static*/ ui32 BIOS::GetTick()
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

/*static*/ void BIOS::ADC::Configure(ui8 nACouple, ui8 nARange, ui16 nAOffset, ui8 nBCouple, ui8 nBRange, ui16 nBPosition, ui16 nTimePsc, ui16 nTimeArr)
{
}

BOOL bADCReady = FALSE;
/*static*/ unsigned char BIOS::ADC::Ready()
{
	static long lLast = 0;
	if ( bADCReady )
		return bADCReady;
	long lTick = GetTickCount();
	if (lLast == 0)
		lLast = GetTickCount();
	bADCReady = (GetTickCount()-lLast) > 100;
	lLast = lTick;
	return bADCReady;
}

/*static*/ unsigned long BIOS::ADC::Get()
{
	static long lCounter = 0;
	if (lCounter == 4096)
		lCounter = 0;

	FLOAT a = cos(lCounter*0.035f)*0.8f+0.2f;
	FLOAT b = sin(lCounter*0.011f+1)*0.5f;
	unsigned long da = (ui32)((a+1.0f)*127);
	unsigned long db = (ui32)((b+1.0f)*127);
	da |= rand()&3;
	lCounter++;
	return da | (db<<8);
}

/*static*/ void BIOS::ADC::Restart()
{
	bADCReady = FALSE;
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
