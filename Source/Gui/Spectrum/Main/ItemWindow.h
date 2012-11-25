#ifndef __MENUITESPECWINDOW_H__
#define __MENUITESPECWINDOW_H__
#include <Source/Gui/Oscilloscope/Disp/ItemDisp.h>

class CItemSpecWindow : public CMPItem
{
	CProviderEnum	m_proWindow;

public:
	virtual void Create(CWnd *pParent) 
	{
		CWndMenuItem::Create( NULL, RGB565(8080b0), 3, pParent);

		m_proWindow.Create( (const char**)CSettings::Spectrum::ppszTextWindow,
			(NATIVEENUM*)&Settings.Spec.Window, CSettings::Spectrum::_WindowMax );
		m_pProvider = &m_proWindow; // for CMPItem
	}

	virtual void OnPaint()
	{
		ui16 clr = RGB565(000000);

		CWndMenuItem::OnPaint();
		int x = m_rcClient.left + 12 + MarginLeft;
		int y = m_rcClient.top;
		int _x = x;
		BIOS::LCD::Print( x, y, clr, RGBTRANS, "Window" );
		y += 16;
		if ( HasFocus() )
		{
			x += BIOS::LCD::Draw(x, y, RGB565(000000), RGBTRANS, CShapes::sel_left);
			CRect rcRect(x, y, x + m_pProvider->GetWidth(), y + 14);
			BIOS::LCD::Bar( rcRect, RGB565(0000000) );
			m_pProvider->OnPaint( rcRect, HasFocus() );
			x = rcRect.right;
			x += BIOS::LCD::Draw(x, y, RGB565(000000), RGBTRANS, CShapes::sel_right);
		} else
		{
			CRect rcRect(x, y, x + m_pProvider->GetWidth(), y + 14);
			m_pProvider->OnPaint( rcRect, HasFocus() );
		}

		x = _x;
		y += 16;
		x += BIOS::LCD::Print( x, y, clr, RGBTRANS, "N=" );
		BIOS::LCD::Print( x, y, clr, RGBTRANS, CUtils::itoa(Settings.Spec.nWindowLength) );
	}
/*
	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & BIOS::KEY::KeyLeft && (*m_pProvider)-1 == CValueProvider::Yes )
		{
			(*m_pProvider)--;
			Invalidate();
			SendMessage(m_pParent, ToWord('u', 'p'), 0);
		}
		if ( nKey & BIOS::KEY::KeyRight && (*m_pProvider)+1 == CValueProvider::Yes )
		{
			(*m_pProvider)++;
			Invalidate();
			SendMessage(m_pParent, ToWord('u', 'p'), 0);
		}
		if ( nKey & BIOS::KEY::KeyEnter  )
		{
			// is that provider enumerator ?
			if ( m_pProvider->Get() !=	CValueProvider::Invalid )
			{
				SendMessage(m_pParent, ToWord('l', 'e'), (ui32)(NATIVEPTR)(m_pProvider));
			}
		}
		CWndMenuItem::OnKey( nKey );
	}
*/
};

#endif

