class CMIProgress : public CWndMenuItem
{
public:
	virtual void Create(const char* pszId, ui16 clr, ui8 rows, CWnd *pParent) 
	{
		CWndMenuItem::Create(pszId, clr, rows, pParent);
	}

	virtual int GetValue() = 0;
	virtual void SetValue(int) = 0;
	virtual int GetMin() = 0;
	virtual int GetMax() = 0;

	virtual void OnKey(ui16 nKey)
	{
		int nValue = GetValue();
		if ( nKey & BIOS::KEY::KeyLeft && nValue > GetMin() )
		{
			SetValue( nValue - 1 );
			RedrawProgress();
		}
		if ( nKey & BIOS::KEY::KeyRight && nValue < GetMax() )
		{
			SetValue( nValue + 1 );
			RedrawProgress();
		}
		CWndMenuItem::OnKey( nKey );
	}

	virtual void OnPaint()
	{
		CWndMenuItem::OnPaint();
		RedrawProgress();
	}

	void RedrawProgress()
	{
		ui16 clrBack = RGB565(404040);
		ui16 clrFront = HasFocus() ? RGB565(ffffff) : RGB565(b0b0b0);
		CRect rcBar( m_rcClient.left + 18, m_rcClient.bottom - 12, m_rcClient.right - 8, m_rcClient.bottom - 4 );
		BIOS::LCD::Bar( rcBar, clrBack );
		rcBar.Deflate(2, 2, 2, 2);
		int nWidth = 16;
		int nPos = ( rcBar.Width() - nWidth ) * GetValue() / ( GetMax() - GetMin() );
		rcBar.left += nPos;
		rcBar.right = rcBar.left + nWidth;
		BIOS::LCD::Bar( rcBar, clrFront );
	}
};
