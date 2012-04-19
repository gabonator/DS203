#ifndef __METER_H__
#define __METER_H__

#include <Source/Framework/Wnd.h>


class CWndUserMeter : public CWnd
{
public:
	float m_fAverage, m_fVariance;
	bool bFirst;

	CWndUserMeter()
	{
		m_fAverage = 0;
		bFirst = true;
	}
	
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndUserMeter", dwFlags /*| CWnd::WsNoActivate*/, CRect(0, 16, 400, 240), pParent);
	}

	void DrawDigit(int x, int y, int n, ui16 clrOn, ui16 clrOff );
	void DrawDigitH(int x, int y, ui16 clr );
	void DrawDigitV(int x, int y, ui16 clr );
	void DrawDot(int x, int y, ui16 clr );
	void OnWave();

	virtual void OnPaint();
	virtual void OnTimer();

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		// LAYOUT ENABLE/DISABLE FROM TOP MENU BAR
		if (code == ToWord('L', 'D') )
		{
			return;
		}

		if (code == ToWord('L', 'E') )
		{
			bFirst = true;
			return;
		}
		if (code == ToWord('d', 'g') )
		{
			OnWave();
			return;
		}
	}

};

#endif