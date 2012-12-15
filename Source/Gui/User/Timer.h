//#define ENABLE_MODULE_TIMER

#if defined(ADD_MODULE) && defined(ENABLE_MODULE_TIMER)
	ADD_MODULE( "Timer", CWndUserTimer )
#endif

#if !defined(__TIMER_H__) && defined(ENABLE_MODULE_TIMER)
#define __TIMER_H__

#include <Source/Framework/Wnd.h>
#include <Source/Core/Utils.h>
#include <Source/Core/Bitmap.h>
#include <Source/Gui/Windows/WndButton.h>

class CWndUserTimer : public CWnd
{
	static const unsigned char bitmapDigit[];
	static const unsigned char bitmapDots[];
	int m_nDefault;
	int	m_nValue;
	bool m_bRunning;
	int m_nBeep;

	CWndButton m_btnStartPause;
	CWndButton m_btnReset;
	CWndButton m_btnDigit[4];

public:
	virtual void Create(CWnd *pParent, ui16 dwFlags);
	virtual void OnTimer();
	void SetDigit( CBitmap& bmp, int nValue, ui16 clrBack, ui16 clrFront1, ui16 clrFront2 );
	void DrawNumbers( char* numbers, ui16 clrBack, ui16 clrFront1, ui16 clrFront2 );
	void DrawNumbers();
	virtual void OnPaint();
	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);
	virtual void OnKey(ui16 nKey);
};

#endif