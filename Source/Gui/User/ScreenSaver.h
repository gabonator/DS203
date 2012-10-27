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
		bReset = true;
		x = 0;
	}
	
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndScreenSaver", dwFlags | CWnd::WsTick | CWnd::WsNoActivate, CRect(0, 16, 400, 240), pParent);
	}

	virtual void OnPaint()
	{/*
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
		*/
		if ( x++ > 10 )
			return;
		BIOS::LCD::Bar(m_rcClient, RGB565(FFFFFF));
		BIOS::LCD::Print(m_rcClient.left+8, m_rcClient.top + 8, RGB565(B00000), RGBTRANS, "Filter: h[0.00] = 1.00, h[-6.11] = 0.79");
		BIOS::LCD::Print(m_rcClient.left+8, m_rcClient.top + 8+16, RGB565(000000), RGBTRANS, "Signal quality: 36% ");
		BIOS::LCD::Print(m_rcClient.left+8, m_rcClient.top + 8+16+16, RGB565(000000), RGBTRANS, "Reconstructions: 69, errors: 0 ");//, 21*3+2*3);

		CRect rcDecod( m_rcClient.left + 8, m_rcClient.top + 8+16+16+16+8, m_rcClient.right - 8, m_rcClient.bottom - 16 - 8 );
		BIOS::LCD::Bar(rcDecod, RGB565(404060));
		const char *msg = "<LSHIFT>a</LSHIFT>hoj, toto je pokus <LSHIFT>1</LSHIFT>";
		int xx = rcDecod.left+2;
		int yy = rcDecod.top+2;
		const char* pmsg = msg;
		char ch;
		int clr = RGB565(ffff00);
		while (ch = *pmsg++)
		{
			if (ch == '<')
				clr = RGB565(00ff00);
			BIOS::LCD::Printf(xx, yy, clr, RGBTRANS, "%c", ch);
			xx += 8;
			if ( xx >= rcDecod.right-8 )
			{
				xx = rcDecod.left+2;
				yy+= 16;
			}
			if (ch == '>')
				clr = RGB565(ffff00);
		}
		//BIOS::LCD::Print(rcDecod.left + 2, rcDecod.top + 2, RGB565(000000), RGBTRANS, "<LSHIFT>A</LSHIFT>hoj, toto je pokus <LSHIFT>1</LSHIFT>");

		BIOS::LCD::Print(m_rcClient.left+8, m_rcClient.bottom-20, RGB565(000000), RGBTRANS, "Code: F0 12");
		BIOS::LCD::Print(m_rcClient.right-200, m_rcClient.bottom-20, RGB565(00ff00), 0, "Trig waiting...");
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