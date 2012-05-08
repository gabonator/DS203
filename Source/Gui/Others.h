#ifndef __OTHERS_H__
#define __OTHERS_H__

#include <Source\Framework\Thread.h>

class CWndToolbox : public CWnd, public CThread
{
#pragma pack(push)
#pragma pack(2)
	struct BmpHdr 
	{
		ui16 wBfType;
		ui32 dwBfSize;
		ui16 wBfReserved1;
		ui16 wBfReserved2;
		ui32 dwBfOffset;

		ui32 dwBiSize;
		ui32 dwBiWidth;
		ui32 dwBiHeight;
		ui16 wBiPlanes;
		ui16 wBiBitDepth;
		ui32 dwBiCompression;
		ui32 dwBiSizeImage;
		ui32 dwBiXPels;
		ui32 dwBiYPels;

		ui32 dwBiClrUsed;
		ui32 dwBiClrImportant;
	};
#pragma pack(pop)

	enum {
		Width = 200,
		Height = 80
	};

public:
	bool m_bRunning;
	bool m_bFirst;
	int m_nFocus;
	
	CWndToolbox()
	{
		m_bRunning = false;
		m_nFocus = -1;
		m_bFirst = true;
	}

	void Create( CWnd* pParent )
	{
	  CWnd::Create("CWndToolbox", CWnd::WsHidden | CWnd::WsModal, 
			CRect( (BIOS::LCD::LcdWidth-Width)/2 ,
			(BIOS::LCD::LcdHeight-Height)/2,
			(BIOS::LCD::LcdWidth+Width)/2,
			(BIOS::LCD::LcdHeight+Height)/2 ), pParent);
	}
	virtual void OnPaint()
	{
		CRect rcClient(m_rcClient);
		if ( m_bFirst )
		{
			CDesign::Shadow(rcClient, 0x80ffffff); // aa rr gg bb
			rcClient.Deflate( 2, 2, 2, 2 );
			CDesign::Shadow(rcClient, 0x80ffffff); // aa rr gg bb
			m_bFirst = false;
		}

		if ( m_nFocus == -1 )
			m_nFocus = 0;

		#define FOC(n) (m_nFocus==n)?RGB565(ffffff):RGB565(808080)
		PrintBold( m_rcClient.left + 8, m_rcClient.top + 8, FOC(0), RGB565(000000), "\x10 Save bitmap");
		PrintBold( m_rcClient.left + 8, m_rcClient.top + 8 + 16, FOC(1), RGB565(000000), "\x10 Save waveform (TXT)");
		PrintBold( m_rcClient.left + 8, m_rcClient.top + 8 + 32, FOC(2), RGB565(000000), "\x10 Save waveform (SVG)");

		char str[32];
		BIOS::DBG::sprintf(str, "bat %d%%", BIOS::GetBattery());
		BIOS::LCD::Printf( m_rcClient.left + 8, m_rcClient.top + 8 + 48, RGB565(000000), RGBTRANS, "\x10 Select  \xfe Close");
		PrintBold( m_rcClient.left + 8 + 120, m_rcClient.top + 8, RGB565(ffff00), RGB565(000000), str);
		//BIOS::LCD::Printf( m_rcClient.left + 8 + 130, m_rcClient.top + 8, RGB565(000000), RGBTRANS, str);
	}

	void PrintBold( int x, int y, ui16 clrFront, ui16 clrBorder, PCSTR szLabel )
	{
		BIOS::LCD::Print( x-1, y  , clrBorder, RGBTRANS, szLabel );
		BIOS::LCD::Print( x  , y+1, clrBorder, RGBTRANS, szLabel );
		BIOS::LCD::Print( x+1, y  , clrBorder, RGBTRANS, szLabel );
		BIOS::LCD::Print( x  , y-1, clrBorder, RGBTRANS, szLabel );

		BIOS::LCD::Print( x-1, y-1, clrBorder, RGBTRANS, szLabel );
		BIOS::LCD::Print( x-1, y+1, clrBorder, RGBTRANS, szLabel );
		BIOS::LCD::Print( x+1, y-1, clrBorder, RGBTRANS, szLabel );
		BIOS::LCD::Print( x+1, y+1, clrBorder, RGBTRANS, szLabel );

		BIOS::LCD::Print( x+0, y+0, clrFront, RGBTRANS, szLabel );
	}

	virtual BOOL IsRunning()
	{
		return m_bRunning;
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( nKey == BIOS::KEY::KeyDown )
		{
			if ( m_nFocus < 2 )
			{
				m_nFocus++;
				Invalidate();
			}
			return;
		}
		if ( nKey == BIOS::KEY::KeyUp )
		{
			if ( m_nFocus > 0 )
			{
				m_nFocus--;
				Invalidate();
			}
			return;
		}
		if ( nKey == BIOS::KEY::KeyEnter )
		{
			m_bRunning = FALSE;
			return;
		}
		if ( nKey == BIOS::KEY::KeyLeft || nKey == BIOS::KEY::KeyRight )
			return;

		m_nFocus = -1;
		m_bRunning = FALSE;
	}

	virtual int GetResult()
	{
		return m_nFocus;
	}

	void DoModal()
	{
		ui16 buffer[Width*Height];
		BIOS::LCD::GetImage( m_rcClient, buffer );
		m_bRunning = true;
		m_bFirst = true;
		BIOS::ADC::Enable(false);
		CWnd* pSafeFocus = GetFocus();
		SetFocus();
		ShowWindow( CWnd::SwShow );
		Invalidate();
		while ( IsRunning() )
		{
			Sleep(20);
		}
		ShowWindow( CWnd::SwHide );
		BIOS::LCD::PutImage( m_rcClient, buffer );

		switch ( GetResult() )
		{
		case 0: 
			// Save bitmap
			SaveScreenshot();
			BIOS::Beep(200);
			break;
		case 1: 
			// Save wave TXT
			_ASSERT(0);
			break;
		case 2: 
			// Save wave SVG
			_ASSERT(0);
			break;
		case -1: break;
		}

		BIOS::ADC::Enable(true);
		pSafeFocus->SetFocus();
	}

	void SaveScreenshot()
	{
		char strName[] = "IMG000  BMP";
		bool bExists = false;
		FILEINFO f;
		do {
			bExists = false;
			if ( BIOS::DSK::Open( &f, strName, BIOS::DSK::IoRead ) )
			{
				BIOS::DSK::Close( &f );
				if ( ++strName[5] > '9' )
				{
					strName[5] = '0';
					if ( ++strName[4] > '9' )
					{
						strName[4] = '0';
						strName[3]++;
						_ASSERT( strName[3] <= '9' );
					}
				}
				continue;
			}
			break;
		} while (1);

		// strName contains unique non existent file name 
		if ( !BIOS::DSK::Open( &f, strName, BIOS::DSK::IoWrite ) )
		{
			_ASSERT(0);
			return;
		}
		int nSize = 0, nOffset = 0;
		ui8* pData = (ui8*)BIOS::DSK::GetSharedBuffer();

		BmpHdr* pHdr = (BmpHdr*)pData;
		pHdr->wBfType = 'B' | ('M'<<8);
		pHdr->dwBfSize = sizeof(BmpHdr) + BIOS::LCD::LcdWidth * BIOS::LCD::LcdHeight * 3;  // no need to align row to multiply of 4
		pHdr->wBfReserved1 = 0;
		pHdr->wBfReserved2 = 0;
		pHdr->dwBfOffset = 0x36;

		pHdr->dwBiSize = 0x28;
		pHdr->dwBiWidth = BIOS::LCD::LcdWidth;
		pHdr->dwBiHeight = BIOS::LCD::LcdHeight;
		pHdr->wBiPlanes = 1;
		pHdr->wBiBitDepth = 24;
		pHdr->dwBiCompression = 0;
		pHdr->dwBiSizeImage = BIOS::LCD::LcdWidth * BIOS::LCD::LcdHeight * 3;
		pHdr->dwBiXPels = 0;
		pHdr->dwBiYPels = 0;
		pHdr->dwBiClrUsed = 0;
		pHdr->dwBiClrImportant = 0;

		nOffset = sizeof(BmpHdr);
		for ( int y = BIOS::LCD::LcdHeight-1; y >= 0; y-- )
			for ( int x = 0; x < BIOS::LCD::LcdWidth; x++ )
			{
				unsigned char arrColors[3];
				ui16 wPixel = BIOS::LCD::GetPixel( x, y );
				if (((x>>2)+(y>>2))&1)
				{
					BIOS::LCD::PutPixel( x, y, wPixel^0x18e3);
				}
				arrColors[2] = Get565R(wPixel);
				arrColors[1] = Get565G(wPixel);
				arrColors[0] = Get565B(wPixel);
				for ( int c=0; c<3; c++ )
				{
					pData[nOffset] = arrColors[c];
					if ( ++nOffset >= FILEINFO::SectorSize )
					{
						nSize += FILEINFO::SectorSize;
						nOffset = 0;
						BIOS::DSK::Write( &f, pData );
					}

				}
			}

		if ( nOffset > 0 )
			BIOS::DSK::Write( &f, pData );

		BIOS::DSK::Close( &f, nSize + nOffset );
		// Display message
		//Sleep(500);

		for ( int y = BIOS::LCD::LcdHeight-1; y >= 0; y-- )
			for ( int x = 0; x < BIOS::LCD::LcdWidth; x++ )
			{
				if (((x>>2)+(y>>2))&1)
				{
					ui16 wPixel = BIOS::LCD::GetPixel( x, y );
					BIOS::LCD::PutPixel( x, y, wPixel^0x18e3);
					// ..... ...... .....
					// ...11 ...111 ...11
				}
			}

	}
};

class CWndMenuCursor : public CWnd
{
public:
	CWndMenuItem	m_itmX1;
	CWndMenuItem	m_itmX2;
	CWndMenuItem	m_itmY1;
	CWndMenuItem	m_itmY2;
	CWndMenuItem	m_itmMode;
	CWndMenuItem	m_itmDeltaX;
	CWndMenuItem	m_itmDeltaY;

	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndMenuCursor", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
		m_itmX1.Create("X1", RGB565(808080), 1, this);
		m_itmX2.Create("X2", RGB565(808080), 1, this);
		m_itmY1.Create("Y1", RGB565(808080), 1, this);
		m_itmY2.Create("Y2", RGB565(808080), 1, this);
		m_itmDeltaX.Create("dX", RGB565(ffffff), 2, this);
		m_itmDeltaY.Create("dY", RGB565(ffffff), 2, this);
		m_itmMode.Create("Mode\nManual", RGB565(808080), 1, this);
	}
};

class CWndMenuMeas : public CWnd
{
public:
	CWndMenuItem	m_itm1;
	CWndMenuItem	m_itm2;
	CWndMenuItem	m_itm3;
	CWndMenuItem	m_itm4;
	CWndMenuItem	m_itm5;
	CWndMenuItem	m_itm6;

	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndMenuMeas", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
		m_itm1.Create("RMS:\n5.323V", RGB565(ffff00), 2, this);
		m_itm2.Create("Vpp:\n12.000V", RGB565(ffff00), 2, this);
		m_itm3.Create("f:\n128.3 Hz", RGB565(ffff00), 2, this);
		m_itm4.Create("Lo:\n5.323V", RGB565(00ffff), 2, this);
		m_itm5.Create("Hi:\n12.000V", RGB565(00ffff), 2, this);
		m_itm6.Create("Avg:\n0.233V", RGB565(00ff00), 2, this);
	}
};

class CWndMenuSettings : public CWnd
{
public:
	CWndMenuItem	m_itmSave;
	CWndMenuItem	m_itmBacklight;
	CWndMenuItem	m_itmVolume;
	CWndMenuItem	m_itmLanguage;
	CWndMenuItem	m_itmAutoOff;
	CWndMenuItem	m_itmAbout;

	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndMenuSettings", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
		m_itmSave.Create("Save", RGB565(ffffff), 1, this);
		m_itmBacklight.Create("Backl.", RGB565(808080), 1, this);
		m_itmVolume.Create("Volume", RGB565(808080), 1, this);
		m_itmLanguage.Create("Language\nEnglish", RGB565(808080), 2, this);
		m_itmAutoOff.Create("Auto off\n5 min", RGB565(808080), 2, this);
		m_itmAbout.Create("About", RGB565(808080), 1, this);
	}
};

class CWndMenuDisplay : public CWnd
{
public:
	CWndMenuItem	m_itmMode;
	CWndMenuItem	m_itmDisplay;
	CWndMenuItem	m_itmDomain;
	CWndMenuItem	m_itmPersist;
	CWndMenuItem	m_itmAutoOff;
	CWndMenuItem	m_itmAbout;

	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndMenuDisplay", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
		m_itmMode.Create("Mode\nX-T", RGB565(808080), 2, this);
		m_itmDisplay.Create("Display\nPoints", RGB565(808080), 2, this);
		m_itmDomain.Create("Domain\nTime", RGB565(808080), 2, this);
		m_itmPersist.Create("Persist\nOff", RGB565(808080), 2, this);
	}
};

class CWndMenuMath : public CWnd
{
public:
	CWndMenuItem	m_itmChannel;
	CWndMenuItem	m_itmSource1;
	CWndMenuItem	m_itmSource2;
	CWndMenuItem	m_itmOperation;

	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndMenuMath", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
		m_itmChannel.Create("Off", RGB565(0000ff), 2, this);
		m_itmSource1.Create("Operand1\nChannel\nCh1", RGB565(ffff00), 3, this);
		m_itmSource2.Create("Operand2\nConstant\n4.0", RGB565(808080), 3, this);
		m_itmOperation.Create("Operator\nMultiply", RGB565(808080), 2, this);
	}
};

class CWndMenuTools : public CWnd
{
public:
	CWndMenuItem	m_itmBaudDet;
	CWndMenuItem	m_itmUartDecoder;
	CWndMenuItem	m_itmArea;

	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndMenuTools", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
		m_itmBaudDet.Create("BaudDet", RGB565(808080), 1, this);
		m_itmUartDecoder.Create("Uart", RGB565(808080), 1, this);
		m_itmArea.Create("Area", RGB565(808080), 1, this);
	}
};

class CWndMessageBox : public CWnd
{
	ui16 m_clrFrame;
	PCSTR m_strMessage;

public:
	void Show(CWnd* pParent, PCSTR strCaption, PCSTR strMessage, ui16 nColor)
	{
		m_clrFrame = nColor;
		m_strMessage = strMessage;

		if ( IsWindow() )
		{
			KillTimer();
			StopModal();	// will destroy this window
		}

		CWnd::Create(strCaption, CWnd::WsVisible, CRect(40, 100, 360, 160), pParent);
		StartModal();
		BIOS::Beep(100);
		SetTimer(3000);
	}

	virtual void OnPaint()
	{
		CRect rcClient = m_rcClient;
		CDesign::Window(m_rcClient, m_clrFrame);

		BIOS::LCD::Print( rcClient.CenterX()-((ui8)strlen(m_pszId)<<2), rcClient.top+2, 
			RGB565(000000), RGBTRANS, m_pszId);

		rcClient.Deflate(2+12+4, 2+14+2, 2+12+4, 2+2+0);
		int nExtentW = strlen(m_strMessage)*8;
		int nExtentH = 14;

		BIOS::LCD::Print( rcClient.Center().x - (nExtentW >> 1), rcClient.Center().y - (nExtentH >> 1 ),
			RGB565(000000), RGBTRANS, m_strMessage);		
	}

	virtual void OnKey(ui16 nKey)
	{
		KillTimer();
		StopModal();
	}


	virtual void OnTimer()
	{		
		KillTimer();
		StopModal();
	}
};

class CWndButton : public CWnd
{
public:
	virtual void OnPaint()
	{
		ui16 clr = HasFocus() ? RGB565(ffffff) : RGB565(000000);

		ui16 x = m_rcClient.left;
		x -= 8;
		x += BIOS::LCD::Draw( x, m_rcClient.top, clr, RGBTRANS, sel_left );
		x += BIOS::LCD::Print( x, m_rcClient.top, RGBTRANS, clr, this->m_pszId );
		x += BIOS::LCD::Draw( x, m_rcClient.top, clr, RGBTRANS, sel_right );
	}	
	virtual void OnKey(ui16 nKey)
	{
		if ( nKey == BIOS::KEY::KeyEnter )
		{
			GetParent()->OnKey( nKey );
			return;
		}
		CWnd::OnKey( nKey );
	}
};

class CWndConfirm : public CWnd
{
	ui16 m_clrFrame;
	PCSTR m_strMessage;
	PCSTR m_strButton1;
	PCSTR m_strButton2;
	CWndButton m_wndButton1;
	CWndButton m_wndButton2;

public:
	void Show(CWnd* pParent, PCSTR strCaption, PCSTR strMessage, ui16 nColor, PCSTR strButton1, PCSTR strButton2)
	{
		m_clrFrame = nColor;
		m_strMessage = strMessage;
		m_strButton1 = strButton1;
		m_strButton2 = strButton2;

		if ( IsWindow() )
			StopModal();	// will destroy this window
		
		int nLines = 1;
		for ( int i = 0; strMessage[i]; i++ )
			if ( strMessage[i] == '\n' )
				nLines++;

		CWnd::Create(strCaption, CWnd::WsVisible | CWnd::WsModal, CRect(30, 90, 350, 140 + nLines*16), pParent);
		m_wndButton1.Create( strButton1, CWnd::WsVisible, 
			CRect( m_rcClient.left + 20, m_rcClient.bottom - 20, m_rcClient.left + 20 + 40, m_rcClient.bottom - 4 ),
			this);
		m_wndButton2.Create( strButton2, CWnd::WsVisible, 
			CRect( m_rcClient.right - 20 - 40, m_rcClient.bottom - 20, m_rcClient.right - 20, m_rcClient.bottom - 4 ),
			this);
		StartModal( &m_wndButton1 );
	}

	void Hide()
	{
		if ( IsWindow() )
			StopModal();
	}

	virtual void OnPaint()
	{
		CRect rcClient = m_rcClient;
		CDesign::Window(m_rcClient, m_clrFrame);

		BIOS::LCD::Print( rcClient.CenterX()-((ui8)strlen(m_pszId)<<2), rcClient.top+2, 
			RGB565(000000), RGBTRANS, m_pszId);

		rcClient.Deflate(2+12+4, 2+14+2, 2+12+4, 2+2+0);

		BIOS::LCD::Print( rcClient.left + 4, rcClient.top + 4,
			RGB565(000000), RGBTRANS, m_strMessage);		
	}
	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & BIOS::KEY::KeyEnter )
		{
			_ASSERT( GetFocus() );
			SendMessage( GetParent(), ToWord('e', 'd'), (ui32)GetFocus()->m_pszId);
		}
	}
};

#endif