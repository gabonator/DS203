#ifndef __MENUANALOG_H__
#define __MENUANALOG_H__

class CItemAnalog : public CWndMenuItem
{
	CSettings::AnalogChannel* m_pInfo;

public:
	virtual void Create(CSettings::AnalogChannel* pInfo, CWnd *pParent) 
	{
		_ASSERT( pInfo );
		m_pInfo = pInfo;
		CWndMenuItem::Create( NULL, pInfo->u16Color, 2, pParent);
		CWndMenuItem::SetColorPtr( &pInfo->u16Color );
	}

	virtual void OnPaint()
	{
		ui16 clr = ( m_pInfo->Enabled == 
			CSettings::AnalogChannel::_YES ) ? RGB565(000000) : RGB565(808080);

		CWndMenuItem::OnPaint();
		int x = m_rcClient.left + 12 + MarginLeft;
		int y = m_rcClient.top;
		x += BIOS::LCD::Print( x, y, clr, RGBTRANS, 
			m_pInfo->pszName );
		x += 8;
		
		if ( m_pInfo->Coupling == CSettings::AnalogChannel::_DC )
			x += BIOS::LCD::Draw( x, y, clr, RGBTRANS, CShapes::sig_dc );

		if ( m_pInfo->Coupling == CSettings::AnalogChannel::_AC )
			x += BIOS::LCD::Draw( x, y, clr, RGBTRANS, CShapes::sig_ac );

		if ( m_pInfo->Coupling == CSettings::AnalogChannel::_GND )
			x += BIOS::LCD::Draw( x, y, clr, RGBTRANS, CShapes::sig_gnd );

		x = m_rcClient.left + 12 + MarginLeft;
		y += 16;
		if ( !HasFocus() )
		{
			x += BIOS::LCD::Print( x, y, clr, RGBTRANS, 
				CSettings::AnalogChannel::ppszTextResolution[ m_pInfo->Resolution ] );
			x += BIOS::LCD::Draw(x, y, clr, RGBTRANS, CShapes::per_div);
		} else
		{
			x -= 8;
			x += BIOS::LCD::Draw(x, y, clr, RGBTRANS, CShapes::sel_left);
			x += BIOS::LCD::Print(x, y, RGB565(ffffff), clr, 
				CSettings::AnalogChannel::ppszTextResolution[ m_pInfo->Resolution ] );
			x += BIOS::LCD::Draw(x, y, clr, RGBTRANS, CShapes::sel_right);
			x += BIOS::LCD::Draw(x, y, clr, RGBTRANS, CShapes::per_div);
		}
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & BIOS::KEY::KeyLeft && m_pInfo->Resolution > 0 )
		{
			_ASSERT(sizeof(m_pInfo->Resolution) == sizeof(NATIVEENUM));
			DecEnum(m_pInfo->Resolution);
			Invalidate();
			SendMessage(m_pParent, ToWord('i', 'u'), (ui32)(NATIVEPTR)m_pInfo);
		}
		if ( nKey & BIOS::KEY::KeyRight && m_pInfo->Resolution < CSettings::AnalogChannel::_ResolutionMax )
		{
			_ASSERT(sizeof(m_pInfo->Resolution) == sizeof(NATIVEENUM));
			IncEnum(m_pInfo->Resolution);
			Invalidate();
			SendMessage(m_pParent, ToWord('i', 'u'), (ui32)(NATIVEPTR)m_pInfo);
		}
		if ( nKey & BIOS::KEY::KeyEnter )
		{
			SendMessage(m_pParent, ToWord('m', 'a'), (ui32)(NATIVEPTR)m_pInfo);			
		}
		CWnd::OnKey( nKey );
	}
};

#endif