#include "Annot.h"
#include <Source/Core/Utils.h>
#include <Source/Core/Settings.h>
#include <Source/Gui/MainWnd.h>

/*virtual*/ void CWndSpecAnnotations::OnPaint()
{
	// when the waveform cover whole range 0..255, the amplitude is 128,
	// we need only half of the amplitude range to show
	float fMax = Settings.Runtime.m_fCH1Res * 4.0f;
	char* strMax = CUtils::FormatVoltage(fMax, 5);

	CRect rcTarget( 40, 40, 300, 222 );

	if ( MainWnd.m_wndSpectrumMiniFD.IsVisible() )
		rcTarget = MainWnd.m_wndSpectrumMiniFD.m_rcClient;
	if ( MainWnd.m_wndSpectrumGraph.IsVisible() )
		rcTarget = MainWnd.m_wndSpectrumGraph.m_rcClient;
	if ( MainWnd.m_wndSpectrumMiniSG.IsVisible() )
		rcTarget = MainWnd.m_wndSpectrumMiniSG.m_rcClient;

	char* strUnits = NULL;
	int x = 2;

	if ( !MainWnd.m_wndSpectrumMiniSG.IsVisible() )
	{	
		strUnits = strstr( strMax, " " );
		if ( strUnits )
		{
			*strUnits = 0;
			strUnits++;
		}
		BIOS::LCD::Bar(x, rcTarget.top, rcTarget.left-1, rcTarget.top+16, RGB565(000000));
		x += BIOS::LCD::Print(x, rcTarget.top, RGB565(b0b0b0), RGB565(000000), strMax);
		x += 4;
		x += BIOS::LCD::Print(x, rcTarget.top, RGB565(808080), RGB565(000000), strUnits);
	}

	float fTime = Settings.Runtime.m_fTimeRes;
	if ( fTime == 0 )
		return;
	// calculate time corresponding to single sample
	fTime /= 30.0f; //CGraphWnd::BlkX
	// calculate sampling frequency;
	float fSample = 1.0f / fTime;
	// nyquist
	fSample *= 0.5f;
	char* strFreq = CUtils::FormatFrequency(fSample);
	BIOS::LCD::Bar(rcTarget.left, rcTarget.bottom+2, rcTarget.right, rcTarget.bottom+18, RGB565(000000));

	x = rcTarget.right - strlen(strFreq)*8;
	strUnits = strstr( strFreq, " " );
	if ( strUnits )
	{
		*strUnits = 0;
		strUnits++;
	}
	BIOS::LCD::Print(x-5*8, rcTarget.bottom + 2, RGB565(808080), RGB565(000000), "SPAN ");
	x += BIOS::LCD::Print(x, rcTarget.bottom + 2, RGB565(b0b0b0), RGB565(000000), strFreq);
	BIOS::LCD::Print(x, rcTarget.bottom + 2, RGB565(b0b0b0), RGB565(000000), " ");
	x += 4;
	x += BIOS::LCD::Print(x, rcTarget.bottom + 2, RGB565(808080), RGB565(000000), strUnits);


	fSample *= 0.2f; // 1/5 for single div
	strFreq = CUtils::FormatFrequency(fSample);
	x = rcTarget.left;
	x += BIOS::LCD::Print(x, rcTarget.bottom + 2, RGB565(808080), RGB565(000000), strFreq);
	x += BIOS::LCD::Draw(x, rcTarget.bottom + 2, RGB565(808080), RGB565(000000), CShapes::per_div);
}
