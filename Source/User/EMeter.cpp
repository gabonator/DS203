#include "EMeter.h"
#ifdef ENABLE_EMETER

void CWndEMeter::OnWave( int nValue )
{
	#define TOFIXED(a) ((a)<<10)
	#define FROMFIXED(a) ((a)>>10)
	static int nLastVal = 0;
	static int x=0;
	static int nFloatAvg = 0;
	static int nFloatMin = 0;
	const int nFactor = 240;
	const int nFactorMin = 240;
	const int nFactorCharge = 240;
	const int nFactorDischarge = 252;
	static int nCounter = -1;
	static int nTotal = 0;

	static int nLastTick = 0;
	static int nLastTickImp = 0;
	static int nSnr = 0;
	static int nRising = 0;
	
	// ---

	if ( nFloatAvg == 0 )
		nFloatAvg = TOFIXED(nValue);
	else
	{
		if ( nValue > TOFIXED(nFloatAvg) )
			nFloatAvg = ( nFloatAvg * nFactorCharge + TOFIXED(nValue) * (256-nFactorCharge) ) / 256;
		else
			nFloatAvg = ( nFloatAvg * nFactorDischarge + TOFIXED(nValue) * (256-nFactorDischarge) ) / 256;
	}

	bool bSignal = FROMFIXED(nFloatAvg) > nValue + 15;

	static bool bLastSignal = false;

	if ( bSignal )
	{
		BIOS::SYS::Beep(100);
		if (nFloatMin == 0 )
			nFloatMin = TOFIXED(nValue);
		else 
			nFloatMin = ( nFloatMin * nFactorMin + TOFIXED(nValue) * (256-nFactorMin) ) / 256;
	}

	if ( !bSignal && nFloatMin != 0 )
	{
		int nCurSnr = nValue - FROMFIXED(nFloatMin);
		if ( nSnr == 0 )
			nSnr = TOFIXED(nCurSnr);
		else
			nSnr = ( nSnr * nFactor + TOFIXED(nCurSnr) * (256-nFactor) ) / 256;
	}

	if ( nCounter != -1 )
		nCounter++;

	if ( bSignal && !bLastSignal )
	{
		nRising = BIOS::SYS::GetTick();
		nCounter = 0;
	}

	if ( !bSignal && bLastSignal )
	{
		if ( nCounter >= 3 && nCounter < 15 )
		{
			nTotal++;
			int nTick = BIOS::SYS::GetTick();
			// nCounter = dlzka imulzu
			BIOS::LCD::Printf(8, 30, RGB565(ffffff), RGB565(000000), "len=%d (%dms), snr=%d, total=%d   ",
				nCounter, nTick-nRising, FROMFIXED(nSnr), nTotal);

			float fReal = nTotal/1600.0f;
			BIOS::LCD::Printf(8, 30+16, RGB565(ffffff), RGB565(000000), "Energy = %3f kWh  ",
				fReal);

			if ( nLastTick == 0 )
			{
				nLastTick = nTick;
				nLastTickImp = nTotal;
			} 
			if ( nTick - nLastTick >= 60000 )
			{
				// one minute passed - do not count with the current impulse
				float fPulsePerSecond = (float)(nTotal - 1 - nLastTickImp) * 1000.0f / (nTick - nLastTick);
				float fPulsePerHour = fPulsePerSecond * 3600.0f;
	
				// 1600 imp => 1kWh
				// 1600 imp/hour = 1kW
				float fPower = fPulsePerHour / 1600.0f;
				BIOS::LCD::Printf(8, 30+2*16, RGB565(ffffff), RGB565(000000), "\x7f Minute = %3f kW (%dp)  ",
					fPower, nTotal - 1 - nLastTickImp);

				nLastTick = nTick;
				nLastTickImp = nTotal;
			}

			int lPassed = BIOS::SYS::GetTick() - nTick;
			BIOS::LCD::Printf(330, 30, RGB565(808080), 0, "%d ms  ", lPassed);
		} else
		{
			int nTick = BIOS::SYS::GetTick();
			BIOS::LCD::Printf(40, 40, RGB565(ff0000), RGB565(000000), "invalid signal! len=%d (%dms)  ",
				nCounter, nTick-nRising);
		}
	}


	bLastSignal = bSignal;

	BIOS::LCD::Bar(x, BIOS::LCD::LcdHeight-128, x+1, BIOS::LCD::LcdHeight, bSignal ? RGB565(808080) : RGB565(202020));
	BIOS::LCD::PutPixel(x, BIOS::LCD::LcdHeight - FROMFIXED(nFloatAvg)/2-1, RGB565(00ff00));
	BIOS::LCD::PutPixel(x, BIOS::LCD::LcdHeight - FROMFIXED(nFloatAvg)/2-2, RGB565(00ff00));
	BIOS::LCD::PutPixel(x, BIOS::LCD::LcdHeight - FROMFIXED(nFloatMin)/2-1, RGB565(0000b0));
	BIOS::LCD::PutPixel(x, BIOS::LCD::LcdHeight - FROMFIXED(nFloatMin)/2-2, RGB565(0000b0));
	if ( nSnr > 0 )
	{
		BIOS::LCD::PutPixel(x, BIOS::LCD::LcdHeight - FROMFIXED(nSnr)/2-1, RGB565(ff00ff));
		BIOS::LCD::PutPixel(x, BIOS::LCD::LcdHeight - FROMFIXED(nSnr)/2-2, RGB565(ff00ff));
	}
	BIOS::LCD::PutPixel(x, BIOS::LCD::LcdHeight - nLastVal/2-1, RGB565(ffff00));
	BIOS::LCD::PutPixel(x, BIOS::LCD::LcdHeight - nLastVal/2-2, RGB565(ffff00));

	if ( ++x >= BIOS::LCD::LcdWidth )
		x = 0;

	BIOS::LCD::Bar(x, BIOS::LCD::LcdHeight-128, x+1, BIOS::LCD::LcdHeight, RGB565(ff2020));
		
	nLastVal = nValue;
}

void CWndEMeter::OnPaint()
{
	int x = 140;
	ui16 clr = RGB565(404040);
	int y = BIOS::LCD::LcdHeight - 128 - 16;
	BIOS::LCD::Bar(0, BIOS::LCD::LcdHeight - 128 - 18, BIOS::LCD::LcdWidth, BIOS::LCD::LcdHeight - 128, clr);
	x += 8 * BIOS::LCD::Printf(x, y, RGB565(000000), RGB565(808080), " pulse ");
	x += 8 * BIOS::LCD::Printf(x, y, RGB565(ffff00), clr, " CH1 ");
	x += 8 * BIOS::LCD::Printf(x, y, RGB565(00ff00), clr, "idle-avg ");
	x += 8 * BIOS::LCD::Printf(x, y, RGB565(0000B0), clr, "peak ");
	x += 8 * BIOS::LCD::Printf(x, y, RGB565(ff00ff), clr, "SNR ");
}

#endif