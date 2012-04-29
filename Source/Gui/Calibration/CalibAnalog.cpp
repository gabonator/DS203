#include "CalibAnalog.h"
#include <Source/Gui/MainWnd.h>

void CWndListCalAnalog::Finished()
{
	// calibration finished
	CSettings::LinCalibCurve& calCurve = Settings.CH1Calib.CalData[m_calRange];
	if ( m_calChannel == CSettings::Trigger::_CH2 )
		calCurve = Settings.CH2Calib.CalData[m_calRange];

	calCurve = m_calCurve;
	MainWnd.Invalidate();
	// parent doesn't exist anymore, window is destroyed!
	MainWnd.m_wndMessage.Show( &MainWnd, "Information", "Calibration finished!", RGB565(ffff00));
}

void CWndListCalAnalog::Aborted()
{
	MainWnd.Invalidate();
	MainWnd.m_wndMessage.Show( &MainWnd, "Information", "Error or calibration aborted", RGB565(ffff00));
}
