#include "GraphOsc.h"
#include <Source/Gui/MainWnd.h>

void CWndOscGraph::_PrepareColumn( ui16 *column, ui16 n, ui16 clr )
{
	for (ui16 y=0; y<DivsY*BlkY; y++)
		column[y] = clr;
//		memset( column, clr, DivsY*BlkY*2 );
	if ( n == 0 )
		return;
	if ( (n % BlkX) == 0)
	{
		for (ui16 y=5; y<DivsY*BlkY; y += 5)
			column[y] = RGB565(808080);
	} else
	if ( (n%6) == 0 )
	{
		for (ui16 y=BlkY; y<DivsY*BlkY-1; y += BlkY)
			column[y] = RGB565(808080);
	}
}

/*virtual*/ void CWndOscGraph::Create(CWnd *pParent, ui16 dwFlags) 
{
	CWndGraph::Create( pParent, dwFlags | CWnd::WsListener );
}

/*virtual*/ void CWndOscGraph::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
	{
		Invalidate();
		UpdateResolutions();
		return;
	}
}

/*virtual*/ void CWndOscGraph::OnPaint()
{
	ui16 column[CWndGraph::DivsY*CWndGraph::BlkY];
	if ( !CWnd::m_rcOverlay.IsValid() )
	{
		CRect rc = m_rcClient;
		rc.Inflate( 1, 1, 1, 1 );
		BIOS::LCD::Rectangle( rc, RGB565(b0b0b0) );
	}

	ui16 clr1 = Settings.CH1.u16Color;
	ui16 clr2 = Settings.CH2.u16Color;
	ui8 en1 = Settings.CH1.Enabled == CSettings::AnalogChannel::_YES;
	ui8 en2 = Settings.CH2.Enabled == CSettings::AnalogChannel::_YES;

	ui8 bTrigger = (BIOS::GetTick() - Settings.Trig.nLastChange) < 5000;
	ui16 nTriggerTime = (Settings.Trig.nTime - Settings.Time.Shift);
	if (!bTrigger)
		nTriggerTime = -1;


	int nCut = CWnd::m_rcOverlay.IsValid() ? CWnd::m_rcOverlay.left - m_rcClient.left : m_rcClient.Width();
	int nCutTop = CWnd::m_rcOverlay.IsValid() ? CWnd::m_rcOverlay.bottom - m_rcClient.top : 0;
	if ( nCutTop >= m_rcClient.Height() )
		nCut = m_rcClient.Width();
	int nFirstTop = CWnd::m_rcOverlay.IsValid() ? CWnd::m_rcOverlay.top - m_rcClient.top : 0;

	int nMax = m_rcClient.Width(); 
	if ( CWnd::m_rcOverlay.IsValid() && CWnd::m_rcOverlay.left - m_rcClient.left <= 0 )
		return;

	CSettings::Calibrator::FastCalc Ch1fast, Ch2fast;
	Settings.CH1Calib.Prepare( &Settings.CH1, Ch1fast );
	Settings.CH2Calib.Prepare( &Settings.CH2, Ch2fast );

	// if window is outside...
	int nMaxIndex = BIOS::ADC::GetCount();
	bool bLines = true;
	int nPrev1 = -1, nPrev2 = -1;

	int nMarkerT1 = -1, nMarkerT2 = -1, nMarkerY1 = -1, nMarkerY2 = -1;
	bool bAreaT = false;

	if ( MainWnd.m_wndToolBar.GetCurrentLayout() == &MainWnd.m_wndMenuCursor )
		SetupMarkers( Ch1fast, Ch2fast, nMarkerT1, nMarkerT2, nMarkerY1, nMarkerY2 );
	if ( MainWnd.m_wndToolBar.GetCurrentLayout() == &MainWnd.m_wndMenuMeas )
		SetupSelection( bAreaT, nMarkerT1, nMarkerT2 );

	int nIndex = Settings.Time.Shift;
	for (ui16 x=0; x<nMax; x++, nIndex++)
	{
		int clrCol = (nTriggerTime != x) ? 0x0101 : 0x00;

		if ( bAreaT && nIndex > nMarkerT1 && nIndex < nMarkerT2 )
			clrCol = RGB565(101060);
		if ( nMarkerT1 == nIndex )
			clrCol = Settings.MarkT1.u16Color;
		if ( nMarkerT2 == nIndex )
			clrCol = Settings.MarkT2.u16Color;
			
		_PrepareColumn( column, x, clrCol );

		ui32 val = nIndex < nMaxIndex ?  BIOS::ADC::GetAt(nIndex) : 0;

		if ( en2 )
		{
			si16 ch2 = (ui8)((val>>8) & 0xff);
			//ch2 = CSettings::Correct(Settings.CH1.u16Position, ch2);
			ch2 = Settings.CH2Calib.Correct( Ch2fast, ch2 );
			if ( ch2 < 0 ) 
				ch2 = 0;
			if ( ch2 > 255 ) 
				ch2 = 255;
			ui16 y = (ch2*(DivsY*BlkY))>>8;
				
			if ( !bLines )
				column[y] = clr2;
			else
			{
				if ( nPrev2 == -1 )
					nPrev2 = y;
				for ( int _y = min(y, nPrev2); _y <= max(y, nPrev2); _y++)
					column[_y] = clr2;
				nPrev2 = y;
			}
		}

		if ( en1 )
		{
			si16 ch1 = (ui8)((val) & 0xff);
//				ch1 = CSettings::Correct(Settings.CH1.u16Position, ch1);
			ch1 = Settings.CH1Calib.Correct( Ch1fast, ch1 );
			if ( ch1 < 0 ) 
				ch1 = 0;
			if ( ch1 > 255 ) 
				ch1 = 255;
			ui16 y = (ch1*(DivsY*BlkY))>>8;

			if ( !bLines )
				column[y] = clr1;
			else
			{
				if ( nPrev1 == -1 )
					nPrev1 = y;
				for ( int _y = min(y, nPrev1); _y <= max(y, nPrev1); _y++)
					column[_y] = clr1;
				nPrev1 = y;
			}
		}

		if ( bTrigger && (x & 1) == 1 )
		{
			ui16 y = (Settings.Trig.nLevel*(DivsY*BlkY))>>8;
			column[y] = RGB565(404040);
		}
		if ( nMarkerY1 > 0 )
			column[nMarkerY1] = Settings.MarkY1.u16Color;
		if ( nMarkerY2 > 0 )
			column[nMarkerY2] = Settings.MarkY2.u16Color;
 
		if ( x <= nCut )
			BIOS::LCD::Buffer( m_rcClient.left + x, m_rcClient.top, column, DivsY*BlkY );
		else
		{
			if ( nCutTop > 0 )
				BIOS::LCD::Buffer( m_rcClient.left + x, m_rcClient.top + nCutTop, column, DivsY*BlkY - nCutTop);
			if ( nFirstTop > 0 )
				BIOS::LCD::Buffer( m_rcClient.left + x, m_rcClient.top, column + DivsY*BlkY - nFirstTop, nFirstTop );
		}
	}
}

void CWndOscGraph::SetupSelection( bool& bSelection, int& nMarkerT1, int& nMarkerT2 )
{
	bSelection = ( Settings.MarkT1.Mode == CSettings::Marker::_On && 
		Settings.MarkT2.Mode == CSettings::Marker::_On );
	
	if ( !bSelection )
		return;
	if ( Settings.MarkT1.nValue >= Settings.MarkT2.nValue )
		return;

	nMarkerT1 = Settings.MarkT1.nValue;
	nMarkerT2 = Settings.MarkT2.nValue;
}

void CWndOscGraph::SetupMarkers( CSettings::Calibrator::FastCalc& Ch1fast, CSettings::Calibrator::FastCalc& Ch2fast,
	int& nMarkerT1, int& nMarkerT2, int& nMarkerY1, int& nMarkerY2 )
{
	if ( Settings.MarkT1.Mode == CSettings::Marker::_On )
	{
		nMarkerT1 = Settings.MarkT1.nValue;
	}
	if ( Settings.MarkT2.Mode == CSettings::Marker::_On )
	{
		nMarkerT2 = Settings.MarkT2.nValue;
	}
	if ( Settings.MarkY1.Mode == CSettings::Marker::_On )
	{
		nMarkerY1 = Settings.MarkY1.nValue;
		if ( Settings.MarkY1.Source == CSettings::Marker::_CH1 )
		{
			nMarkerY1 = Settings.CH1Calib.Correct( Ch1fast, nMarkerY1 );
		} else
		{
			nMarkerY1 = Settings.CH2Calib.Correct( Ch2fast, nMarkerY1 );
		}
		nMarkerY1 = (nMarkerY1*(DivsY*BlkY))>>8;
	}
	if ( Settings.MarkY2.Mode == CSettings::Marker::_On )
	{
		nMarkerY2 = Settings.MarkY2.nValue;
		if ( Settings.MarkY2.Source == CSettings::Marker::_CH1 )
		{
			nMarkerY2 = Settings.CH1Calib.Correct( Ch1fast, nMarkerY2 );
		} else
		{
			nMarkerY2 = Settings.CH2Calib.Correct( Ch2fast, nMarkerY2 );
		}
		nMarkerY2 = (nMarkerY2*(DivsY*BlkY))>>8;
	}
	if ( Settings.MarkY1.Mode == CSettings::Marker::_Auto )
	{
		nMarkerY1 = BIOS::ADC::GetAt( Settings.MarkT1.nValue );
		if ( Settings.MarkY1.Source == CSettings::Marker::_CH1 )
		{
			nMarkerY1 = (ui8)((nMarkerY1) & 0xff);
			Settings.MarkY1.nValue = nMarkerY1;
			nMarkerY1 = Settings.CH1Calib.Correct( Ch1fast, nMarkerY1 );
		} else
		{
			nMarkerY1 = (ui8)((nMarkerY1>>8) & 0xff);
			Settings.MarkY1.nValue = nMarkerY1;
			nMarkerY1 = Settings.CH2Calib.Correct( Ch2fast, nMarkerY1 );
		}
		nMarkerY1 = (nMarkerY1*(DivsY*BlkY))>>8;
	}
	if ( Settings.MarkY2.Mode == CSettings::Marker::_Auto )
	{
		nMarkerY2 = BIOS::ADC::GetAt( Settings.MarkT2.nValue );
		if ( Settings.MarkY2.Source == CSettings::Marker::_CH1 )
		{
			nMarkerY2 = (ui8)((nMarkerY2) & 0xff);
			Settings.MarkY2.nValue = nMarkerY2;
			nMarkerY2 = Settings.CH1Calib.Correct( Ch1fast, nMarkerY2 );
		} else
		{
			nMarkerY2 = (ui8)((nMarkerY2>>8) & 0xff);
			Settings.MarkY2.nValue = nMarkerY2;
			nMarkerY2 = Settings.CH2Calib.Correct( Ch2fast, nMarkerY2 );
		}
		nMarkerY2 = (nMarkerY2*(DivsY*BlkY))>>8;
	}
	if ( nMarkerY1 >= DivsY*BlkY )
		nMarkerY1 = -1;
	if ( nMarkerY2 >= DivsY*BlkY )
		nMarkerY2 = -1;
}

void CWndOscGraph::GetCurrentRange(int& nBegin, int& nEnd)
{
	nBegin = Settings.Time.Shift;
	nEnd = nBegin + m_rcClient.Width();
	nEnd = min( nEnd, (int)BIOS::ADC::GetCount() );
}

// Move to better place
void CWndOscGraph::UpdateResolutions()
{
	Settings.Runtime.m_fTimeRes = CSettings::TimeBase::pfValueResolution[ Settings.Time.Resolution ];
	Settings.Runtime.m_fCH1Res  = 
		CSettings::AnalogChannel::pfValueResolution[ Settings.CH1.Resolution ] *
		CSettings::AnalogChannel::pfValueProbe[ Settings.CH1.Probe ];
	Settings.Runtime.m_fCH2Res  = 
		CSettings::AnalogChannel::pfValueResolution[ Settings.CH2.Resolution ] *
		CSettings::AnalogChannel::pfValueProbe[ Settings.CH2.Probe ];
}