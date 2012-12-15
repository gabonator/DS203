#ifndef __DMM_H__
#define __DMM_H__

#include <Source/Framework/Wnd.h>
#include <Source/Gui/Dmm/Mode/MenuMode.h>


class CWndDmm : public CWnd
{
public:
	float m_fAverage, m_fVariance;
	bool bRefresh;
	bool bTimer;

	CWndDmm()
	{
		bRefresh = true;
		m_fAverage = 0;
		bTimer = false;
	}
	
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndUserDmm", dwFlags | CWnd::WsListener | CWnd::WsNoActivate, CRect(0, 16, 320-CWndMenuItem::MarginLeft, 240), pParent);
	}

	void DrawDigit(int x, int y, int width, int size, int space, int n, ui16 clrOn, ui16 clrOff );
	void OnWave();

	virtual void OnPaint();
	virtual void OnTimer();

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		// LAYOUT ENABLE/DISABLE FROM TOP MENU BAR
		if (code == ToWord('L', 'D') )
		{
			bTimer = false;
			KillTimer();
			return;
		}

		if (code == ToWord('L', 'E') )
		{
			bRefresh = true;
			return;
		}

		if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
		{
			OnWave();
			return;
		}
	}

};

#endif