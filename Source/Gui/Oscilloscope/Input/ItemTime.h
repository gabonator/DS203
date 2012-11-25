#ifndef __MENUTIME_H__
#define __MENUTIME_H__

class CItemTime : public CWndMenuItem
{
	CSettings::TimeBase* m_pInfo;
	CProviderEnum	m_proResolution;

public:
	virtual void Create(CSettings::TimeBase* pInfo, CWnd *pParent) 
	{
		_ASSERT( pInfo );
		m_pInfo = pInfo;
		CWndMenuItem::Create( "Timebase", RGB565(ffffff), 2, pParent);

		m_proResolution.Create( (const char**)CSettings::TimeBase::ppszTextResolution,
			(NATIVEENUM*)&pInfo->Resolution, CSettings::TimeBase::_ResolutionMax );
	}

	virtual void OnPaint()
	{
		ui16 clr = RGB565(000000);

		CWndMenuItem::OnPaint();
		int x = m_rcClient.left + 12 + MarginLeft;
		int y = m_rcClient.top;
		BIOS::LCD::Print( x, y, clr, RGBTRANS, m_pszId );
		y += 16;
		if ( HasFocus() )
		{
			x -= 8;
			x += BIOS::LCD::Draw(x, y, clr, RGBTRANS, CShapes::sel_left);
			x += BIOS::LCD::Print( x, y, RGBTRANS, clr, CSettings::TimeBase::ppszTextResolution[m_pInfo->Resolution]);
			x += BIOS::LCD::Draw(x, y, clr, RGBTRANS, CShapes::sel_right);
		} else
		{
			x += BIOS::LCD::Print( x, y, clr, RGBTRANS, CSettings::TimeBase::ppszTextResolution[m_pInfo->Resolution]);
		}
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & BIOS::KEY::KeyLeft && m_proResolution-1 == CValueProvider::Yes )
		{
			m_proResolution--;
			Invalidate();
			SendMessage(m_pParent, ToWord('i', 'u'), (ui32)(NATIVEPTR)m_pInfo);
		}
		if ( nKey & BIOS::KEY::KeyRight && m_proResolution+1 == CValueProvider::Yes )
		{
			m_proResolution++;
			Invalidate();
			SendMessage(m_pParent, ToWord('i', 'u'), (ui32)(NATIVEPTR)m_pInfo);
		}
		if ( nKey & BIOS::KEY::KeyEnter )
		{
			SendMessage(m_pParent, ToWord('m', 't'), (ui32)(NATIVEPTR)&m_proResolution);
		}
		CWnd::OnKey( nKey );
	}
};

#endif