#ifndef __SPECANNOT_H__
#define __SPECANNOT_H__

#include <Source/Framework/Wnd.h>

class CWndSpecAnnotations : public CWnd
{
public:
	virtual void Create(CWnd *pParent, ui16 wFlags)
	{
		CWnd::Create("CWndSpecAnnotations", wFlags | CWnd::WsNoActivate, CRect(0, 0, 1, 1), pParent);
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
	}

	virtual void OnPaint();

};

#endif