#include "ItemBeepOnOff.h"
#include "ItemShortcut.h"
#include "Source\Gui\Toolbar.h"

class CProviderShortcut : public CValueProvider
{
	enum {
		Invalid = -999
	};
	int* m_pVal;

public:
	void Create(int* pVal)
	{
		m_pVal = pVal;
	}

	virtual VPNavigate operator +(si8 d)
	{
		return FindNext(*m_pVal, d) != Invalid ? Yes : No; 
	}

	virtual void operator++(int)
	{
		*m_pVal = FindNext(*m_pVal, +1);
	}

	virtual void operator--(int)
	{
		*m_pVal = FindNext(*m_pVal, -1);
	}

	const char* GetLabel()
	{
		int nVal = *m_pVal;
		if ( nVal >= 0 )
			return CWndToolBar::GetMenuItems()[nVal].m_pName;
		return CSettings::CRuntime::ppszTextShortcut[-nVal-1];
	}

	virtual void OnPaint(const CRect& rcRect, ui8 bFocus)
	{
		ui16 clr = bFocus ? RGB565(ffffff) : RGB565(000000);
		ui16 x = rcRect.left;
		x += BIOS::LCD::Print( x, rcRect.top, clr, RGBTRANS, 
			GetLabel() );
	}	

	virtual ui16 GetWidth()
	{
		return (ui16)strlen(GetLabel()) << 3;
	}

	int FindNext(int nSearch, int nDir)
	{
		const CWndToolBar::CBarItem* pItems = CWndToolBar::GetMenuItems();
		if ( nSearch < 0 || ( nSearch == 0 && nDir < 0 ) )
		{
			if ( nSearch+nDir >= CSettings::CRuntime::_ShortcutMax )
				return nSearch + nDir;
			return Invalid;
		}
		if ( nDir > 0 )
		{
			for ( nSearch++; pItems[nSearch].m_eType != CWndToolBar::CBarItem::IEnd; nSearch++ )
				if ( pItems[nSearch].m_eType == CWndToolBar::CBarItem::IMain )
					return nSearch;

		} else {
			for ( nSearch--; nSearch >= 0; nSearch-- )
				if ( pItems[nSearch].m_eType == CWndToolBar::CBarItem::IMain )
					return nSearch;
		}
		return Invalid;
 	}
};

class CWndMenuKeySettings : public CWnd
{
public:
	CItemBeepOnOff		m_itmBeepOnOff;
	CProviderShortcut	m_proShortcutCircle;
	CItemShortcut		m_itmShortcutCircle;
	CProviderShortcut	m_proShortcutTriangle;
	CItemShortcut		m_itmShortcutTriangle;
	CProviderShortcut	m_proShortcutS1;
	CItemShortcut		m_itmShortcutS1;
	CProviderShortcut	m_proShortcutS2;
	CItemShortcut		m_itmShortcutS2;

	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndMenuKeySettings", dwFlags, CRect(60, 40, 280, 240), pParent);

		m_itmBeepOnOff.Create(this);

		m_proShortcutCircle.Create( &Settings.Runtime.m_nShortcutCircle );
		m_itmShortcutCircle.Create( this, &m_proShortcutCircle, "Circle Button" );

		m_proShortcutTriangle.Create( &Settings.Runtime.m_nShortcutTriangle );
		m_itmShortcutTriangle.Create( this, &m_proShortcutTriangle, "Triangle Button" );

		m_proShortcutS1.Create( &Settings.Runtime.m_nShortcutS1 );
		m_itmShortcutS1.Create( this, &m_proShortcutS1, "S1 Button" );

		m_proShortcutS2.Create( &Settings.Runtime.m_nShortcutS2 );
		m_itmShortcutS2.Create( this, &m_proShortcutS2, "S2 Button" );
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		//if ( code == ToWord('m', 'o') && pSender == &m_itmReboot )
		//	BIOS::SYS::Execute( BIOS::SYS::EDfu );
	}
};
