#include "MainWnd.h"
//#include <Source/Framework/Eval.h>

#include "MainWndSdk.h"

//#include <stdarg.h>

/*static*/ CMainWnd	*CMainWnd::m_pInstance = NULL;

void CMainWnd::Create()
{
	m_pInstance = this;

	// image correctly uploaded in ROM ?
	_ASSERT( 
		CSettings::TimeBase::ppszTextResolution[CSettings::TimeBase::_1s][0] == '1' &&
		CSettings::TimeBase::ppszTextResolution[CSettings::TimeBase::_1s][1] == 's' &&
		CSettings::TimeBase::ppszTextResolution[CSettings::TimeBase::_1s][2] == 0 );

	//Settings.Save();
	//Settings.SaveCalibration();

	Settings.Load();
	Settings.LoadCalibration();
	
	CWnd::Create("CMainWnd", WsVisible, CRect(0, 0, BIOS::LCD::LcdWidth, BIOS::LCD::LcdHeight), NULL );

	m_wndToolBar.Create( this );
	m_wndGraph.Create( this, WsHidden | WsNoActivate );
	m_wndSignalGraph.Create( this, WsNoActivate );
	m_wndSpectrumGraph.Create( this, WsNoActivate );

	m_wndMenuInput.Create( this, WsHidden );
	m_wndMenuCursor.Create( this, WsHidden );
	m_wndMenuMeas.Create( this, WsHidden );
	m_wndMenuMath.Create( this, WsHidden );
	m_wndMenuSettings.Create( this, WsHidden );
	m_wndMenuDisplay.Create( this, WsHidden );
	m_wndMenuGenerator.Create( this, WsHidden );
	m_wndMenuTools.Create( this, WsHidden );
	m_wndZoomBar.Create( this, WsHidden, &m_wndGraph );
	m_wndLReferences.Create( this, WsHidden );
	m_wndTReferences.Create( this, WsHidden );
	m_wndSpectrumMain.Create( this, WsHidden );
	m_wndScreenSaver.Create( this, WsHidden );
	m_wndUserGame.Create( this, WsHidden );
	m_wndUserBalls.Create( this, WsHidden );
	m_wndUserMeter.Create( this, WsHidden );
	m_wndModuleSel.Create(this, WsHidden );
	m_wndUserCalibration.Create( this, WsHidden );

	if ( Settings.Runtime.m_nMenuItem != -1 )
		SendMessage( &m_wndToolBar, ToWord('g', 'i'), Settings.Runtime.m_nMenuItem);
	else
		SendMessage( &m_wndToolBar, ToWord('g', 'i'), 1);
	
	//m_wndToolBar.SetFocus();
	//SendMessage( &m_wndToolBar, ToWord('g', 'o'), (ui32)"Meter");
	//m_wndMenuInput.m_itmCH1.SetFocus();
	//m_wndMenuGenerator.m_itmBpm.SetFocus();
	//OnMessage( &m_wndToolBar, ToWord('f', 'c'), 0 ); // force update

	SetTimer(1000);
}

/*virtual*/ void CMainWnd::OnTimer()
{
	static int nSeconds = 0;
	static ui32 nChecksum = -1;

	if ( nChecksum == (ui32)-1 )
	{
		nChecksum = Settings.GetChecksum();
	}
	if ( ++nSeconds >= 20 )
	{
		nSeconds = 0;
		ui32 nNewChecksum = Settings.GetChecksum();
		if ( nNewChecksum != nChecksum )
		{
			//m_wndMessage.Show(this, "Information", "Saving settings...", RGB565(ffff00));
			Settings.Save();
			nChecksum = nNewChecksum;
		}
	}

	SdkProc();
}

/*virtual*/ void CMainWnd::OnPaint()
{
	BIOS::LCD::Clear(RGB565(000000));
}

/*virtual*/ void CMainWnd::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	if ( pSender == &m_wndToolBar )
	{
		if ( code == ToWord('L', 'D') && data )	// Layout disable
		{
			CWnd* pLayout = (CWnd*)data;
			SendMessage( pLayout, code, 0 );
			pLayout->ShowWindow( SwHide );
		}
		if ( code == ToWord('L', 'E') && data )	// Layout enable
		{
			CWnd* pLayout = (CWnd*)data;
			SendMessage( pLayout, code, 0 );
			pLayout->ShowWindow( SwShow );
		}
		if ( code == ToWord('L', 'R') )	// Layout reset
		{
			Invalidate();
		}
	}
/*
	if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
	{
		if ( m_wndGraph.m_dwFlags & WsVisible )
			m_wndGraph.Invalidate();
		else if ( m_wndSpectrumGraph.m_dwFlags & WsVisible )
			m_wndSpectrumGraph.Invalidate();
		else if ( GetFocus() != this )
			SendMessage( GetFocus(), code, data );
	}
	*/
}

