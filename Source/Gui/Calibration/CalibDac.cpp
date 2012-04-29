#include "CalibDac.h"
#include "Source/Gui/MainWnd.h"

/*virtual*/ void CWndListCalDac::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	if ( pSender == &m_itmANumber && code == ToWord('u', 'p') )	
	{
		if ( m_nValueA < 100 )
		{
			m_nValueA = 100;
			m_itmANumber.Invalidate();
		}
		if ( m_nValueA > 8000 )
		{
			m_nValueA = 8000;
			m_itmANumber.Invalidate();
		}
		BIOS::GEN::ConfigureDc( m_nValueA );
	}
	if ( pSender == &m_itmBNumber && code == ToWord('u', 'p') )	
	{
		if ( m_nValueB < 100 )
		{
			m_nValueB = 100;
			m_itmBNumber.Invalidate();
		}
		if ( m_nValueB > 8000 )
		{
			m_nValueB = 8000;
			m_itmBNumber.Invalidate();
		}
		BIOS::GEN::ConfigureDc( m_nValueB );
	}

	// Selector
	if ( pSender == &m_itmExecute && code == ToWord('l', 'e') )	// provider selector combo box
	{
		Save();
		StopModal();
		//MainWnd.m_wndConfirm.Show( this, "Message", "Do you really want to\nsave these settings?", RGB565(ffff00), "Yes", "No");
	}
	/*
	if ( pSender == &MainWnd.m_wndConfirm && code == ToWord('e', 'd') && data == (ui32)"Yes" )
	{
		MainWnd.m_wndConfirm.Hide();
		StopModal();
		Save();
		Settings.SaveCalibration();
		MainWnd.m_wndMessage.Show( this, "Message", "Values saved", RGB565(ffff00));
	}
	if ( pSender == &MainWnd.m_wndConfirm && code == ToWord('e', 'd') && data == (ui32)"No" )
	{
		MainWnd.m_wndConfirm.Hide();
		StopModal(); // hide this list
	}*/
}
	
