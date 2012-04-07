
class CWndLevelReferences : public CWnd
{
public:
	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndLevelReferences", dwFlags | CWnd::WsNoActivate, 
			CRect(0, 22, 11, 22+CWndGraph::DivsY*CWndGraph::BlkY), pParent);
	}

	virtual void OnPaint()
	{
		BIOS::LCD::Bar( m_rcClient, RGB565(000000) );
		//if ( Settings.Trig.Sync != CSettings::Trigger::_None )                                           
		{
			ui16 y = Settings.Trig.nLevel;
			y = (y * (CWndGraph::DivsY*CWndGraph::BlkY)) >> 8;
			BIOS::LCD::Draw( m_rcClient.left, m_rcClient.bottom - y-5, 
				RGB565(606060), RGBTRANS, trig_base );
		}
		if ( Settings.CH2.Enabled )
		{
			ui16 y = Settings.CH2.u16Position;

			y = CSettings::GetZero(y);
			y = (y * (CWndGraph::DivsY*CWndGraph::BlkY)) >> 8;

			BIOS::LCD::Draw( m_rcClient.left, m_rcClient.bottom - y-5, 
				Settings.CH2.u16Color, RGBTRANS, chb_base );
		}
		if ( Settings.CH1.Enabled )
		{
			si16 y = Settings.CH1.u16Position;

			y = CSettings::GetZero(y);
			y = (y * (CWndGraph::DivsY*CWndGraph::BlkY)) >> 8;

			BIOS::LCD::Draw( m_rcClient.left, m_rcClient.bottom - y-5, 
				Settings.CH1.u16Color, RGBTRANS, cha_base );
		}
	}
};


class CWndTimeReferences : public CWnd
{
public:
	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndTimeReferences", dwFlags | CWnd::WsNoActivate, 
			CRect(12, 16, 12+CWndGraph::DivsX*CWndGraph::BlkX, 21), pParent);
	}

	virtual void OnPaint()
	{
		BIOS::LCD::Bar( m_rcClient, RGB565(000000) );
		si16 x = (Settings.Trig.nTime-Settings.Time.Shift) >> 2;
		if ( x >= 0 && x < m_rcClient.Width() )
			BIOS::LCD::Draw( m_rcClient.left + x-4, m_rcClient.top, 
				RGB565(606060), RGBTRANS, trig_time );
	}
};
