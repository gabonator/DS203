#ifndef __CALIBDAC_H__
#define __CALIBDAC_H__

#include <Source/Framework/Wnd.h>
#include <Source/Main/Application.h>
#include <Source/Core/Controls.h>
#include <Source/Core/ListItems.h>
#include <Source/Core/Settings.h>

class CWndListCalDac : public CListBox
{
public:
	CLStaticItem	m_itmStaticA;
	CLStaticItem	m_itmStaticB;
	CLStaticItem	m_itmStaticC;
	CLStaticItem	m_itmStaticD;

	CProviderDigit	m_proADigit1000;
	CProviderDigit	m_proADigit100;
	CProviderDigit	m_proADigit10;
	CProviderDigit	m_proADigit1;
	CDigitsItem<4>	m_itmANumber;

	CProviderDigit	m_proBDigit1000;
	CProviderDigit	m_proBDigit100;
	CProviderDigit	m_proBDigit10;
	CProviderDigit	m_proBDigit1;
	CDigitsItem<4>	m_itmBNumber;

	CProviderBtn	m_proExecute;
	CLPItem			m_itmExecute;

	int				m_nValueA;
	int				m_nValueB;

public:
	void Create( CWnd* pParent )
	{
		Load();
		CListBox::Create( "ADC Calibration", WsVisible | WsModal, CRect(20, 30, 280, 201), RGB565(8080b0), pParent );

		m_itmStaticA.Create( "Set the numbers to get", CWnd::WsVisible | CWnd::WsNoActivate, this);
		m_itmStaticB.Create( "following voltage on wave out", CWnd::WsVisible | CWnd::WsNoActivate, this);
		m_itmStaticC.Create( "", CWnd::WsVisible | CWnd::WsNoActivate, this);
		
		m_proADigit1000.Create( &m_nValueA, 1000 );
		m_proADigit100.Create( &m_nValueA, 100 );
		m_proADigit10.Create( &m_nValueA, 10 );
		m_proADigit1.Create( &m_nValueA, 1 );

		CValueProvider* arrADigits[4] = {&m_proADigit1000, &m_proADigit100, &m_proADigit10, &m_proADigit1};
		m_itmANumber.Create("  0.5 V", CWnd::WsVisible, this, arrADigits);

		m_proBDigit1000.Create( &m_nValueB, 1000 );
		m_proBDigit100.Create( &m_nValueB, 100 );
		m_proBDigit10.Create( &m_nValueB, 10 );
		m_proBDigit1.Create( &m_nValueB, 1 );

		CValueProvider* arrBDigits[4] = {&m_proBDigit1000, &m_proBDigit100, &m_proBDigit10, &m_proBDigit1};
		m_itmBNumber.Create("  1.5 V", CWnd::WsVisible, this, arrBDigits);

		m_itmStaticD.Create( "", CWnd::WsVisible | CWnd::WsNoActivate, this);

		m_proExecute.Create( "Ok!" );
		m_itmExecute.Create( "Use values", CWnd::WsVisible, &m_proExecute, this );
	}

	void Load()
	{
		m_nValueA = Settings.DacCalib.m_arrCurveOut[0];
		m_nValueB = Settings.DacCalib.m_arrCurveOut[1];
	}

	void Save()
	{
		Settings.DacCalib.m_arrCurveOut[0] = m_nValueA;
		Settings.DacCalib.m_arrCurveOut[1] = m_nValueB;
		Settings.DacCalib.m_arrCurveIn[0] = 0.5f;
		Settings.DacCalib.m_arrCurveIn[1] = 1.5f;
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);
};

#endif