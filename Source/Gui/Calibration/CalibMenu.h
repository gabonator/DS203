#ifndef __CALIBMENU_H__
#define __CALIBMENU_H__

#include <Source/Framework/Wnd.h>
#include <Source/Core/Controls.h>
#include <Source/Core/ListItems.h>
#include <Source/Core/Settings.h>
#include "CalibAnalog.h"
#include "CalibDac.h"

class CMIAdc : public CWndMenuItem
{
	int	m_nVolt;

public:
	virtual void Create(const char* pszId, ui16 clr, ui8 rows, CWnd *pParent) 
	{
		m_nVolt = 5;
		CWndMenuItem::Create(pszId, clr, rows, pParent);
	}
	void UpdateAdc()
	{
		float fVolt = m_nVolt / 10.0f;
		int nAdc = Settings.DacCalib.Get( fVolt );
		BIOS::GEN::ConfigureDc( nAdc );
	}
	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & BIOS::KEY::KeyLeft && m_nVolt > 3 )
		{
			m_nVolt--;
			Invalidate();
			UpdateAdc();
		}
		if ( nKey & BIOS::KEY::KeyRight && m_nVolt < 25 )
		{
			m_nVolt++;
			Invalidate();
			UpdateAdc();
		}
		CWndMenuItem::OnKey( nKey );
	}
	virtual void OnPaint()
	{
		CWndMenuItem::OnPaint();
		BIOS::LCD::Printf( m_rcClient.left + 12 + 16, m_rcClient.top + 16, RGB565(000000), RGBTRANS, "%1f V", 
			m_nVolt/10.0f+0.01f ); // rounding error!?
	}

};

class CWndMenuCalibration : public CWnd
{
public:
	CMIAdc			m_itmAdc;
	CWndMenuItem	m_itmSimple;
	CWndMenuItem	m_itmComplex;
	CWndMenuItem	m_itmLoad;
	CWndMenuItem	m_itmSave;
	CWndMenuItem	m_itmReset;

	CWndListCalDac	m_wndListAdc;
//	CWndListCalAnalog	m_wndListAnalog;
	CWndListCalSimple	m_wndListSimple;

	CWndMenuCalibration()
	{
	}

	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndMenuCalibration", dwFlags, CRect(80, 40, 320, 260), pParent);
		m_itmAdc.Create("Adc", RGB565(8080b0), 2, this);
		m_itmSimple.Create("Analog-Simple", RGB565(8080b0), 1, this);
		m_itmLoad.Create("Load\ncalib data", RGB565(ffffff), 2, this);
		m_itmSave.Create("Save\ncalib data", RGB565(ffffff), 2, this);
		m_itmReset.Create("Reset\ncalib data", RGB565(ffffff), 2, this);
//		m_itmComplex.Create("Analog-Compl.", RGB565(404040), 1, this);
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);
};

#endif
