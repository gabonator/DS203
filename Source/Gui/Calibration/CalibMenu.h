#ifndef __CALIBMENU_H__
#define __CALIBMENU_H__

#include <Source/Framework/Wnd.h>
#include <Source/Core/Controls.h>
#include <Source/Core/ListItems.h>
#include <Source/Core/Settings.h>
#include "CalibAnalog.h"
#include "CalibDac.h"

class CWndMenuCalibration : public CWnd
{
public:
	CWndMenuItem	m_itmAdc;
	CWndMenuItem	m_itmAnalog;
	CWndMenuItem	m_itmLoad;
	CWndMenuItem	m_itmSave;
	CWndMenuItem	m_itmReset;

	CWndListCalDac	m_wndListAdc;
	CWndListCalAnalog	m_wndListAnalog;

	CWndMenuCalibration()
	{
	}

	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndMenuCalibration", dwFlags, CRect(400-120, 20, 400, 240), pParent);
		m_itmAdc.Create("Adc", RGB565(00b040), 2, this);
		m_itmAnalog.Create("Analog", RGB565(00b040), 2, this);
		m_itmLoad.Create("Load\ncalib data", RGB565(ffffff), 2, this);
		m_itmSave.Create("Save\ncalib data", RGB565(ffffff), 2, this);
		m_itmReset.Create("Reset\ncalib data", RGB565(ffffff), 2, this);
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);
};

#endif
