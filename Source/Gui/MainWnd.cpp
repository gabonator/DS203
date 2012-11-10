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

	Settings.Load();
	Settings.LoadCalibration();
	m_wndGraph.UpdateResolutions();

	
	CWnd::Create("CMainWnd", WsVisible | WsListener, CRect(0, 0, BIOS::LCD::LcdWidth, BIOS::LCD::LcdHeight), NULL );

	m_wndToolBar.Create( this );
	//m_wndManager.Create( this );
	m_wndGraph.Create( this, WsHidden | WsNoActivate );
	m_wndSignalGraph.Create( this, WsNoActivate );
	m_wndSpectrumGraph.Create( this, WsNoActivate );
	m_wndSpectrumMiniFD.Create( this, WsNoActivate );
	m_wndSpectrumMiniTD.Create( this, WsNoActivate );
	m_wndSpectrumMiniSG.Create( this, WsNoActivate );

	m_wndMenuInput.Create( this, WsHidden );
	m_wndMenuCursor.Create( this, WsHidden );
	m_wndMenuMeas.Create( this, WsHidden );
	m_wndMenuMath.Create( this, WsHidden );
	m_wndMenuSettings.Create( this, WsHidden );
	m_wndMenuDisplay.Create( this, WsHidden );
	m_wndMenuGenerator.Create( this, WsHidden );
	m_wndMenuTools.Create( this, WsHidden );
	m_wndZoomBar.Create( this, WsHidden, &m_wndGraph );
	m_wndInfoBar.Create( this, WsHidden, &m_wndGraph );
	m_wndLReferences.Create( this, WsHidden );
	m_wndTReferences.Create( this, WsHidden );
	m_wndSpectrumMain.Create( this, WsHidden );
	m_wndSpectrumMarker.Create( this, WsHidden );
	m_wndSpectrumAnnot.Create( this, WsHidden );
	m_wndScreenSaver.Create( this, WsHidden );
	m_wndUserGame.Create( this, WsHidden );
	m_wndUserBalls.Create( this, WsHidden );
	m_wndUserMeter.Create( this, WsHidden );
	m_wndAbout.Create( this, WsHidden );
	m_wndModuleSel.Create(this, WsHidden );
	m_wndUserCalibration.Create( this, WsHidden );

	m_wndToolbox.Create(this);

	if ( Settings.Runtime.m_nMenuItem != -1 )
		SendMessage( &m_wndToolBar, ToWord('g', 'i'), Settings.Runtime.m_nMenuItem);
	else
		SendMessage( &m_wndToolBar, ToWord('g', 'i'), 1);

	m_lLastAcquired = 0;
	SetTimer(200);
}

/*virtual*/ void CMainWnd::OnTimer()
{
	static int nSeconds = 0;
	static ui32 nChecksum = -1;

	if ( nChecksum == (ui32)-1 )
	{
		int nUptime = Settings.Runtime.m_nUptime;
		Settings.Runtime.m_nUptime = 0;
		nChecksum = Settings.GetChecksum();
		Settings.Runtime.m_nUptime = nUptime;
	}

	if ( ++nSeconds >= 120*5 )
	{
		nSeconds = 0;
	
		ui32 nNewChecksum = Settings.GetStaticChecksum();
		if ( nNewChecksum != nChecksum )
		{
			//m_wndMessage.Show(this, "Information", "Saving settings...", RGB565(ffff00));
			Settings.Save();
			nChecksum = nNewChecksum;
		}
	}
	if ( (nSeconds & 7) == 0 )
	{
		SdkProc();
		BIOS::SERIAL::Send("Ready.\n");

		int ch = BIOS::SERIAL::Getch();
		if (ch >= 0)
			BIOS::SERIAL::Send("Mam znak!!!\n");
	}
	
	if ( BIOS::ADC::Enabled() && Settings.Trig.Sync == CSettings::Trigger::_Auto )
	{
		long lTick = BIOS::GetTick();
		if ( m_lLastAcquired != -1 && lTick - m_lLastAcquired > 150 )
		{
			// whatever is in buffer, just process it. 250ms should be 
			// sufficient to grab samples for one screen,
			// in first pass it will copy nonintialzed buffer!
			BIOS::ADC::Copy( BIOS::ADC::GetCount() );
			WindowMessage( CWnd::WmBroadcast, ToWord('d', 'g') );
			// force update
			BIOS::ADC::Restart();
		}
	}
}

/*virtual*/ void CMainWnd::OnPaint()
{
	BIOS::LCD::Clear(RGB565(000000));
}

/*virtual*/ void CMainWnd::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
	{
		m_lLastAcquired = BIOS::GetTick();
		if ( BIOS::ADC::Enabled() && Settings.Trig.Sync == CSettings::Trigger::_Single )
		{
			BIOS::ADC::Enable( false );
			Settings.Trig.State = CSettings::Trigger::_Stop;
			if ( m_wndMenuInput.m_itmTrig.IsVisible() )
				m_wndMenuInput.m_itmTrig.Invalidate();
		}
		return;
	}
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
		return;
	}
}

/*virtual*/ void CMainWnd::WindowMessage(int nMsg, int nParam /*=0*/)
{
	if ( nMsg == WmKey && nParam == BIOS::KEY::KeyFunction )
	{
		// show toolbox	
		if ( !m_wndToolbox.IsVisible() && !m_wndManager.IsVisible() )
		{
			m_wndToolbox.DoModal();
			if ( m_wndToolbox.GetResult() == CWndToolbox::MenuManager)
			{
				m_wndManager.Create(this);
				m_wndManager.DoModal();
			}
		}
	}

	if ( nMsg == WmKey && nParam == BIOS::KEY::KeyFunction2 )
	{
		m_wndToolbox.SaveScreenshot();
	}

	CWnd::WindowMessage( nMsg, nParam );
}