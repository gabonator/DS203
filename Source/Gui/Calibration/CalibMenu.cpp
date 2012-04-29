#include "CalibMenu.h"
#include <Source/Framework/Wnd.h>
#include <Source/Main/Application.h>
#include <Source/Core/Controls.h>
#include <Source/Core/ListItems.h>
#include <Source/Core/Settings.h>
#include <Source/Gui/MainWnd.h>

/*virtual*/ void CWndMenuCalibration::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	if ( pSender == &m_wndListAnalog && code == ToWord('e', 'x')  )
	{
		m_wndListAnalog.StopModal();
		return;
	}
	if ( pSender == &m_wndListAdc && code == ToWord('e', 'x')  )
	{
		m_wndListAdc.StopModal();
		return;
	}
	if ( pSender == &m_itmSave && code == ToWord('m', 'o')  )
	{
		MainWnd.m_wndConfirm.Show( this, "Confirmation", "Do you really want to\nsave calibration settings?", RGB565(ffff00), "Cancel", "Save");
		return;
	}
	if ( pSender == &m_itmLoad && code == ToWord('m', 'o')  )
	{
		MainWnd.m_wndConfirm.Show( this, "Confirmation", "Do you really want to\nload calibration settings?", RGB565(ffff00), "Cancel", "Load");
		return;
	}
	if ( pSender == &m_itmReset && code == ToWord('m', 'o')  )
	{
		MainWnd.m_wndConfirm.Show( this, "Confirmation", "Do you really want to set\ndefault calibration settings?", RGB565(ffff00), "Cancel", "Reset");
		return;
	}
	if ( pSender == &MainWnd.m_wndConfirm && code == ToWord('e', 'd') && data == (ui32)"Save" )
	{
		MainWnd.m_wndConfirm.Hide();
		Settings.SaveCalibration();
		MainWnd.m_wndMessage.Show( this, "Message", "Calibration data saved", RGB565(ffff00));
		return;
	}
	if ( pSender == &MainWnd.m_wndConfirm && code == ToWord('e', 'd') && data == (ui32)"Load" )
	{
		MainWnd.m_wndConfirm.Hide();
		if ( Settings.LoadCalibration() )
			MainWnd.m_wndMessage.Show( this, "Message", "Calibration data loaded", RGB565(ffff00));
		else
			MainWnd.m_wndMessage.Show( this, "Error", "Calibration data could not be loaded", RGB565(ff0000));
		return;
	}

	if ( pSender == &MainWnd.m_wndConfirm && code == ToWord('e', 'd') && data == (ui32)"Reset" )
	{
		MainWnd.m_wndConfirm.Hide();
		Settings.ResetCalibration();
		return;
	}

	if ( pSender == &MainWnd.m_wndConfirm && code == ToWord('e', 'd') && data == (ui32)"Cancel" )
	{
		MainWnd.m_wndConfirm.Hide();
		return;
	}

	if ( data == (ui32)&m_wndListAdc.m_proExecute && code == ToWord('l', 'e' ) )
	{
		m_wndListAdc.m_itmExecute.SendMessage( &m_wndListAdc, code, 0 );
		return;
	}
	if ( data == (ui32)&m_wndListAnalog.m_proExecute && code == ToWord('l', 'e' ) )
	{
		m_wndListAnalog.m_itmExecute.SendMessage( &m_wndListAnalog, code, 0 );
		return;
	}
	if ( data == (ui32)&m_wndListAnalog.m_proInfo && code == ToWord('l', 'e' ) )
	{
		m_wndListAnalog.m_itmInfo.SendMessage( &m_wndListAnalog, code, 0 );
		return;
	}

	if ( code == ToWord('m', 'o') && pSender == &m_itmAdc )
	{
		m_wndListAdc.Create( this );
		m_wndListAdc.StartModal( &m_wndListAdc.m_itmExecute );
		return;
	}

	if ( code == ToWord('m', 'o') && pSender == &m_itmAnalog )
	{
		m_wndListAnalog.Create( this );
		m_wndListAnalog.StartModal( &m_wndListAnalog.m_itmResolution );
		return;
	}

	if ( code == ToWord('o', 'k') && pSender->m_pParent == &m_wndListAdc )
	{
		// done
		m_wndListAdc.StopModal();
		return;
	}

	if ( code == ToWord('e', 'x') && pSender == &m_wndListAdc )
	{
		// cancel
		m_wndListAdc.StopModal();
		return;
	}

	if ( code == ToWord('L', 'E') || code == ToWord('L', 'D') || code == ToWord('L', 'R') )
	{
		return;
	}
}
