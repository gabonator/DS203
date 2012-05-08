#include "MenuInput.h"

#include <Source/Gui/MainWnd.h>

/*static*/ void CWndMenuInput::ConfigureAdc()
{
	const static ui16 arrPsc[] = 
	{ 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 10-1, 10-1, 10-1, 100-1, 100-1, 
	100-1, 1000-1, 1000-1, 1000-1, 1000-1, 1000-1 };
	const static ui16 arrArr[] = 
	{ 1-1, 1-1, 2-1, 3-1, 5-1, 12-1, 24-1, 48-1, 120-1, 240-1, 480-1, 120-1, 240-1, 480-1,
	120-1, 240-1, 480-1, 120-1, 240-1, 480-1, 1200-1, 1000-1};

	_ASSERT( COUNT(arrPsc) == COUNT(arrArr) && COUNT(arrPsc) == CSettings::TimeBase::_ResolutionMax+1 );

	ui8 nACouple = Settings.CH1.Coupling == CSettings::AnalogChannel::_DC ? 0 : 1;
	ui8 nARange = (ui8)(NATIVEENUM)Settings.CH1.Resolution;
	ui16 nAOffset = Settings.CH1.u16Position;
	ui8 nBCouple = Settings.CH2.Coupling == CSettings::AnalogChannel::_DC ? 0 : 1;
	ui8 nBRange = (ui8)(NATIVEENUM)Settings.CH2.Resolution;
	ui16 nBPosition = Settings.CH2.u16Position;
	ui16 nTimePsc = arrPsc[ (NATIVEENUM)Settings.Time.Resolution ];
	ui16 nTimeArr = arrArr[ (NATIVEENUM)Settings.Time.Resolution ];
	
	BIOS::ADC::Enable(true);
	BIOS::ADC::Configure( nACouple, nARange, nAOffset, nBCouple, nBRange, nBPosition, nTimePsc, nTimeArr );
	BIOS::ADC::Restart();
}

/*static*/ void CWndMenuInput::ConfigureTrigger()
{
	if ( Settings.Trig.Sync == CSettings::Trigger::_None )
	{
		BIOS::ADC::ConfigureTrigger(0, 0, 0, -1 );
	} else {
		BIOS::ADC::ConfigureTrigger(Settings.Trig.nTime, Settings.Trig.nLevel, 
			(ui8)(NATIVEENUM)Settings.Trig.Source, 
			(ui8)(NATIVEENUM)Settings.Trig.Type );
	}
}

CWndMenuInput::CWndMenuInput()
{
}

/*virtual*/ void CWndMenuInput::Create(CWnd *pParent, ui16 dwFlags) 
{
	CWnd::Create("CWndMenuInput", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
	m_itmCH1.Create(&Settings.CH1, this);
	m_itmCH2.Create(&Settings.CH2, this);
	m_itmCH3.Create(&Settings.CH3, this);
	m_itmCH4.Create(&Settings.CH4, this);
	m_itmTime.Create(&Settings.Time, this);
	m_itmWindow.Create(this);
	m_itmTrig.Create(this);
}

/*virtual*/ void CWndMenuInput::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	// LAYOUT ENABLE/DISABLE FROM TOP MENU BAR
	if (code == ToWord('L', 'D') )
	{
		MainWnd.m_wndGraph.ShowWindow( SwHide );
		MainWnd.m_wndZoomBar.ShowWindow( SwHide );
		MainWnd.m_wndLReferences.ShowWindow( SwHide );
		MainWnd.m_wndTReferences.ShowWindow( SwHide );
	}

	if (code == ToWord('L', 'E') )
	{
		MainWnd.m_wndGraph.ShowWindow( SwShow );
		MainWnd.m_wndZoomBar.ShowWindow( SwShow );
		MainWnd.m_wndLReferences.ShowWindow( SwShow );
		MainWnd.m_wndTReferences.ShowWindow( SwShow );
	}


	// Trigger
	if (code == ToWord('m', 'r') )
	{
		m_wndListTrigger.Create( this );
		m_wndListTrigger.StartModal(&m_wndListTrigger.m_itmLevel);
	}
	if ( code == ToWord('o', 'k') && pSender->m_pParent == &m_wndListTrigger )
	{
		// done
		m_wndListTrigger.StopModal();
	}
	if ( code == ToWord('e', 'x') && pSender == &m_wndListTrigger )
	{
		// cancel
		m_wndListTrigger.StopModal();
	}
	if ( code == ToWord('u', 'p') && pSender->m_pParent == &m_wndListTrigger )
	{
		if ( pSender == &m_wndListTrigger.m_itmLevel )
			MainWnd.m_wndLReferences.Invalidate();
		if ( pSender == &m_wndListTrigger.m_itmTime )
			MainWnd.m_wndTReferences.Invalidate();

		ConfigureTrigger();
		Settings.Trig.nLastChange = BIOS::GetTick();
		// update
		CWnd::m_arrModals.GetLast().m_pPrevFocus->Invalidate();
	}

	// window
	if ( code == ToWord('w', 'u') )
	{
		MainWnd.m_wndTReferences.Invalidate();
		MainWnd.m_wndZoomBar.Invalidate();
	}

	// Timebase
	if (code == ToWord('m', 't') )	// more timebase
	{
		m_wndComboSelector.Create( GetFocus()->m_pszId, WsVisible | WsModal, 
			CRect( 30, 100, 370, 140), RGB565(ffffff), (CValueProvider*)(NATIVEPTR)data, this);

		m_wndComboSelector.StartModal();
	}

	// Digital
	if (code == ToWord('m', 'd') )	// more input digital
	{
		m_wndListDInput.Create( (CSettings::DigitalChannel*)(NATIVEPTR)data, this );
		m_wndListDInput.StartModal( &m_wndListDInput.m_itmPosition );
	}
	if ( code == ToWord('o', 'k') && pSender->m_pParent == &m_wndListDInput )
	{
		// done
		m_wndListDInput.StopModal();
	}
	if ( code == ToWord('e', 'x') && pSender == &m_wndListDInput )
	{
		// cancel
		m_wndListDInput.StopModal();
	}

	// Analog
	if (code == ToWord('m', 'a') )	// more input analog
	{
		m_wndListAInput.Create( (CSettings::AnalogChannel*)(NATIVEPTR)data, this );
		m_wndListAInput.StartModal( &m_wndListAInput.m_itmResolution );
	}
	if ( code == ToWord('o', 'k') && pSender->m_pParent == &m_wndListAInput )
	{
		// done
		m_wndListAInput.StopModal();
	}
	if ( code == ToWord('e', 'x') && pSender == &m_wndListAInput )
	{
		m_wndListAInput.StopModal();
	}
	if ( code == ToWord('u', 'p') && pSender->m_pParent == &m_wndListAInput )
	{
		if ( pSender == &m_wndListAInput.m_itmRed ||
			pSender == &m_wndListAInput.m_itmGrn ||
			pSender == &m_wndListAInput.m_itmBlu )
		{
			m_wndListAInput.m_itmColour.Invalidate();
		}
		if ( pSender == &m_wndListAInput.m_itmPosition )
		{
			MainWnd.m_wndLReferences.Invalidate();
		}
		// update
		CWnd::m_arrModals.GetLast().m_pPrevFocus->Invalidate();
		ConfigureAdc();
	}
	if ( code == ToWord('u', 'p') && pSender->m_pParent == &m_wndListDInput )
	{
		if ( pSender == &m_wndListDInput.m_itmRed ||
			pSender == &m_wndListDInput.m_itmGrn ||
			pSender == &m_wndListDInput.m_itmBlu )
		{
			m_wndListDInput.m_itmColour.Invalidate();
		}
		if ( pSender == &m_wndListDInput.m_itmPosition )
		{
			MainWnd.m_wndLReferences.Invalidate();
		}
		// update
		CWnd::m_arrModals.GetLast().m_pPrevFocus->Invalidate();
	}
	// Selector
	if ( code == ToWord('l', 'e') )	// provider selector combo box
	{
		CValueProvider* pProvider = (CValueProvider*)(NATIVEPTR)data;
		if ( pProvider->Get() == 0 )
		{
			// TODO: remove? used only for button element testing
			// button ?
			MainWnd.m_wndMessage.Show( this, "Message", ((CProviderBtn*)pProvider)->m_pName, RGB565(ffff00));
			return;
		} else
		{
			m_wndComboSelector.Create( GetFocus()->m_pszId, WsVisible | WsModal, 
				CRect( 30, 100, 370, 140), RGB565(ffffff), pProvider, this);
		}
		m_wndComboSelector.StartModal();
	}
	if ( code == ToWord('o', 'k') && pSender == &m_wndComboSelector )
	{
		// done
		m_wndComboSelector.StopModal();
	}
	if ( code == ToWord('e', 'x') && pSender == &m_wndComboSelector )
	{
		// cancel
		m_wndComboSelector.StopModal();
	}
	if ( code == ToWord('i', 'u') )
	{
		ConfigureAdc();
	}
	if ( code == ToWord('r', 'u') )
	{
		Settings.Trig.nLastChange = BIOS::GetTick();
		MainWnd.m_wndLReferences.Invalidate();
		ConfigureTrigger();
	}
}


