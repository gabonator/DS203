#include "MenuSpectMarker.h"

#include <Source/Gui/MainWnd.h>

CWndMenuSpectMarker::CWndMenuSpectMarker()
{
}

/*virtual*/ void CWndMenuSpectMarker::Create(CWnd *pParent, ui16 dwFlags) 
{
	CWnd::Create("CWndMenuSpectMain", dwFlags | CWnd::WsListener, CRect(316-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
	m_itmSource.Create("Source\nCH1", RGB565(ffff00), 2, this);
	m_itmTrack.Create("Position\nAuto", RGB565(8080b0), 2, this);
	m_itmValue.Create(this);
}

/*virtual*/ void CWndMenuSpectMarker::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
	{
		m_itmValue.Invalidate();
	}

	// LAYOUT ENABLE/DISABLE FROM TOP MENU BAR
	if (code == ToWord('L', 'D') )
	{
		MainWnd.m_wndSpectrumMiniTD.ShowWindow( SwHide );
		MainWnd.m_wndSpectrumMiniFD.ShowWindow( SwHide );
		MainWnd.m_wndSpectrumMiniSG.ShowWindow( SwHide );
		MainWnd.m_wndSpectrumGraph.ShowWindow( SwHide );
		MainWnd.m_wndSpectrumAnnot.ShowWindow( SwHide );
		return;
	}

	if (code == ToWord('L', 'E') )
	{
		MainWnd.m_wndSpectrumMiniTD.ShowWindow( 
			( Settings.Spec.Display == CSettings::Spectrum::_FftTime || 
			Settings.Spec.Display == CSettings::Spectrum::_Spectrograph ) ? SwShow : SwHide );
		MainWnd.m_wndSpectrumMiniFD.ShowWindow( Settings.Spec.Display == CSettings::Spectrum::_FftTime ? SwShow : SwHide );
		MainWnd.m_wndSpectrumGraph.ShowWindow( Settings.Spec.Display == CSettings::Spectrum::_Fft ? SwShow : SwHide );
		MainWnd.m_wndSpectrumMiniSG.ShowWindow( Settings.Spec.Display == CSettings::Spectrum::_Spectrograph ? SwShow : SwHide );
		MainWnd.m_wndSpectrumAnnot.ShowWindow( SwShow );
		return;
	}
}


