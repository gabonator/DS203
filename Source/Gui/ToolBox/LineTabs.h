class CLTabs : public CListItem
{
	ui16 m_clrBack;
	const char**	m_arrItems;
	int				m_nFocus;

public:
	void Create(const char** arrItems, ui16 clrBack, CWnd* pParent)
	{
		m_nFocus = 0;
		m_clrBack = clrBack;
		m_arrItems = arrItems;
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
			rcRect.top += 16;
			rcRect.bottom = rcRect.top + nHeight;
			rcRect.Deflate(4, 0, 4, 0);
		}
		CWnd::Create( "CLTabs", CWnd::WsVisible, rcRect, pParent );
	}

	virtual void OnPaint()
	{
		BIOS::LCD::Bar( m_rcClient, m_clrBack );
		int x = m_rcClient.left;
		int y = m_rcClient.top;
		for ( int i=0; m_arrItems[i]; i++ )
		{
			u16 clrTab = i==m_nFocus ? RGB565(b0b0b0) : m_clrBack;
			u16 clrLabel = RGB565(ffffff);
			if ( HasFocus() && (i==m_nFocus))
			{
				clrLabel = RGB565(000000);
				clrTab = RGB565(ffffff);
			}
			x += BIOS::LCD::Draw( x, y, clrTab, m_clrBack, CShapes::corner_left);
			int _x = x;
			x += BIOS::LCD::Print( x, y, clrLabel, clrTab, m_arrItems[i]);
			BIOS::LCD::Bar( _x, y+14, x, y+16, clrTab);
			x += BIOS::LCD::Draw( x, y, clrTab, m_clrBack, CShapes::corner_right);
		}
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & BIOS::KEY::KeyLeft && m_nFocus > 0 )
		{
			m_nFocus--;
			Invalidate();
			SendMessage(m_pParent, ToWord('u', 'p'), 0);
		}
		if ( nKey & BIOS::KEY::KeyRight && m_arrItems[m_nFocus+1]  )
		{
			m_nFocus++;
			Invalidate();
			SendMessage(m_pParent, ToWord('u', 'p'), 0);
		}

		CListItem::OnKey( nKey );
	}

	const int& GetFocus()
	{
		return m_nFocus;
	}
};

