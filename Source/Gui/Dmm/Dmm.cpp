#include "Dmm.h"
#include <Source/Core/Settings.h>
#include <string.h>
#include <Source/Core/Utils.h>

void CWndDmm::OnPaint()
{
	char strDisplay[16];
	CSettings::Calibrator::FastCalc fastCalc;
	Settings.CH1Calib.Prepare( &Settings.CH1, fastCalc );

	float fCorrect = Settings.CH1Calib.Correct( fastCalc, m_fAverage );
	float fDisplay = Settings.CH1Calib.Voltage( fastCalc, m_fAverage );
	BIOS::DBG::sprintf( strDisplay, "%f mV", fDisplay );

	ui16 cOn = RGB565(11bbff);
	ui16 cOff = RGB565(050505);
	ui16 cClr = RGB565(000000);

	if ( bRefresh )
	{
		BIOS::LCD::Bar( m_rcClient, cClr );
		bRefresh = false;
	}

	BIOS::LCD::Printf( 8, 220, RGB565(808080), RGB565(ffffff), "adc=%3f cal=%3f var=%3f range=%s ",
		m_fAverage, fCorrect, m_fVariance, CSettings::AnalogChannel::ppszTextResolution[Settings.CH1.Resolution]);

	if ( m_fAverage < 16 || m_fAverage > 240 )
		strcpy(strDisplay, " Err");

	const char* pDisplay = strDisplay;
	CUtils::Print( 20, 20, cOn, cClr, 4, strDisplay);

	CUtils::Printf( 80, 80, cOn, cClr, 2, "%f mV", fDisplay);
	int i;
	int width = 8;
	int size = 30;
	int space = 1;
	for (i=0; *pDisplay && i < 4; i++, pDisplay++)
	{
		int nDigit = *pDisplay;
		if ( pDisplay[1] == '.' )
		{
			nDigit |= 128;
			pDisplay++;
		}
 		DrawDigit(20+i*(size+2*width+7*space), 120, width, size, space, nDigit, cOn, cOff);
	}
	for ( ; i < 4; i++ )
		DrawDigit(20+i*(size+2*width+7*space), 120, width, size, space, 0, cClr, cClr);
}

void DrawTriangle(int x, int y, int size, bool half, ui8 quadrant, ui16 clr)
{
	if(half)
	{
		for(int xPos = 0 ; xPos < size ; xPos++)
		{
			int yStart;
			if(quadrant == 0 || quadrant == 3)
			{
				yStart = 0;
			}
			else if (quadrant == 1)
			{
				yStart = size - xPos - 1;
			}
			else // if(quadrant == 2)
			{
				yStart = xPos;
			}
			int yMax;
			if(quadrant == 1 || quadrant == 2)
			{
				yMax = size;
			}
			else if(quadrant == 0)
			{
				yMax = xPos;
			}
			else // if(quadrant == 3)
			{
				yMax = size - xPos - 1;
			}
			for(int yPos = yStart ; yPos < yMax ; yPos++)
			{
				BIOS::LCD::PutPixel(x+xPos,y+yPos,clr);
			}
		}
	}
	else
	{
		int halfSize = size / 2;
		if(quadrant == 0 || quadrant == 2)
		{
			for(int xPos = 0 ; xPos < halfSize ; xPos++)
			{
				int yStart = (quadrant == 0) ? 0 : halfSize-xPos;
				int yMax = (quadrant == 0) ? xPos : halfSize;
				for(int yPos = yStart ; yPos < yMax ; yPos++)
				{
					BIOS::LCD::PutPixel(x+xPos,y+yPos,clr);
				}
			}
			for(int xPos = 0 ; xPos < halfSize ; xPos++)
			{
				int yStart = (quadrant == 0) ? 0 : xPos;
				int yMax = (quadrant == 0) ? halfSize-xPos : halfSize;
				for(int yPos = yStart ; yPos < yMax ; yPos++)
				{
					BIOS::LCD::PutPixel(x+halfSize+xPos,y+yPos,clr);
				}
			}
		}
		else // if(quadrant == 1 || quadrant == 3)
		{
			for(int yPos = 0 ; yPos < halfSize ; yPos++)
			{
				int xStart = (quadrant == 1) ? halfSize-yPos : 0;
				int xMax = (quadrant == 1) ? halfSize : yPos;
				for(int xPos = xStart ; xPos < xMax ; xPos++)
				{
					BIOS::LCD::PutPixel(x+xPos,y+yPos,clr);
				}
			}
			for(int yPos = 0 ; yPos < halfSize ; yPos++)
			{
				int xStart = (quadrant == 1) ? yPos : 0;
				int xMax = (quadrant == 1) ? halfSize : halfSize-yPos;
				for(int xPos = xStart ; xPos < xMax ; xPos++)
				{
					BIOS::LCD::PutPixel(x+xPos,y+halfSize+yPos,clr);
				}
			}
		}
	}
}

void _DrawDigitHT(int x, int y, int width, int size, ui16 clr)
{
	if ( clr == RGB565(ff00ff) )
		return;
	DrawTriangle(x,y,width,true,0,clr);
	BIOS::LCD::Bar( x+width, y, x+size, y+width, clr ); 
	DrawTriangle(x+size,y,width,true,3,clr);
}

void _DrawDigitHC(int x, int y, int width, int size, ui16 clr)
{
	if ( clr == RGB565(ff00ff) )
		return;
	DrawTriangle(x,y,width,false,1,clr);
	BIOS::LCD::Bar( x+width/2, y, x+size+width/2, y+width, clr ); 
	DrawTriangle(x+size+width/2,y,width,false,3,clr);
}

void _DrawDigitHB(int x, int y, int width, int size, ui16 clr)
{
	if ( clr == RGB565(ff00ff) )
		return;
	DrawTriangle(x,y,width,true,1,clr);
	BIOS::LCD::Bar( x+width, y, x+size, y+width, clr ); 
	DrawTriangle(x+size,y,width,true,2,clr);
}

void _DrawDigitVL(int x, int y, int width, int size, ui16 clr)
{
	if ( clr == RGB565(ff00ff) )
		return;
	DrawTriangle(x,y,width,true,2,clr);
	BIOS::LCD::Bar( x, y+width, x+width, y+size, clr ); 
	DrawTriangle(x,y+size,width,true,3,clr);
}

void _DrawDigitVR(int x, int y, int width, int size, ui16 clr)
{
	if ( clr == RGB565(ff00ff) )
		return;
	DrawTriangle(x,y,width,true,1,clr);
	BIOS::LCD::Bar( x, y+width, x+width, y+size, clr ); 
	DrawTriangle(x,y+size,width,true,0,clr);
}

void _DrawDot0(int x, int y, int width, int size, int space, ui16 clr)
{
	if ( clr == RGB565(ff00ff) )
		return;
	BIOS::LCD::Bar( x, y, x+(width/4)*3, y+(width/4)*3, clr );
}

void CWndDmm::DrawDigit(int x, int y, int width, int size, int space, int nDigit, ui16 clrOn, ui16 clrOff)
{
	ui32 decoder[] =
	{0x1111110, 0x0110000, 0x1101101, 0x1111001, 0x0110011, 0x1011011, 0x1011111, 0x1110000, 0x1111111, 0x1111011};
	ui32 dec = 0;
	if ( (nDigit&127) >= '0' && (nDigit&127) <= '9' )
	{
		dec = decoder[nDigit&15];
		if ( nDigit & 128 )
			dec |= 0x10000000;
	} else
	{
		switch (nDigit)
		{
		case ' ': dec = 0; break;
		case '?': dec = 0; break;
		case '-': dec = 0x0000001; break;
		case 'E': dec = 0x1001111; break;
		case 'r': dec = 0x0000101; break;
		}
	}

	#define EN(n) ( dec>>(28-n*4)&1 ) ? RGB565(ff00ff) : clrOff
 	_DrawDigitHT( x+space, y, width, size, EN(1) ); // horny
	_DrawDigitHC( x+space, y+width/2+size+2*space-1, width, size, EN(7) ); // stredny
	_DrawDigitHB( x+space, y+size*2+width+4*space, width, size, EN(4) ); // dolny
	_DrawDigitVR( x+size+2*space, y+space, width, size, EN(2) );
	_DrawDigitVR( x+size+2*space, y+size+width+3*space, width, size, EN(3) );
	_DrawDigitVL( x, y+space, width, size, EN(6) ); 
	_DrawDigitVL( x, y+size+width+3*space, width, size, EN(5) ); 
	_DrawDot0(x+size+width+5*space, y+size*2+width+6*space, width, size, space, EN(0));
	#undef EN
	#define EN(n) ( dec>>(28-n*4)&1 ) ? clrOn : RGB565(ff00ff)
 	_DrawDigitHT( x+space, y, width, size, EN(1) ); // horny
	_DrawDigitHC( x+space, y+width/2+size+2*space-1, width, size, EN(7) ); // stredny
	_DrawDigitHB( x+space, y+size*2+width+4*space, width, size, EN(4) ); // dolny
	_DrawDigitVR( x+size+2*space, y+space, width, size, EN(2) );
	_DrawDigitVR( x+size+2*space, y+size+width+3*space, width, size, EN(3) );
	_DrawDigitVL( x, y+space, width, size, EN(6) ); 
	_DrawDigitVL( x, y+size+width+3*space, width, size, EN(5) ); 
	_DrawDot0(x+size+width+5*space, y+size*2+width+6*space, width, size, space, EN(0));
	#undef EN
}

void CWndDmm::OnWave()
{
	if (bTimer)
		return;
/*
#ifdef _WIN32
	m_fAverage = (float)167.27f;
	m_fVariance = 0.312f;	
	KillTimer();
	SetTimer(100);
	bTimer = true;
	return;
#endif
*/
	int nSum = 0;
	int nMax = -1, nMin = -1;
	for (int i=0; i</*BIOS::ADC::Length()*/ (int)BIOS::ADC::GetCount(); i++)
	{
		int nValue = BIOS::ADC::GetAt(i)&0xff;
		if ( nMax == -1 )
			nMax = nMin = nValue;
		else
		{
			nMax = max(nMax, nValue);
			nMin = min(nMin, nValue);
		}
		nSum += nValue;
	}

	m_fAverage = (float)nSum/4096.0f;
	m_fVariance = (nMax-nMin)/4.0f;	// pseudo variance :)
	KillTimer();
	SetTimer(100);
	bTimer = true;
}

/*virtual*/ void CWndDmm::OnTimer()
{
	bTimer = false;
	KillTimer();
	Invalidate();
}