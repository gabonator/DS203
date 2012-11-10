#ifndef __MENUITESPECDISPLAY_H__
#define __MENUITESPECDISPLAY_H__

class CItemSpecDisplay : public CMPItem
{
	CProviderEnum	m_proDisplay;

public:
	virtual void Create(CWnd *pParent) 
	{
		CWndMenuItem::Create( NULL, RGB565(8080b0), 2, pParent);

		m_proDisplay.Create( (const char**)CSettings::Spectrum::ppszTextDisplay,
			(NATIVEENUM*)&Settings.Spec.Display, CSettings::Spectrum::_DisplayMax );
		m_pProvider = &m_proDisplay;
	}

	virtual void OnPaint()
	{
		ui16 clr = RGB565(000000);

		CWndMenuItem::OnPaint();
		int x = m_rcClient.left + 12 + MarginLeft;
		int y = m_rcClient.top;
		int _x = x;
		if ( HasFocus() )
		{
			x -= 8;
			x += BIOS::LCD::Draw(x, y, RGB565(000000), RGBTRANS, sel_left);
			CRect rcRect(x, y, x + m_pProvider->GetWidth(), y + 14);
			BIOS::LCD::Bar( rcRect, RGB565(0000000) );
			m_pProvider->OnPaint( rcRect, HasFocus() );
			x = rcRect.right;
			x += BIOS::LCD::Draw(x, y, RGB565(000000), RGBTRANS, sel_right);
		} else
		{
			CRect rcRect(x, y, x + m_pProvider->GetWidth(), y + 14);
			m_pProvider->OnPaint( rcRect, HasFocus() );
		}

		x = _x;
		y += 16;
		BIOS::LCD::Print( x, y, clr, RGBTRANS, 
			CSettings::Spectrum::ppszTextScale[Settings.Spec.YScale] );
	}
};

#endif

