#ifndef __MENUTRIGGER_H__
#define __MENUTRIGGER_H__

class CItemTrigger : public CWndMenuItem
{
	CProviderNum	m_proLevel;
	int						m_nTimer;
public:
	virtual void Create(CWnd *pParent) 
	{
		CWndMenuItem::Create( "Trig", RGB565(404040), 2, pParent);

		m_proLevel.Create( &Settings.Trig.nLevel, 0, 255 );
		SetTimer(500);
		m_nTimer = 0;
	}

	virtual void OnPaint()
	{
		ui16 clr = RGB565(000000);
		ui16 clrSource = RGB565(ff00ff);

		switch ( Settings.Trig.Source )
		{
			case CSettings::Trigger::_CH1: 
				clrSource = Settings.CH1.u16Color; 
				break;
			case CSettings::Trigger::_CH2: 
				clrSource = Settings.CH2.u16Color; 
				break;
			case CSettings::Trigger::_CH3: 
				clrSource = Settings.CH3.u16Color; 
				break;
			case CSettings::Trigger::_CH4: 
				clrSource = Settings.CH4.u16Color; 
				break;
			case CSettings::Trigger::_Math: 
				_ASSERT(0);
		}


		CWndMenuItem::OnPaint();
		int x = m_rcClient.left + 12 + MarginLeft;
		int y = m_rcClient.top;
		BIOS::LCD::Print( x, y, clr, RGBTRANS, m_pszId );
		x += 52;

		if ( Settings.Trig.Type == CSettings::Trigger::_EdgeLH )
		{
			BIOS::LCD::Draw( x, y, RGB565(000000), RGBTRANS, trig_pos_ );
			x += BIOS::LCD::Draw( x, y, clrSource, RGBTRANS, trig_pos );
		}
		else
		{
			BIOS::LCD::Draw( x, y, RGB565(000000), RGBTRANS, trig_neg_ );
			x += BIOS::LCD::Draw( x, y, clrSource, RGBTRANS, trig_neg );
		}
		
		x = m_rcClient.left + 12 + MarginLeft;
		y += 16;
		if ( HasFocus() )
		{
			x += BIOS::LCD::Draw(x, y+1, clr, RGBTRANS, sel_left);
			x += BIOS::LCD::Draw(x, y+1, RGBTRANS, clr, updown);
			x += BIOS::LCD::Draw(x, y+1, clr, RGBTRANS, sel_right);

			x = m_rcClient.left + 12 + MarginLeft;
		} else
		{
			BIOS::LCD::Print( x, y, clr, RGBTRANS, CSettings::Trigger::ppszTextSync[Settings.Trig.Sync]);
		}

		x += 52;

		if ( !BIOS::ADC::Enabled() )
		{
			x += BIOS::LCD::Draw( x, y, RGB565(ff0000), RGBTRANS, trig_stop);
		} else
		{
			switch ( Settings.Trig.State )
			{
			case CSettings::Trigger::_Run:
				x += BIOS::LCD::Draw( x, y, RGB565(00b000), RGBTRANS, trig_run);
				break;
			case CSettings::Trigger::_Stop:
				x += BIOS::LCD::Draw( x, y, RGB565(ff0000), RGBTRANS, trig_stop);
				break;
			case CSettings::Trigger::_Wait:
				x += BIOS::LCD::Print( x, y, RGB565(ff0000), RGBTRANS, "W");
				break;
			case CSettings::Trigger::_Unsync:
				_ASSERT(0);
			}
		}
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & BIOS::KEY::KeyLeft && m_proLevel-1 == CValueProvider::Yes )
		{
			m_proLevel--;
			SendMessage(m_pParent, ToWord('r', 'u'), 0);
		}
		if ( nKey & BIOS::KEY::KeyRight && m_proLevel+1 == CValueProvider::Yes )
		{
			m_proLevel++;
			SendMessage(m_pParent, ToWord('r', 'u'), 0);
		}
		if ( nKey & BIOS::KEY::KeyEnter )
		{
			SendMessage(m_pParent, ToWord('m', 'r'), 0);
		}

		CWnd::OnKey( nKey );
	}

	virtual void OnTimer()
	{
		if ( IsVisible() && 
			BIOS::ADC::Enabled() && Settings.Trig.State == CSettings::Trigger::_Wait )
		{
			int x = m_rcClient.left + 12 + MarginLeft;
			int y = m_rcClient.top;
			x += 52;
			y += 16;
			if ( m_nTimer++ & 1 )
				BIOS::LCD::Print( x, y, RGB565(b00000), RGBTRANS, "W");
			else
				BIOS::LCD::Print( x, y, RGB565(00b000), RGBTRANS, "W");
		}
	}
};

#endif