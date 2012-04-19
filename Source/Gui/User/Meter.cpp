#include "Meter.h"
#include <Source/Core/Settings.h>
#include <string.h>

void CWndUserMeter::OnPaint()
{
	char strDisplay[8];
	CSettings::Calibrator::FastCalc fastCalc;
	Settings.CH1Calib.Prepare( &Settings.CH1, fastCalc );

	float fCorrect = Settings.CH1Calib.Correct( fastCalc, m_fAverage );
	float fDisplay = Settings.CH1Calib.Voltage( fastCalc, m_fAverage );
	BIOS::DBG::sprintf( strDisplay, "%f", fDisplay );

	ui16 cOn = RGB565(000000);
	ui16 cOff = RGB565(e0e0e0);
	ui16 cClr = RGB565(ffffff);
	if ( bFirst )
	{
		BIOS::LCD::Bar( m_rcClient, cClr );
		//bFirst = false;
	}

	if ( GetFocus() == this )
		BIOS::LCD::Printf( 8, 220, RGB565(808080), RGB565(ffffff), "adc=%3f cal=%3f var=%3f range=%s ",
			m_fAverage, fCorrect, m_fVariance, CSettings::AnalogChannel::ppszTextResolution[Settings.CH1.Resolution]);
	else
	{
		strcpy(strDisplay, "????");
		BIOS::LCD::Print( 20, 220, RGB565(808080), RGB565(ffffff), " Press down to activate this window ");
	}
	if ( m_fAverage < 16 || m_fAverage > 240 )
		strcpy(strDisplay, " Err");
		


	const char* pDisplay = strDisplay;
	int i;
	for (i=0; *pDisplay && i < 4; i++, pDisplay++)
	{
		/*
		int nDigit = 15;
		if ( *pDisplay == '-' )
			nDigit = 10;
		else
		if ( *pDisplay >= '0' && *pDisplay <= '9' )
			nDigit = *pDisplay - '0';

		if ( pDisplay[1] == '.' )
		{
			nDigit |= 0x10;
			pDisplay++;
		}
		*/
		int nDigit = *pDisplay;
		if ( pDisplay[1] == '.' )
		{
			nDigit |= 128;
			pDisplay++;
		}
 		DrawDigit(20+i*90, 40, nDigit, cOn, cOff);
	}
	for ( ; i < 4; i++ )
		DrawDigit(20+i*90, 40, 0, cClr, cClr);
}

void DrawDigitH0(int x, int y, ui16 clr)
{
	/*
	for (int i=0; i<16; i++)
	{
		int nShift = abs(i-8);
		BIOS::LCD::Line( x+nShift, y+i, x+60-nShift, y+i, clr );
	}*/
	if ( clr == RGB565(ff00ff) )
		return;
	BIOS::LCD::Bar( x, y, x+60, y+16, clr ); 
}

void DrawDigitV0(int x, int y, ui16 clr)
{
/*
	for (int i=0; i<16; i++)
	{
		int nShift = abs(i-8);
		BIOS::LCD::Line( x+i, y+nShift, x+i, y+60-nShift, clr );
	}
	*/
	if ( clr == RGB565(ff00ff) )
		return;
	BIOS::LCD::Bar( x, y, x+16, y+60, clr ); 
}

void DrawDot0(int x, int y, ui16 clr)
{
	if ( clr == RGB565(ff00ff) )
		return;
	BIOS::LCD::Bar( x, y, x+12, y+12, clr );
}

void CWndUserMeter::DrawDigit(int x, int y, int nDigit, ui16 clrOn, ui16 clrOff)
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
	/*
	ui32 dec = 0;
	if ( (n & 0xf) >= 0 && (n & 0xf) <= 10 )
		dec = decoder[n&0xf];

	if (n&0x10)
		dec |= 0x10000000;
		*/
	#define EN(n) ( dec>>(28-n*4)&1 ) ? RGB565(ff00ff) : clrOff
 	DrawDigitH0( x, y, EN(1) ); // horny
	DrawDigitH0( x, y+62, EN(7) ); // stredny
	DrawDigitH0( x, y+124, EN(4) ); // dolny
	DrawDigitV0( x+53, y+9, EN(2) );
	DrawDigitV0( x+53, y+71, EN(3) );
	DrawDigitV0( x-9, y+9, EN(6) ); 
	DrawDigitV0( x-9, y+71, EN(5) ); 
	DrawDot0(x+70, y+128, EN(0));
	#undef EN
	#define EN(n) ( dec>>(28-n*4)&1 ) ? clrOn : RGB565(ff00ff)
 	DrawDigitH0( x, y, EN(1) ); // horny
	DrawDigitH0( x, y+62, EN(7) ); // stredny
	DrawDigitH0( x, y+124, EN(4) ); // dolny
	DrawDigitV0( x+53, y+9, EN(2) );
	DrawDigitV0( x+53, y+71, EN(3) );
	DrawDigitV0( x-9, y+9, EN(6) ); 
	DrawDigitV0( x-9, y+71, EN(5) ); 
	DrawDot0(x+70, y+128, EN(0));
	#undef EN
}

void CWndUserMeter::OnWave()
{
	int nSum = 0;
	int nMax = -1, nMin = -1;
	for (int i=0; i</*BIOS::ADC::Length()*/ 4096; i++)
	{
		int nValue = BIOS::ADC::Get()&0xff;
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
	SetTimer(100);
}

/*virtual*/ void CWndUserMeter::OnTimer()
{
	KillTimer();
	bFirst = false;
	Invalidate();
	bFirst = true;
}