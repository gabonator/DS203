#include "MenuSpectMain.h"

#include <Source/Gui/MainWnd.h>

CWndMenuSpectMain::CWndMenuSpectMain()
{
}

/*virtual*/ void CWndMenuSpectMain::Create(CWnd *pParent, ui16 dwFlags) 
{
	CWnd::Create("CWndMenuSpectMain", dwFlags, CRect(316-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
	m_itmCH1.Create(&Settings.CH1, this);
	m_itmCH2.Create(&Settings.CH2, this);
	m_itmTime.Create(&Settings.Time, this);
	m_itmDisplay.Create(this);
	m_itmWindow.Create(this);
}

/*virtual*/ void CWndMenuSpectMain::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
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

	if ( code == ToWord('i', 'u') && 
		( pSender == &m_itmCH1 || pSender == &m_itmCH2 || pSender == &m_itmTime ) )
	{
		MainWnd.m_wndMenuInput.ConfigureAdc();
		MainWnd.m_wndGraph.UpdateResolutions();
		MainWnd.m_wndSpectrumAnnot.Invalidate();
		return;
	}
	if ( code == ToWord('m', 'a') && pSender == &m_itmCH1 )
	{
		if ( Settings.CH1.Enabled == CSettings::AnalogChannel::_NO )
			Settings.CH1.Enabled = CSettings::AnalogChannel::_YES;
		else
			Settings.CH1.Enabled = CSettings::AnalogChannel::_NO;
		m_itmCH1.Invalidate();
		return;
	}
	if ( code == ToWord('m', 'a') && pSender == &m_itmCH2 )
	{
		if ( Settings.CH2.Enabled == CSettings::AnalogChannel::_NO )
			Settings.CH2.Enabled = CSettings::AnalogChannel::_YES;
		else
			Settings.CH2.Enabled = CSettings::AnalogChannel::_NO;
		m_itmCH2.Invalidate();
		return;
	}
	if ( code == ToWord('u', 'p') && pSender == &m_itmDisplay )
	{
		MainWnd.m_wndSpectrumMiniTD.ShowWindow( 
			( Settings.Spec.Display == CSettings::Spectrum::_FftTime || 
			Settings.Spec.Display == CSettings::Spectrum::_Spectrograph ) ? SwShow : SwHide );
		MainWnd.m_wndSpectrumMiniFD.ShowWindow( Settings.Spec.Display == CSettings::Spectrum::_FftTime ? SwShow : SwHide );
		MainWnd.m_wndSpectrumGraph.ShowWindow( Settings.Spec.Display == CSettings::Spectrum::_Fft ? SwShow : SwHide );
		MainWnd.m_wndSpectrumMiniSG.ShowWindow( Settings.Spec.Display == CSettings::Spectrum::_Spectrograph ? SwShow : SwHide );
		MainWnd.m_wndSpectrumAnnot.ShowWindow( SwShow );
		MainWnd.Invalidate();
	}
}


