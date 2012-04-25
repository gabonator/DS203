#ifndef __CALIBRATION_H__
#define __CALIBRATION_H__

#include <Source/Framework/Wnd.h>
#include <Source/Main/Application.h>

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
	void OnWave();
	void OnInit();
	void OnStart();
	void OnStop();
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