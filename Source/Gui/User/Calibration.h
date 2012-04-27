#ifndef __CALIBRATION_H__
#define __CALIBRATION_H__

#include <Source/Framework/Wnd.h>
#include <Source/Main/Application.h>
#include <Source/Core/Controls.h>
#include <Source/Core/ListItems.h>
#include <Source/Core/Settings.h>

#include "CalibDac.h"

class CWndMenuCalibration : public CWnd
{
public:
	CWndMenuItem	m_itmAdc;
	CWndMenuItem	m_itmAnalog;

	CWndListCalAdc	m_wndListAdc;

	CWndMenuCalibration()
	{
	}

	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndMenuCalibration", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
		m_itmAdc.Create("Adc", RGB565(00b040), 2, this);
		m_itmAnalog.Create("Analog", RGB565(00b040), 2, this);
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		if ( data == (ui32)&m_wndListAdc.m_proExecute && code == ToWord('l', 'e' ) )
		{
			m_wndListAdc.m_itmExecute.SendMessage( &m_wndListAdc, code, 0 );
		}

		if ( code == ToWord('m', 'o') && pSender == &m_itmAdc )
		{
			m_wndListAdc.Create( this );
			m_wndListAdc.StartModal( &m_wndListAdc.m_itmExecute );
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

	}
};

class CWndCalibration : public CWnd
{
	int nDownload;
public:
	CWndCalibration()
	{
	}
	
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndCalibration", dwFlags | CWnd::WsListener, CRect(0, 16, 400, 240), pParent);
	}

	virtual void OnPaint()
	{
		if ( GetFocus() == this )
		{
			BIOS::LCD::Print( 40, 40, RGB565(000000), RGB565(ffffff), "Press enter to start calibration" );
			BIOS::LCD::Print( 40, 56, RGB565(ffffff), RGB565(000000), "Connect wave out to selected channel" );
		}
		else
			BIOS::LCD::Print( 40, 40, RGB565(ffffff), RGB565(000000), "Press down to enable this window" );		
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
		{
			OnWave();
			return;
		}
		// LAYOUT ENABLE/DISABLE FROM TOP MENU BAR
		if (code == ToWord('L', 'D') )
		{
			return;
		}

		if (code == ToWord('L', 'E') )
		{
			OnInit();
			return;
		}
	}

	/*virtual*/ void OnKey(ui16 nKey)
	{
		if ( !IsRunning() )
			CWnd::OnKey( nKey );

		if ( nKey & BIOS::KEY::KeyEnter )
		{
			if ( !IsRunning() )
			{
				OnStart();
				return;
			}
		}
		if ( IsRunning() )
			OnStop();
	}

	//
	void OnStart();
	void OnStop();

	void OnWave();
	void OnInit();
	bool IsRunning();

	static void Wait( int nTime )
	{
		ui32 nLastTime = BIOS::GetTick() + nTime;
		do 
		{
			Application.operator()();
		} while ( BIOS::GetTick() < nLastTime);
	}

};

#endif