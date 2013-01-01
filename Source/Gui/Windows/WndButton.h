#ifndef __WNDBUTTON_H__
#define __WNDBUTTON_H__

#include <Source/Core/Shapes.h>

class CWndButton : public CWnd
{
public:
	virtual void OnPaint()
	{
		ui16 clr = HasFocus() ? RGB565(ffffff) : RGB565(000000);

		ui16 x = m_rcClient.left;
		x -= 8;
		x += BIOS::LCD::Draw( x, m_rcClient.top, clr, RGBTRANS, CShapes::sel_left );
		x += BIOS::LCD::Print( x, m_rcClient.top, RGBTRANS, clr, this->m_pszId );
		x += BIOS::LCD::Draw( x, m_rcClient.top, clr, RGBTRANS, CShapes::sel_right );
	}	
	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & ( BIOS::KEY::KeyEnter | BIOS::KEY::KeyLeft | BIOS::KEY::KeyRight ) )
		{
			GetParent()->OnKey( nKey );
			return;
		}
		CWnd::OnKey( nKey );
	}
};

#endif
