class CMouse
{
	ui16 m_pixels[4];
	int m_nX;
	int m_nY;

	int m_nLastX;
	int m_nLastY;

	bool m_bDown;
	bool m_bClick;
	bool m_bOverride;

public:
	CMouse()
	{
		m_nX = BIOS::LCD::LcdWidth / 2;
		m_nY = BIOS::LCD::LcdHeight / 2;
		m_nLastX = -1;
		m_nLastY = -1;
		m_bDown = false;
		m_bClick = false;
		m_bOverride = false;
	}

	void Override( int x, int y, int b )
	{
		m_bOverride = true;
		m_nX = x;
		m_nY = y;

		if ( m_bDown == false && b )
			m_bClick = true;
		m_bDown = b ? true : false;
	}

	void Update()
	{
		if ( m_bOverride )
			return;

		m_nX = BIOS::MOUSE::GetX();
		m_nY = BIOS::MOUSE::GetY();

		if ( m_bDown == false && BIOS::MOUSE::GetDown() )
			m_bClick = true;
		m_bDown = BIOS::MOUSE::GetDown() ? true : false;
	}

	int GetX()
	{
		return m_nX;
	}

	int GetY()
	{
		return m_nY;
	}

	void Show()
	{
		if ( !m_bOverride && !BIOS::MOUSE::IsSupported() )
			return;

		if ( m_nLastX != -1 )
			Hide();
		Update();

		_ASSERT( m_nLastX == -1 );
		m_nLastX = -1;
		if ( m_nX < 0 || m_nX >= BIOS::LCD::LcdWidth-1 )
			return;
		if ( m_nY < 0 || m_nY >= BIOS::LCD::LcdHeight-1 )
			return;

		m_nLastX = m_nX;
		m_nLastY = m_nY;

		ui16* pPix = m_pixels;
		for (int y=m_nY; y<m_nY+2; y++)
			for (int x=m_nX; x<m_nX+2; x++)
			{
				ui16 nPix = BIOS::LCD::GetPixel(x, y);
				BIOS::LCD::PutPixel(x, y, ~nPix);
				*pPix++ = nPix;
			}
	}

	void Hide()
	{
		if ( m_nLastX == -1 )
			return;

		ui16* pPix = m_pixels;
		for (int y=m_nLastY; y<m_nLastY+2; y++)
			for (int x=m_nLastX; x<m_nLastX+2; x++)
				BIOS::LCD::PutPixel(x, y, *pPix++);

		m_nLastX = -1;
	}

	bool Clicked()
	{
		if (!m_bClick)
			return false;
		m_bClick = false;
		return true;
	}
};