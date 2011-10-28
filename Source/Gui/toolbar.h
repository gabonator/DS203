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

public:
	virtual void Create(CWnd *pParent) 
	{
		m_nFocus = 0;
		CWnd::Create("CToolbarWnd", WsVisible, CRect(0, 0, BIOS::LCD::LcdWidth, 16), pParent);
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & BIOS::KEY::KeyLeft && m_nFocus > 0 )
		{
			m_nFocus--;
			Invalidate();
			SendMessage(m_pParent, ToWord('f', 'c'), m_nFocus);
		}
		if ( nKey & BIOS::KEY::KeyRight && m_nFocus < 7 )
		{
			m_nFocus++;
			Invalidate();
			SendMessage(m_pParent, ToWord('f', 'c'), m_nFocus);
		}
		CWnd::OnKey( nKey );
	}

	virtual void OnPaint()
	{
		const char* pszItems[] = 
			{"Input", "Cursor", "Meas", "Math", "Disp", "Gen", "Set", "Tool", NULL};

		// gradient background
		CDesign::GradientTop( m_rcClient );

		int x = m_rcClient.left;
		for (int i=0; pszItems[i]; i++)
		{
			ui8 bSelected = (i==m_nFocus);
			u16 clr = bSelected ? clrSelected : clrNormal;
			u16 bgr = bSelected ? 0 : RGBTRANS;

			if ( HasFocus() && bSelected )
			{
				clr = clrSelectedFocus;
				bgr = bgrSelectedFocus;
			}
			
			if ( bSelected )
				x += BIOS::LCD::Draw( x, m_rcClient.top, 0, RGBTRANS, corner_left);
			else
				x += 8;

			x += BIOS::LCD::Print( x, m_rcClient.top, clr, bgr, pszItems[i]);

			if ( bSelected )
				x += BIOS::LCD::Draw( x, m_rcClient.top, 0, RGBTRANS, corner_right);
			else
				x += 8;
		}

	}
};

#endif