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
	CCoreOscilloscope::ConfigureAdc();
	CCoreSettings::Update();

	m_nLastKey = BIOS::SYS::GetTick();
	m_bSleeping = FALSE;
	
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
	m_wndMenuKeySettings.Create( this, WsHidden );
	m_wndMenuDisplay.Create( this, WsHidden );
	m_wndMenuGenerator.Create( this, WsHidden );
	m_wndMenuGeneratorMod.Create( this, WsHidden );
	m_wndMenuGeneratorEdit.Create( this, WsHidden );
	m_wndZoomBar.Create( this, WsHidden, &m_wndGraph );
	m_wndInfoBar.Create( this, WsHidden, &m_wndGraph );
	m_wndLReferences.Create( this, WsHidden );
	m_wndTReferences.Create( this, WsHidden );
	m_wndSpectrumMain.Create( this, WsHidden );
	m_wndSpectrumMarker.Create( this, WsHidden );
	m_wndSpectrumAnnot.Create( this, WsHidden );
	m_wndScreenSaver.Create( this, WsHidden );
	//m_wndUserGame.Create( this, WsHidden );
	//m_wndUserBalls.Create( this, WsHidden );
	//m_wndUserTuner.Create( this, WsHidden );
	//m_wndUserMeter.Create( this, WsHidden );
	//m_wndUserFullView.Create( this, WsHidden );
	m_wndAboutFirmware.Create( this, WsHidden );
	m_wndAboutDevice.Create( this, WsHidden );
	m_wndModuleSel.Create(this, WsHidden );
	m_wndCalibration.Create( this, WsHidden );

#	define ADD_MODULE( strName, type ) m_wndUser##type.Create( this, WsHidden );
#	include "User/_Modules.h"
#	undef ADD_MODULE

	m_wndToolbox.Create(this);

	//if ( Settings.Runtime.m_nMenuItem != -1 )
	//	SendMessage( &m_wndToolBar, ToWord('g', 'i'), Settings.Runtime.m_nMenuItem);
	//else
		SendMessage( &m_wndToolBar, ToWord('g', 'i'), 1);

	m_lLastAcquired = 0;
	SetTimer(200);
}

/*virtual*/ void CMainWnd::OnTimer()
{
	// every 200ms
	static int nSubCounter = 0;

	if ( ++nSubCounter == 5 )
	{
		nSubCounter = 0;
		static ui32 nChecksum = -1;
		static int nSeconds = 0;
		nSeconds++;

		if ( Settings.Runtime.m_nStandby != 0 )
		{
			if ( (int)BIOS::SYS::GetTick() > m_nLastKey + Settings.Runtime.m_nStandby*60000 )
			{
				// enter sleep mode
				BIOS::SYS::SetVolume(0);
				m_wndMessage.Show(this, "Information", "Entering sleep mode", RGB565(ffff00));
				BIOS::ADC::Enable( FALSE );
				for ( int i = Settings.Runtime.m_nBacklight; i > 0; i--)
				{
					BIOS::SYS::SetBacklight( i );
					BIOS::SYS::DelayMs(10);
				}
				BIOS::SYS::Standby( TRUE );
				m_bSleeping = TRUE;
				KillTimer();
			}
		}

 		if ( nChecksum == (ui32)-1 )
		{
			int nUptime = Settings.Runtime.m_nUptime;
			Settings.Runtime.m_nUptime = 0;
			nChecksum = Settings.GetChecksum();
			Settings.Runtime.m_nUptime = nUptime;
		}
		if ( Settings.m_lLastChange != 0 )
		{
			// save settings in 20 seconds
			nSeconds = 120 - 20;
			Settings.m_lLastChange = 0;
		}
		if ( nSeconds >= 120 )
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

		char test[32];
		static int nCounter=0;
		BIOS::DBG::sprintf(test, "Ready.(%d)\n", nCounter++);
		BIOS::SERIAL::Send("TEST!");
		BIOS::SERIAL::Send(test);
	}

	if ( BIOS::ADC::Enabled() && Settings.Trig.Sync == CSettings::Trigger::_Auto )
	{
		long lTick = BIOS::SYS::GetTick();
		if ( m_lLastAcquired != -1 && lTick - m_lLastAcquired > 150 )
		{
			// whatever is in buffer, just process it. 250ms should be 
			// sufficient to grab samples for one screen,
			// in first pass it will copy nonintialzed buffer!
			//int nBegin, nEnd;
			//BIOS::ADC::GetBufferRange( nBegin, nEnd );
			int nEnd = BIOS::ADC::GetCount();
			BIOS::ADC::Copy( /*BIOS::ADC::GetCount()*/ nEnd );
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

//long lForceRestart = -1;
/*virtual*/ void CMainWnd::WindowMessage(int nMsg, int nParam /*=0*/)
{
//	BIOS::LCD::Printf( 0, 0, RGB565(ff0000), RGB565(ffffff), "%d", BIOS::ADC::GetState() );
	if ( nMsg == WmTick )
	{
		if ( m_bSleeping )
			return;
		// timers update
		CWnd::WindowMessage( nMsg, nParam );

		SdkUartProc();
/*
		if ( lForceRestart > 0 && lForceRestart <= (long)BIOS::SYS::GetTick() )
		{
			BIOS::ADC::Restart();
			lForceRestart = -1;
		}*/
		if ( (Settings.Trig.Sync != CSettings::Trigger::_None) && BIOS::ADC::Enabled() && BIOS::ADC::Ready() /*&& lForceRestart < 0*/ )
		{
			int nEnd = BIOS::ADC::GetCount();
//			BIOS::ADC::GetBufferRange( nBegin, nEnd );
			BIOS::ADC::Copy( /*BIOS::ADC::GetCount()*/ nEnd );
			BIOS::ADC::Restart();
//			lForceRestart = BIOS::SYS::GetTick() + 400;

			// trig stuff
			m_lLastAcquired = BIOS::SYS::GetTick();
			if ( BIOS::ADC::Enabled() && Settings.Trig.Sync == CSettings::Trigger::_Single )
			{
				BIOS::ADC::Enable( false );
				Settings.Trig.State = CSettings::Trigger::_Stop;
				if ( m_wndMenuInput.m_itmTrig.IsVisible() )
					m_wndMenuInput.m_itmTrig.Invalidate();
			}

			// broadcast message for windows that process waveform data
			WindowMessage( CWnd::WmBroadcast, ToWord('d', 'g') );
		}
		return;
	}

	if ( nMsg == WmKey )
	{
		if ( m_bSleeping )
		{
			SetTimer( 200 );
			m_bSleeping = false;
			BIOS::SYS::Standby( FALSE );

			CCoreOscilloscope::ConfigureAdc();
			CCoreGenerator::Update();
			m_wndMessage.Hide();
			Invalidate();

			for ( int i = 0; i < Settings.Runtime.m_nBacklight; i++)
			{
				BIOS::SYS::SetBacklight( i );
				BIOS::SYS::DelayMs(10);
			}

			m_nLastKey = BIOS::SYS::GetTick();
			CCoreSettings::Update();	// display backlight
			return;
		}

		m_nLastKey = BIOS::SYS::GetTick();

		if ( nParam == BIOS::KEY::KeyFunction )
			CMainWnd::CallShortcut(Settings.Runtime.m_nShortcutCircle);

		if ( nParam == BIOS::KEY::KeyFunction2 )
			CMainWnd::CallShortcut(Settings.Runtime.m_nShortcutTriangle);

		if ( nParam == BIOS::KEY::KeyS2 )
			CMainWnd::CallShortcut(Settings.Runtime.m_nShortcutS2);

		if ( nParam == BIOS::KEY::KeyS1 )
			CMainWnd::CallShortcut(Settings.Runtime.m_nShortcutS1);
	}

	CWnd::WindowMessage( nMsg, nParam );
}

void CMainWnd::CallShortcut(int nShortcut)
{
	static int nOldFocus = -1;
	if ( nShortcut >= 0 )
	{
		// +1 => first submenu in section
		if ( nShortcut+1 == MainWnd.m_wndToolBar.m_nFocus )
		{
			if ( nOldFocus != -1 )
				SendMessage( &MainWnd.m_wndToolBar, ToWord('g', '2'), nOldFocus );
		} else {
			nOldFocus = MainWnd.m_wndToolBar.m_nFocus;
			SendMessage( &MainWnd.m_wndToolBar, ToWord('g', '2'), nShortcut );
		}
		return;
	}
	switch ( nShortcut )
	{
		case CSettings::CRuntime::None:
			break;
		case CSettings::CRuntime::StartStop:
			m_wndToolbox.ToggleAdc();
			break;
		case CSettings::CRuntime::Toolbox:
			if ( !m_wndToolbox.IsVisible() && !m_wndManager.IsVisible() )
			{
				m_wndToolbox.DoModal();
				if ( m_wndToolbox.GetResult() == CWndToolbox::MenuManager)
				{
					m_wndManager.Create(this);
					m_wndManager.DoModal();
				}
			}
			break;
		case CSettings::CRuntime::WaveManager:
			if ( m_wndManager.IsVisible() )
				m_wndManager.Cancel();
			else
			{
				m_wndManager.Create(this);
				m_wndManager.DoModal();
			}
			break;
		case CSettings::CRuntime::Screenshot:
			m_wndToolbox.SaveScreenshot();
			break;
	default:
		_ASSERT( !!!"Unknown shortcut" );
	}
}
