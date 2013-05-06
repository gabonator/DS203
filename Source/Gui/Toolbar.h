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

	struct CBarItem
	{
		enum EType {
			IMain,
			ISub,
			IEnd
		};

		EType m_eType;                                   
		PSTR m_pName;
		const CWnd* m_pWndMenu; 
	};

	static const CBarItem m_arrMenuItems[];


public:
	
	virtual void Create(CWnd *pParent) 
	{
		m_nFocus = 0;
		CWnd::Create("CToolbarWnd", WsVisible, CRect(0, 0, BIOS::LCD::LcdWidth, 16), pParent);
	}

	virtual void	OnKey(ui16 nKey);
	virtual void	OnPaint();
	virtual void	OnMessage(CWnd* pSender, ui16 code, ui32 data);
	CWnd*			GetCurrentLayout();
	void			ChangeFocus(ui8 oldFocus);

public:
	static const CBarItem* GetMenuItems();
	static int Find( const char* strId );
	int _FindItemByPoint( int mx );

};
	
#endif