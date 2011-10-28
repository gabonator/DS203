#ifndef __CONTROLS_H__
#define __CONTROLS_H__

#include <Source/HwLayer/Types.h>
#include <Source/Framework/Wnd.h>
#include "Design.h"
#include "Shapes.h"
#include <string.h>

class CListBox : public CWnd
{
	ui16	m_clrFrame;

public:

	void Create( const char* pszId, ui16 dwFlags, const CRect& rc, ui16 clr, CWnd* pParent )
	{
		m_clrFrame = clr;
		CWnd::Create( pszId, dwFlags, rc, pParent );
	}

	virtual void OnPaint()
	{
		CDesign::Window( m_rcClient, m_clrFrame );
		BIOS::LCD::Print( m_rcClient.CenterX()-((ui8)strlen(m_pszId)<<2), m_rcClient.top+2, 
			RGB565(000000), RGBTRANS, m_pszId);
		/*
		CRect rcClient = m_rcClient;
		BIOS::RoundRect(rcClient, RGB565(000000));
		rcClient.Deflate(2, 2, 2, 2);

		BIOS::RoundRect(rcClient, m_clrFrame);

		BIOS::Print( rcClient.CenterX()-((ui8)strlen(m_pszId)<<2), rcClient.top, 
			RGB565(000000), RGBTRANS, m_pszId);

		rcClient.Deflate(2, 14, 2, 2);
		BIOS::RoundRect(rcClient, RGB565(b0b0b0));
		*/
	}
};

class CListItem : public CWnd
{
public:
	void Create(const char *pszId, ui16 dwFlags, CWnd* pParent)
	{
		ui8 nHeight = 16;
		CRect rcRect;

		if ( pParent->GetLast() )
		{
			rcRect = pParent->GetLast()->m_rcClient;
			rcRect.top = rcRect.bottom;
			rcRect.bottom = rcRect.top + nHeight;
		} else
		{
			rcRect = pParent->m_rcClient;
			rcRect.top += 20;
			rcRect.bottom = rcRect.top + nHeight;
			rcRect.Deflate(4, 0, 4, 0);
		}
		CWnd::Create( pszId, dwFlags, rcRect, pParent );
	}

	virtual void OnPaint()
	{
		if ( !HasFocus() )
			CDesign::ListItemDisabled( m_rcClient );
		else
			CDesign::ListItemEnabled( m_rcClient );
	}
	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & BIOS::KEY::KeyEscape )
			m_pParent->SendMessage( m_pParent->m_pParent, ToWord('e', 'x'), 0 );
		CWnd::OnKey(nKey);
	}
};

class CWndMenuItem : public CWnd
{
public:
	enum {
		Height = 34,
		Space = 2,
		MarginLeft = 4
	};
	ui16 m_clr;
	ui16* m_pClr;
	ui8 m_nRows;

public:
	virtual void Create(const char* pszId, ui16 clr, ui8 rows, CWnd *pParent) 
	{
		m_pClr = NULL;
		m_clr = clr;
		m_nRows = rows;
		ui8 nHeight = rows*17;
		CRect rcRect;

		if ( pParent->GetLast() )
		{
			rcRect = pParent->GetLast()->m_rcClient;
			rcRect.top = rcRect.bottom + Space;
			rcRect.bottom = rcRect.top + nHeight;
		} else
		{
			rcRect = pParent->m_rcClient;
			rcRect.bottom = rcRect.top + nHeight;
		}
		CWnd::Create( pszId, WsVisible, rcRect, pParent );
	}

	void SetColorPtr(ui16* pclr )
	{
		m_pClr = pclr;
	}

	virtual void OnPaint()
	{
		if ( m_pClr )
			m_clr = *m_pClr;
		BIOS::LCD::Bar(m_rcClient, RGB565(000000));
		m_rcClient.left += MarginLeft;
		if ( HasFocus() )
		{
			CDesign::MenuItemEnabled( m_rcClient, m_clr );
			BIOS::LCD::Print( m_rcClient.left+12, m_rcClient.top, RGB565(000000), RGBTRANS, m_pszId );
		} else {
			CDesign::MenuItemDisabled( m_rcClient, m_clr );
			BIOS::LCD::Print( m_rcClient.left+12, m_rcClient.top, RGB565(000000), RGBTRANS, m_pszId );
		}
		m_rcClient.left -= MarginLeft;
	}

};

class CValueProvider
{
public:
	static const ui32 Invalid;
	enum VPNavigate {
		No,
		Yes,
		Disabled,
	};

	virtual VPNavigate operator +( si8 dir ) { _ASSERT(0); return Disabled; }
	virtual VPNavigate operator -( si8 dir ) { return this->operator+(-dir); }
	virtual void operator++(int) { _ASSERT(0); }
	virtual void operator--(int) { _ASSERT(0); }
	virtual void OnPaint(const CRect& rcRect, ui8 bFocus) { _ASSERT(0); }
	virtual ui16 GetWidth() { _ASSERT(0); return 0; }
	virtual void Set(ui32 value) { _ASSERT(0); }
	virtual ui32 Get() { return Invalid; }
};

class CLPItem : public CListItem
{
	CValueProvider* m_pProvider;

public:
	void Create(const char* pszId, ui16 dwFlags, CValueProvider* pProvider, CWnd* pParent)
	{
		m_pProvider = pProvider;
		CListItem::Create( pszId, dwFlags, pParent );
	}

	virtual void OnPaint()
	{
		ui16 clr = HasFocus() ? RGB565(ffffff) : RGB565(000000);
		
		CListItem::OnPaint();

		int x = m_rcClient.left+4;
		BIOS::LCD::Print( x, m_rcClient.top, clr, RGBTRANS, m_pszId);
	
		x = m_rcClient.left + 120;
		
		CValueProvider::VPNavigate prev = (*m_pProvider)-1;
		CValueProvider::VPNavigate next = (*m_pProvider)+1;

		if (prev == CValueProvider::Yes)
			BIOS::LCD::Draw( x-8, m_rcClient.top, clr, RGBTRANS, more_left );
		else if (prev == CValueProvider::No)
			BIOS::LCD::Draw( x-8, m_rcClient.top, RGB565(808080), RGBTRANS, more_left );

		CRect rcItem;
		rcItem = m_rcClient;
		rcItem.left = x;
		rcItem.right = rcItem.left;	// force to recompute
		
		m_pProvider->OnPaint(rcItem, HasFocus());
		x += m_pProvider->GetWidth();
		if (next == CValueProvider::Yes)
			x += BIOS::LCD::Draw( x, m_rcClient.top, clr, RGBTRANS, more_right );
		else if (next == CValueProvider::No)
			x += BIOS::LCD::Draw( x, m_rcClient.top, RGB565(808080), RGBTRANS, more_right );
	}	

	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & BIOS::KEY::KeyLeft && (*m_pProvider)-1 != CValueProvider::No )
		{
			(*m_pProvider)--;
			Invalidate();
			SendMessage(m_pParent->m_pParent, ToWord('u', 'p'), 0);
		}
		if ( nKey & BIOS::KEY::KeyRight && (*m_pProvider)+1 != CValueProvider::No )
		{
			(*m_pProvider)++;
			Invalidate();
			SendMessage(m_pParent->m_pParent, ToWord('u', 'p'), 0);
		}
		if ( nKey & BIOS::KEY::KeyEnter  )
		{
			// is that provider enumerator ?
			if ( m_pProvider->Get() != CValueProvider::Invalid )
			{
				SendMessage(m_pParent->m_pParent, ToWord('l', 'e'), (ui32)(NATIVEPTR)(m_pProvider));
			}
		}
		CListItem::OnKey( nKey );
	}

};


// Combo

class CComboSelector : public CWnd
{
	ui16	m_clrFrame;
	CValueProvider*	m_pProvider;
	ui32	m_nOldValue;

public:

	void Create( const char* pszId, ui16 dwFlags, const CRect& rc, ui16 clr, CValueProvider* pProvider, CWnd* pParent )
	{
		m_clrFrame = clr;
		m_pProvider = pProvider;
		m_nOldValue = m_pProvider->Get();
		CWnd::Create( pszId, dwFlags, rc, pParent );
	}

	virtual void OnPaint()
	{
		CRect rcClient = m_rcClient;
		CDesign::WindowSelector(m_rcClient, m_clrFrame);

		BIOS::LCD::Print( rcClient.CenterX()-((ui8)strlen(m_pszId)<<2), rcClient.top+2, 
			RGB565(000000), RGBTRANS, m_pszId);
		BIOS::LCD::Draw( m_rcClient.left + 6, m_rcClient.top+20, RGB565(000000), RGBTRANS, more_left );
		BIOS::LCD::Draw( m_rcClient.right - 3 - 6, m_rcClient.top+20, RGB565(000000), RGBTRANS, more_right );

		rcClient.Deflate(2+12+4, 2+14+2, 2+12+4, 2+2+0);

		/*
		BIOS::RoundRect(rcClient, RGB565(000000));
		rcClient.Deflate(2, 2, 2, 2);

		BIOS::RoundRect(rcClient, m_clrFrame);

		BIOS::Print( rcClient.CenterX()-((ui8)strlen(m_pszId)<<2), rcClient.top, 
			RGB565(000000), RGBTRANS, m_pszId);

		BIOS::Draw( m_rcClient.left + 4, m_rcClient.top+18, RGB565(000000), RGBTRANS, more_left );
		BIOS::Draw( m_rcClient.right - 3 - 8, m_rcClient.top+18, RGB565(000000), RGBTRANS, more_right );

		rcClient.Deflate(12, 14, 12, 2);
		BIOS::RoundRect(rcClient, RGB565(b0b0b0));
		rcClient.Deflate(4, 2, 4, 0);
		*/

		ui32 old = m_pProvider->Get();

		// vypocitaj s ktorym prvym itemom musime zacat aby bol focus viditelny
		ui16 nFocusBase = 0;
		m_pProvider->Set(0);
		while ( m_pProvider->Get() != old )
		{
			nFocusBase += m_pProvider->GetWidth();
			nFocusBase += 8;
			_ASSERT( (*m_pProvider)+1 == CValueProvider::Yes );
			(*m_pProvider)++;
		}
		
		// zmesti sa nam to bez scrollovania ?
		nFocusBase += m_pProvider->GetWidth() + 8;

		m_pProvider->Set(0); 

		while ( nFocusBase > rcClient.Width() )
		{
			nFocusBase -= 8;
			nFocusBase -= m_pProvider->GetWidth();
			_ASSERT( (*m_pProvider)+1 == CValueProvider::Yes );
			(*m_pProvider)++;
		}

		// Zacni kreslit
		//m_pProvider->Set(0);
		int nWidth;
		while ( (nWidth = m_pProvider->GetWidth()) < rcClient.Width() )
		{
			ui8 bFocus = m_pProvider->Get() == old;
			if ( bFocus )
			{
				CRect rcBack = rcClient;
				rcBack.right = rcBack.left + nWidth;
				rcBack.bottom = rcBack.top + 14;
				BIOS::LCD::Bar( rcBack, RGB565(000000) );
				BIOS::LCD::Draw( rcClient.left-8, rcClient.top, RGB565(000000), RGBTRANS, sel_left );
			}
				
			m_pProvider->OnPaint( rcClient, bFocus );
			rcClient.left += nWidth;

			if ( bFocus )
				BIOS::LCD::Draw( rcClient.left, rcClient.top, RGB565(000000), RGBTRANS, sel_right );

			rcClient.left += 8;

			if ( (*m_pProvider)+1 != CValueProvider::Yes )
				break;
			(*m_pProvider)++;
		}
		m_pProvider->Set(old);
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & BIOS::KEY::KeyLeft && (*m_pProvider)-1 != CValueProvider::No )
		{
			(*m_pProvider)--;
			Invalidate();
		}
		if ( nKey & BIOS::KEY::KeyRight && (*m_pProvider)+1 != CValueProvider::No )
		{
			(*m_pProvider)++;
			Invalidate();
		}
		if ( nKey & BIOS::KEY::KeyEnter  )
		{
			SendMessage(m_pParent, ToWord('o', 'k'), 0);
		}
		if ( nKey & BIOS::KEY::KeyEscape  )
		{
			m_pProvider->Set(m_nOldValue);
			SendMessage(m_pParent, ToWord('e', 'x'), 0);
		}
	}

};
		

class CItemProvider : public CWndMenuItem
{
	CValueProvider*	m_pProvider;

public:
	virtual void Create(CValueProvider*	pProvider, const char* pszId, ui16 clr, CWnd *pParent) 
	{
		_ASSERT( pProvider );
		m_pProvider = pProvider;
		CWndMenuItem::Create( pszId, clr, 2, pParent);
	}

	virtual void OnPaint()
	{
		const char *pszTemp = m_pszId;
		m_pszId = NULL;
		CWndMenuItem::OnPaint();
		m_pszId = pszTemp;

		CRect rcItem = m_rcClient;
		rcItem.Deflate( 12+MarginLeft, 0, 0, 0 );
		BIOS::LCD::Print( rcItem.left, rcItem.top, RGB565(000000), RGBTRANS, m_pszId );
		rcItem.top += 16;
		if ( HasFocus() )
		{
			rcItem.left -= 8;
			rcItem.left += BIOS::LCD::Draw(rcItem.left, rcItem.top, RGB565(0000000), RGBTRANS, sel_left);
			
			ui16 nWidth = m_pProvider->GetWidth();
			BIOS::LCD::Bar( rcItem.left, rcItem.top, rcItem.left + nWidth, rcItem.top + 14, RGB565(000000));
			
			m_pProvider->OnPaint( rcItem, 1 );
			rcItem.left += nWidth;
			rcItem.left += BIOS::LCD::Draw(rcItem.left, rcItem.top, RGB565(0000000), RGBTRANS, sel_right);

		} else
		{
			m_pProvider->OnPaint( rcItem, 0 );
		}
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & BIOS::KEY::KeyLeft && *m_pProvider - 1 == CValueProvider::Yes )
		{
			(*m_pProvider)--;
			Invalidate();
			SendMessage(m_pParent, ToWord('i', 'u'), NULL);
		}
		if ( nKey & BIOS::KEY::KeyRight && *m_pProvider + 1 == CValueProvider::Yes )
		{
			(*m_pProvider)++;
			Invalidate();
			SendMessage(m_pParent, ToWord('i', 'u'), NULL);
		}
		CWnd::OnKey( nKey );
	}
};

#endif