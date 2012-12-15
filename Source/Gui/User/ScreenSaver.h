//ADD_MODULE( "Demo", CWndScreenSaver )

#ifndef __SCREENSAVER_H__
#define __SCREENSAVER_H__

#include <Source/Framework/Wnd.h>
#include <Source/Core/Utils.h>

class CWndScreenSaver : public CWnd
{
public:
	bool bReset;
	int x, y;

	CWndScreenSaver()
	{
		bReset = true;
		x = 0;
	}
	
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndScreenSaver", dwFlags | CWnd::WsTick | CWnd::WsNoActivate, CRect(0, 16, 400, 240), pParent);
	}

	virtual void OnPaint()
	{
		const int ax[3] = {20, 380, 200};
		const int ay[3] = {40, 50, 220};
		if ( bReset )
		{
			BIOS::LCD::Bar(m_rcClient, RGB565(FFFFFF));
			BIOS::LCD::Print(m_rcClient.left+8, m_rcClient.bottom-20, RGB565(000000), RGBTRANS, "Hello world!");
			bReset = false;
			x = ax[0];
			y = ay[0];
		}
		int n = CUtils::Random();
		int i = n%3;
		x += ax[i];
		x >>= 1;
		y += ay[i];
		y >>= 1;
		BIOS::LCD::PutPixel( x, y, (ui16)(n<<2));
	}

	void OnTick()	
	{
		Invalidate();
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		if (code == ToWord('t', 'i') )
		{
			// OnTick
			OnTick();
			return;
		}

		// LAYOUT ENABLE/DISABLE FROM TOP MENU BAR
		if (code == ToWord('L', 'D') )
		{
			return;
		}

		if (code == ToWord('L', 'E') )
		{
			bReset = true;
			return;
		}
	}
};

#endif