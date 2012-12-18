#ifndef __ITEMSHORTCUT_H__
#define __ITEMSHORTCUT_H__

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

	const char* GetLabelA()
	{
		const CWndToolBar::CBarItem* pItems = CWndToolBar::GetMenuItems();
		int nVal = *m_pVal;
		if ( nVal < 0 )
			return CSettings::CRuntime::ppszTextShortcut[-nVal-1];

		for (int i=nVal; i>=0; i--)
			if ( pItems[i].m_eType == CWndToolBar::CBarItem::IMain )
				return pItems[i].m_pName;

		return NULL;
	}

	const char* GetLabelB()
	{
		int nVal = *m_pVal;
		if ( nVal >= 0 )
			return CWndToolBar::GetMenuItems()[nVal].m_pName;
		return NULL;
	}

	virtual void OnPaint(const CRect& rcRect, ui8 bFocus)
	{
		ui16 clr = bFocus ? RGB565(ffffff) : RGB565(000000);
		ui16 x = rcRect.left;

		const char* strA = GetLabelA();
		const char* strB = GetLabelB();
		if ( strA )
			x += BIOS::LCD::Print( x, rcRect.top, clr, RGBTRANS, strA );
		if ( strB )
		{
			x += 4;
			x += BIOS::LCD::Print( x, rcRect.top, clr, RGBTRANS, "\x1a" );
			x += 4;
			x += BIOS::LCD::Print( x, rcRect.top, clr, RGBTRANS, strB );
		}

	}	

	virtual ui16 GetWidth()
	{
		const char* strA = GetLabelA();
		const char* strB = GetLabelB();
		int nWidth = strA ? strlen(strA) : 0;
		nWidth += strB ? strlen(strB) : 0;
		if ( strB )
			nWidth += 2;
		return nWidth*8;
	}

	int FindNext(int nSearch, int nDir)
	{
		const CWndToolBar::CBarItem* pItems = CWndToolBar::GetMenuItems();
		if ( /*nSearch < 0 || */ nSearch + nDir < 0 )
		{
			if ( nSearch + nDir == 0 )
			{
				// skip first sub menu
				if ( nDir > 0 )
					return 1;
				if ( nDir < 0 )
					return -1;
			}
			if ( nSearch + nDir >= CSettings::CRuntime::_ShortcutMax )
			{
				return nSearch + nDir;
			}
			return Invalid;
		}
		if ( nDir > 0 )
		{
			//if ( pItems[nSearch+1].m_eType != CWndToolBar::CBarItem::IEnd )
			for ( nSearch++; pItems[nSearch].m_eType != CWndToolBar::CBarItem::IEnd; nSearch++ )
				if ( pItems[nSearch].m_eType != CWndToolBar::CBarItem::IMain )
					return nSearch;

		} else {
			for ( nSearch--; nSearch >= 0; nSearch-- )
				if ( pItems[nSearch].m_eType != CWndToolBar::CBarItem::IMain )
					return nSearch;
			return -1;
		}
		return Invalid;
 	}
};

class CItemShortcut : public CItemProvider
{
public:
	void Create( CWnd* pParent, CValueProvider*	pProvider, const char* pszId)
	{
		CItemProvider::Create( pProvider, pszId, RGB565(808080), pParent );
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( ( nKey & BIOS::KEY::KeyLeft ) || ( nKey & BIOS::KEY::KeyRight ) )
        {
            CItemProvider::OnKey( nKey ); // this will update the variable
            Settings.Kick(); 
            return;
        }
		CItemProvider::OnKey( nKey );
	}
};

#endif