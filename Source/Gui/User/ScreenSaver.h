ADD_MODULE( "Demo", CWndScreenSaver )

#ifndef __SCREENSAVER_H__
#define __SCREENSAVER_H__

#include <Source/Framework/Wnd.h>
#include <Source/Core/Utils.h>

class CWndScreenSaver : public CWnd
{
public:
	int m_x, m_y;

	CWndScreenSaver()
	{
		m_x = -1;
		m_y = -1;
	}
	
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndScreenSaver", dwFlags | CWnd::WsTick | CWnd::WsNoActivate, CRect(0, 16, 400, 240), pParent);
	}

	virtual void OnPaint()
	{
		BIOS::LCD::Bar(m_rcClient, RGB565(FFFFFF));
		BIOS::LCD::Print(m_rcClient.left+8, m_rcClient.bottom-20, RGB565(000000), RGBTRANS, "Hello world!");
	}

	void OnTick()	
	{
		const int ax[3] = {20, 380, 200};
		const int ay[3] = {40, 50, 220};

		if ( m_x == -1 )
		{
			m_x = ax[0];
			m_y = ay[0];
		}

		int n = CUtils::Random();
		int i = n % 3;
		m_x = (m_x + ax[i]) / 2;
		m_y = (m_y + ay[i]) / 2;
		BIOS::LCD::PutPixel( m_x, m_y, (ui16)(n << 2) );
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		if ( code == ToWord('t', 'i') )
		{
			// CWnd::WsTick -> OnTick, this function will be called every frame
			OnTick();
			return;
		}
	}
};

#endif