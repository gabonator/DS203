#ifndef __ITEMGENDUTY_H__
#define __ITEMGENDUTY_H__

class CItemDuty : public CWndMenuItem
{
public:
	virtual void Create(const char* pszId, ui16 clr, ui8 rows, CWnd *pParent) 
	{
		CWndMenuItem::Create(pszId, clr, rows, pParent);
	}

	virtual void OnKey(ui16 nKey)
	{
		// Settings.Gen.nArr : Settings.Gen.nCcr
		int nDuty = CCoreGenerator::GetDuty();

		if ( nKey & BIOS::KEY::KeyLeft && nDuty > 0 )
		{
			nDuty -= 5;
			UTILS.Clamp<int>( nDuty, 0, 100 );
			Settings.Gen.nDuty = nDuty;
			CCoreGenerator::SetDuty( nDuty );
			if ( CCoreGenerator::GetDuty() != nDuty )
				Settings.Gen.nCcr--;
			
			UTILS.Clamp<int>( Settings.Gen.nCcr, 0, Settings.Gen.nArr );
			Invalidate();
			SendMessage(m_pParent, ToWord('i', 'u'), NULL);
		}
		if ( nKey & BIOS::KEY::KeyRight && nDuty < 100 )
		{
			nDuty += 5;
			UTILS.Clamp<int>( nDuty, 0, 100 );
			Settings.Gen.nDuty = nDuty;
			CCoreGenerator::SetDuty( nDuty );
			if ( CCoreGenerator::GetDuty() != nDuty )
				Settings.Gen.nCcr++;

			UTILS.Clamp<int>( Settings.Gen.nCcr, 0, Settings.Gen.nArr );
			Invalidate();
			SendMessage(m_pParent, ToWord('i', 'u'), NULL);
		}
		CWndMenuItem::OnKey( nKey );
	}

	virtual void OnPaint()
	{
		CWndMenuItem::OnPaint();
		int nDuty = CCoreGenerator::GetDuty();
		BIOS::LCD::Printf( m_rcClient.left + 12 + 16, m_rcClient.top + 16, RGB565(000000), RGBTRANS, "%d%%", 
			nDuty );
	}
};

#endif