#ifndef __SCREENSAVER_H__
#define __SCREENSAVER_H__

#include <Source/Framework/Wnd.h>

class CWndScreenSaver : public CWnd
{
public:
	bool bReset;
	int x, y;

	CWndScreenSaver()
	{
		bReset = false;
	}
	
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndScreenSaver", dwFlags | CWnd::WsNoActivate, CRect(0, 16, 400, 240), pParent);
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
		int n = Random();
		int i = n%3;
		x += ax[i];
		x >>= 1;
		y += ay[i];
		y >>= 1;
		BIOS::LCD::PutPixel( x, y, (ui16)(n<<2));
	}

	virtual void OnTick()	
	{
		Invalidate();
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
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

private:
	unsigned int Random()
	{
		// our initial starting seed is 5323
		static unsigned int nSeed = 5323;
		static unsigned int nX = 0;

		// Take the current seed and generate a new value from it
		// Due to our use of large constants and overflow, it would be
		// very hard for someone to predict what the next number is
		// going to be from the previous one.
		nSeed = (8253729 * nSeed + 2396403);
		nSeed += nX++;

		// Take the seed and return a value between 0 and 32767
		return nSeed & 32767;
	}
};

#endif