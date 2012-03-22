#ifndef __TOOLBAR_H_
#define __TOOLBAR_H_

#include <Source/Framework/Wnd.h>
#include <Source/Core/Design.h>
#include <Source/Core/Shapes.h>

class CWndToolBar : public CWnd
{
public:
	ui8	m_nFocus;
	enum {
		clrSelected = RGB565(ffffff),
		clrNormal = RGB565(b0b0b0),
		
		clrSelectedFocus = RGB565(000000),
		bgrSelectedFocus = RGB565(ffffff),
	};

	class CBarItem
	{
	public:
		enum EType {
			IMain,
			ISub,
			IEnd
		};

		EType m_eType;                                   
		PCSTR m_pName;
		CWnd* m_pWndMenu; 
	};

public:
	
	virtual void Create(CWnd *pParent) 
	{
		m_nFocus = 0;
		CWnd::Create("CToolbarWnd", WsVisible, CRect(0, 0, BIOS::LCD::LcdWidth, 16), pParent);
	}

	virtual void OnKey(ui16 nKey);
	virtual void OnPaint();
	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);

private:
	CBarItem* GetMenuItems();
};
	
#endif