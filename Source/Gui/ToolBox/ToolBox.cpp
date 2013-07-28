#include "ToolBox.h"
#include <Source/Gui/MainWnd.h>

CWndToolbox::CWndToolbox()
{
	m_bRunning = false;
	m_nFocus = -1;
	m_bFirst = true;
	m_bAdcEnabled = false;
}

void CWndToolbox::Create( CWnd* pParent )
{
	CWnd::Create("CWndToolbox", CWnd::WsHidden | CWnd::WsModal, 
		CRect( (BIOS::LCD::LcdWidth-Width)/2 ,
		(BIOS::LCD::LcdHeight-Height)/2,
		(BIOS::LCD::LcdWidth+Width)/2,
		(BIOS::LCD::LcdHeight+Height)/2 ), pParent);
}

/*virtual*/ void CWndToolbox::OnPaint()
{
	CRect rcClient(m_rcClient);
	if ( m_bFirst )
	{
		CDesign::Shadow(rcClient, 0xc0ffffff); // aa rr gg bb
		//rcClient.Deflate( 2, 2, 2, 2 );
		//CDesign::Shadow(rcClient, 0x80ffffff); // aa rr gg bb
		m_bFirst = false;
	}

	if ( m_nFocus == -1 )
		m_nFocus = 0;

	#define FOC(n) (m_nFocus==n)?RGB565(ffffff):RGB565(808080)
	PrintBold( m_rcClient.left + 8, m_rcClient.top + 2 + 0*16, FOC(0), RGB565(000000), "\x10 Waveform manager");
	PrintBold( m_rcClient.left + 8, m_rcClient.top + 2 + 1*16, FOC(1), RGB565(000000), 
		m_bAdcEnabled ? "\x10 Pause" : "\x10 Resume" );
	PrintBold( m_rcClient.left + 8, m_rcClient.top + 2 + 2*16, FOC(2), RGB565(000000), "\x10 Reset setings");
	PrintBold( m_rcClient.left + 8, m_rcClient.top + 2 + 3*16, FOC(3), RGB565(000000), "\x10 Save setings");

	char str[32];
	BIOS::DBG::sprintf(str, "bat %d%%", BIOS::SYS::GetBattery());
	BIOS::LCD::Line( m_rcClient.left + 4, m_rcClient.bottom - 20, 
		m_rcClient.right - 4, m_rcClient.bottom - 20, RGB565(e0e0e0) );
	BIOS::LCD::Line( m_rcClient.left + 4, m_rcClient.bottom - 19, 
		m_rcClient.right - 4, m_rcClient.bottom - 19, RGB565(808080) );
	BIOS::LCD::Printf( m_rcClient.left + 8, m_rcClient.bottom - 16, RGB565(000000), RGBTRANS, "\x10 Select  \xfe Close");
	PrintBold( m_rcClient.left + 8 + 120, m_rcClient.bottom - 16 - 18, RGB565(ffff00), RGB565(000000), str);
	//BIOS::LCD::Printf( m_rcClient.left + 8 + 130, m_rcClient.top + 8, RGB565(000000), RGBTRANS, str);
}

void CWndToolbox::PrintBold( int x, int y, ui16 clrFront, ui16 clrBorder, PCSTR szLabel )
{
	BIOS::LCD::Print( x-1, y  , clrBorder, RGBTRANS, szLabel );
	BIOS::LCD::Print( x  , y+1, clrBorder, RGBTRANS, szLabel );
	BIOS::LCD::Print( x+1, y  , clrBorder, RGBTRANS, szLabel );
	BIOS::LCD::Print( x  , y-1, clrBorder, RGBTRANS, szLabel );

	BIOS::LCD::Print( x-1, y-1, clrBorder, RGBTRANS, szLabel );
	BIOS::LCD::Print( x-1, y+1, clrBorder, RGBTRANS, szLabel );
	BIOS::LCD::Print( x+1, y-1, clrBorder, RGBTRANS, szLabel );
	BIOS::LCD::Print( x+1, y+1, clrBorder, RGBTRANS, szLabel );

	BIOS::LCD::Print( x+0, y+0, clrFront, RGBTRANS, szLabel );
}

/*virtual*/ BOOL CWndToolbox::IsRunning()
{
	return m_bRunning;
}

/*virtual*/ void CWndToolbox::OnKey(ui16 nKey)
{
	if ( nKey == BIOS::KEY::KeyDown )
	{
		if ( m_nFocus < MenuMax )
		{
			m_nFocus++;
			Invalidate();
		}
		return;
	}
	if ( nKey == BIOS::KEY::KeyUp )
	{
		if ( m_nFocus > 0 )
		{
			m_nFocus--;
			Invalidate();
		}
		return;
	}
	if ( nKey == BIOS::KEY::KeyEnter )
	{
		m_bRunning = FALSE;
		return;
	}
	if ( nKey == BIOS::KEY::KeyLeft || nKey == BIOS::KEY::KeyRight )
		return;

	m_nFocus = -1;
	m_bRunning = FALSE;
}

/*virtual*/ int CWndToolbox::GetResult()
{
	return m_nFocus;
}

void CWndToolbox::DoModal()
{
	BIOS::SYS::Beep(100);
	/*
#ifdef _WIN32
	// no enough ram for this on ARM M3 :(
	ui16 buffer[Width*Height];
	BIOS::LCD::GetImage( m_rcClient, buffer );
#endif
	*/
	m_bRunning = true;
	m_bFirst = true;
	m_bAdcEnabled = BIOS::ADC::Enabled();
	BIOS::ADC::Enable(false);

	if ( MainWnd.m_wndMenuInput.m_wndListTrigger.IsVisible() )
		MainWnd.m_wndMenuInput.m_wndListTrigger.Invalidate();
	if ( MainWnd.m_wndMenuInput.m_itmTrig.IsVisible() )
		MainWnd.m_wndMenuInput.m_itmTrig.Invalidate();

	CWnd* pSafeFocus = GetFocus();
	SetFocus();
	ShowWindow( CWnd::SwShow );
	Invalidate();
	while ( IsRunning() )
	{
		Sleep(20);
	}
	ShowWindow( CWnd::SwHide );
	/*
#ifdef _WIN32
	BIOS::LCD::PutImage( m_rcClient, buffer );
#endif*/

	switch ( GetResult() )
	{
		case MenuPauseResume: 
			// Resume / Pause
			m_bAdcEnabled = !m_bAdcEnabled;
			break;
		case MenuManager:
			m_bAdcEnabled = FALSE;
			// Load wave BIN
			break;
		case MenuReset:
			Settings.Reset();
			break;
		case MenuSave:
			Settings.Save();
			break;
		case -1: break;
	}

	UpdateAdc();

	pSafeFocus->SetFocus();

	CWnd::PushOverlay();
	MainWnd.Invalidate(); // to redraw the graph
	CWnd::PopOverlay();
}

/*virtual*/ void CWndToolbox::ToggleAdc()
{
	m_bAdcEnabled = !BIOS::ADC::Enabled();
	UpdateAdc();
}

/*virtual*/ void CWndToolbox::UpdateAdc()
{
	BIOS::ADC::Enable( m_bAdcEnabled );
	if ( m_bAdcEnabled )
	{
		BIOS::ADC::Restart();
		if ( Settings.Trig.Sync == CSettings::Trigger::_Single )
			Settings.Trig.State = CSettings::Trigger::_Wait;
	}

	if ( MainWnd.m_wndMenuInput.m_wndListTrigger.IsVisible() )
		MainWnd.m_wndMenuInput.m_wndListTrigger.Invalidate();
	if ( MainWnd.m_wndMenuInput.m_itmTrig.IsVisible() )
		MainWnd.m_wndMenuInput.m_itmTrig.Invalidate();
}
