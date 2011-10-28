#include <string.h>

//#define min(a, b) (((a)<(b))?(a):(b))
#ifndef abs
#	define abs(a) ((a)>=0?(a):(-a))
#endif
#define xstrlen(a) 5
/*static*/ u8 _Round(int x, int y);
/*static*/ int _DrawChar(int x, int y, unsigned short clrf, unsigned short clrb, char ch);


CRect m_rcBuffer;
CPoint m_cpBuffer;

#include "LowLcd.h"

/*static*/ void BIOS::LCD::Init()
{
  LCD_Init();
}

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

/*static*/ int BIOS::LCD::Print (int x, int y, unsigned short clrf, unsigned short clrb, char *str)
{
	return BIOS::LCD::Print (x, y, clrf, clrb, (const char*)str);
}

/*static*/ int BIOS::LCD::Printf (int x, int y, unsigned short clrf, unsigned short clrb, const char * format, ...)
{
//	_ASSERT(0);
	return 0;
}

/*static*/ void BIOS::LCD::Clear(unsigned short clr)
{
  __Point_SCR(0, 0); 
  __LCD_Fill(&clr,240*200);//Clear Half Screen
  __LCD_DMA_Ready();
  __LCD_Fill(&clr,240*200);//Clear Half Screen
  __LCD_DMA_Ready();
}

/*static*/ void BIOS::LCD::RoundRect(int x1, int y1, int x2, int y2, unsigned short clr)
{
	for (int y=y1; y<y2; y++)
	{
		if (y == y1+3)
		{
			BIOS::LCD::Bar(x1, y, x2, y2-3, clr);
			y = y2-3;
		}
		for (int x=x1; x<x2; x++)
			if ( !_Round(min(x-x1, x2-x-1), min(y-y1, y2-y-1)) )
				PutPixel(x, y, clr);
	}
}

/*static*/ void BIOS::LCD::RoundRect(const CRect& rc, unsigned short clr)
{
	RoundRect(rc.left, rc.top, rc.right, rc.bottom, clr);
} 

/*static*/ void BIOS::LCD::PutPixel(int x, int y, unsigned short clr)
{
  __Point_SCR(x, 239-y);
  __LCD_SetPixl(clr);
}

/*static*/ void BIOS::LCD::Rectangle(const CRect& rc, unsigned short clr)
{
	BIOS::LCD::Bar(rc.left, rc.top, rc.right, rc.top+1, clr);
	BIOS::LCD::Bar(rc.left, rc.bottom-1, rc.right, rc.bottom, clr);
	BIOS::LCD::Bar(rc.left, rc.top, rc.left+1, rc.bottom, clr);
	BIOS::LCD::Bar(rc.right-1, rc.top, rc.right, rc.bottom, clr);
}

/*static*/ void BIOS::LCD::Bar(int x1, int y1, int x2, int y2, unsigned short clr)
{
  __LCD_Set_Block(x1, x2-1, 240-y2, 239-y1);
  __LCD_Fill(&clr,(x2-x1)*(y2-y1));
  __LCD_DMA_Ready();
  __LCD_Set_Block(0, 399, 0, 239);
}

/*static*/ void BIOS::LCD::Pattern(int x1, int y1, int x2, int y2, const ui16 *pat, int l)
{
	const ui16* patb = pat;
	const ui16* pate = patb + l;
	
  __LCD_Set_Block(x1, x2-1, 240-y2, 239-y1);

	for (int y=y1; y<y2; y++)
		for (int x=x1; x<x2; x++)
		{
			__LCD_SetPixl(*pat);
			if (++pat == pate) 
				pat = patb;
		}
  __LCD_Set_Block(0, 399, 0, 239);
}

/*static*/ void BIOS::LCD::Bar(const CRect& rc, unsigned short clr)
{
	Bar(rc.left, rc.top, rc.right, rc.bottom, clr);
} 

/*static*/ void BIOS::LCD::BufferBegin(const CRect& rc, ui8 nMode)
{
	m_rcBuffer = rc;
	m_cpBuffer = rc.TopLeft();
}

/*static*/ void BIOS::LCD::BufferPush(ui16 clr)
{
	BIOS::LCD::PutPixel( m_cpBuffer.x, m_cpBuffer.y, clr );
	if ( ++m_cpBuffer.x >= m_rcBuffer.right )
	{
		m_cpBuffer.x = m_rcBuffer.left;
		m_cpBuffer.y++;
	}		
}

/*static*/ void BIOS::LCD::BufferEnd()
{
}

/*static*/ int BIOS::LCD::Draw(int x, int y, unsigned short clrf, unsigned short clrb, const char *p)
{
	int h = *p++;
//	y+=h-1;
	for (int _x=0; _x<80; _x++)
		for (int _y=0; _y<h; _y++)
		{
			unsigned char d = *p++; //[_x*h+_y];
			if (!d)
				return _x;
			if ( d & 4 )
			{
				if (clrf != RGBTRANS)
					PutPixel(x+_x, y+_y, clrf);
			}
			else
			{
				if (clrb != RGBTRANS)
					PutPixel(x+_x, y+_y, clrb);
			}
		}
	return 8;
}

/*static*/ void BIOS::LCD::Buffer(int x, int y, unsigned short* pBuffer, int n)
{
  __Row_DMA_Ready();
  __Point_SCR(x, 240-y-n);
  __LCD_Copy(pBuffer, n);
  __LCD_DMA_Ready();
}

// -----------------------------------------------------------------------

/*static*/ u8 _Round(int x, int y)
{
	const static u8 r[] = 
	{
		1, 1, 1, 0,
		1, 0, 0, 0,
		1, 0, 0, 0,
		0, 0, 0, 0
	};
	if ( !(x&~3) && !(y&~3) )
		return r[(y<<2)|x];
	return 0;
}

/*static*/ int _DrawChar(int x, int y, unsigned short clrf, unsigned short clrb, char ch)
{
	const unsigned short *pFont = Get_TAB_8x14(ch);
	if (clrb == RGBTRANS)
	{
//		ui16 _x;
//		ui8 _y;
/*
		ui16 tempbuf[14];
		
		__LCD_Set_Block(x, x+1, 240-(y+14), 239-y);
		LCD_WR_Ctrl(0x2E);

		tempbuf[0] = LCD_PORT; 
		for (_y=0; y<14; y++)
		    tempbuf[_y] = LCD_PORT; 

		__LCD_Set_Block(x, x+8-1, 240-(y+14), 239-y);
*/
		for (ui8 _x=0; _x<8; _x++)
		{
			const unsigned short col = *pFont++;
	
			for (ui8 _y=0; _y<14; _y++)
				if ( (col & (2<<(14-_y))) )
					BIOS::LCD::PutPixel(x+_x, y+_y, clrf);
/*

			for (_y=0; _y<14; _y++)
				if ( (col & (2<<_y)) )
					__LCD_SetPixl( clrf);
				else
					__LCD_SetPixl(tempbuf[_y]);
*/
		}
//		__LCD_Set_Block(0, 399, 0, 239);
	} else if (clrf == RGBTRANS)
	{
		for (int _x=0; _x<8; _x++)
		{
			const unsigned short col = *pFont++;
	
			for (int _y=0; _y<14; _y++)
				if ( !(col & (2<<(14-_y))) )
					BIOS::LCD::PutPixel(x+_x, y+_y, clrb);
		}
	} else
	{
		for (int _x=0; _x<8; _x++)
		{
			const unsigned short col = *pFont++;
	
			for (int _y=0; _y<14; _y++)
				if ( (col & (2<<(14-_y))) )
					BIOS::LCD::PutPixel(x+_x, y+_y, clrf);
				else
					BIOS::LCD::PutPixel(x+_x, y+_y, clrb);
		}
	}
	return 8;
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
