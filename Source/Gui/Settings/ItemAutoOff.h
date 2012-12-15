class CProviderAutoOff : public CValueProvider
{
	enum {
		Invalid = -999
	};
	int m_nMin;
	int m_nMax;

public:
	int& GetValue()
	{
		return Settings.Runtime.m_nStandby;
	}

	void Create(int nMin, int nMax)
	{
		m_nMin = nMin;
		m_nMax = nMax;
	}

	virtual VPNavigate operator +(si8 d)
	{
		int nNewValue = GetValue()+d;
		return ( nNewValue >= m_nMin && nNewValue <= m_nMax ) ? Yes : No; 
	}

	virtual void operator++(int)
	{
		GetValue()++;
	}

	virtual void operator--(int)
	{
		GetValue()--;
	}

	const char* GetLabel()
	{
		if ( GetValue() == 0 )
			return "Never";

		char* pText = CUtils::itoa( GetValue() );
		strcat(pText, " mins");
		return pText; 
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
};

class CMIAutoOff : public CItemProvider
{
	CProviderAutoOff m_proBeepOnOff;

public:
	void Create( PCSTR strName, ui16 clr, int nRows, CWnd* pParent )
	{
		m_proBeepOnOff.Create( 0, 15 );
		CItemProvider::Create( &m_proBeepOnOff, strName, clr, pParent );
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( ( nKey & BIOS::KEY::KeyLeft ) || ( nKey & BIOS::KEY::KeyRight ) )
		{
			CItemProvider::OnKey( nKey );
			Settings.Kick();
			return;
		}
		CItemProvider::OnKey( nKey );
	}
};
