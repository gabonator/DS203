#include "Toolbar.h"
#include <Source/Gui/MainWnd.h>

CWndToolBar::CBarItem* CWndToolBar::GetMenuItems()
{
	static CBarItem arrMenuItems[] = 
	{
		{ CBarItem::IMain,	"Oscilloscope", &MainWnd.m_wndModuleSel, 0},	// 0
		{ CBarItem::ISub,	"Input", &MainWnd.m_wndMenuInput, 0},			// 1
		{ CBarItem::ISub,	"Cursor", &MainWnd.m_wndMenuCursor, 0},			// 2
		{ CBarItem::ISub,	"Meas", &MainWnd.m_wndMenuMeas, 0},				// 3
		{ CBarItem::ISub,	"Math", &MainWnd.m_wndMenuMath, 0},				// 4
		{ CBarItem::ISub,	"Disp", &MainWnd.m_wndMenuDisplay, 0},			// 5

		{ CBarItem::IMain,	"Spectrum", &MainWnd.m_wndModuleSel, 1},		// 6
		{ CBarItem::ISub,	"FFT", &MainWnd.m_wndSpectrumMain, 1},			// 7
		{ CBarItem::ISub,	"Marker", &MainWnd.m_wndSpectrumMarker, 1},		// 8

		{ CBarItem::IMain,	"Generator", &MainWnd.m_wndModuleSel, 2},		// 9
		{ CBarItem::ISub,	"Wave", &MainWnd.m_wndMenuGenerator, 2},		// 10
		{ CBarItem::ISub,	"Edit", &MainWnd.m_wndMenuGeneratorEdit, 2},	// 11
		{ CBarItem::ISub,	"Modulation", &MainWnd.m_wndMenuGeneratorMod, 2},	// 12
		
		{ CBarItem::IMain,	"Settings", &MainWnd.m_wndModuleSel, 3},		// 13
		{ CBarItem::ISub,	"Main", &MainWnd.m_wndMenuSettings, 3},			// 14
		{ CBarItem::ISub,	"Keys", &MainWnd.m_wndMenuKeySettings, 3},		// 15
		
		{ CBarItem::IMain,	"About", &MainWnd.m_wndModuleSel, 4},			// 16
		{ CBarItem::ISub,	"Device", &MainWnd.m_wndAboutDevice, 4},		// 17
		{ CBarItem::ISub,	"Firmware", &MainWnd.m_wndAboutFirmware, 4},	// 18


		{ CBarItem::IMain,	"User app", &MainWnd.m_wndModuleSel, 5},		// 19
		{ CBarItem::ISub,	"Tuner", &MainWnd.m_wndUserTuner, 5},			// 20
		{ CBarItem::ISub,	"Demo", &MainWnd.m_wndScreenSaver, 5},			// 21
		{ CBarItem::ISub,	"Snake", &MainWnd.m_wndUserGame, 5},			// 22
		//{ CBarItem::ISub,	"Balls", &MainWnd.m_wndUserBalls, 5},
		{ CBarItem::ISub,	"Calib", &MainWnd.m_wndUserCalibration, 5},		// 23
		{ CBarItem::ISub,	"Meter", &MainWnd.m_wndUserMeter, 5},			// 24

		{ CBarItem::IMain,	"Dmm", &MainWnd.m_wndModuleSel, 6},				// 25
		{ CBarItem::ISub,	"Meas", &MainWnd.m_wndUserDmm, 6},				// 26

		{ CBarItem::IEnd,		NULL, NULL }
	};

	return arrMenuItems;
}

/*virtual*/ void CWndToolBar::OnPaint()
{
	CWndToolBar::CBarItem* pItems = GetMenuItems();

	int nFocus = m_nFocus;

	// find nearest sub menu
	int nMenu = nFocus;
	while ( pItems[nMenu].m_eType == CBarItem::ISub && nMenu > 0 )
		nMenu--;

	_ASSERT( pItems[nMenu].m_eType == CBarItem::IMain );

	int x = m_rcClient.left;
	int nExtentx = (strlen(pItems[nMenu].m_pName) << 3);
	
	if ( nFocus == nMenu && HasFocus() )
	{
		x += BIOS::LCD::Draw(x, 0, RGB565(ffffff), RGB565(000000), CShapes::corner_left);
		BIOS::LCD::Bar( x, m_rcClient.top, x + nExtentx, m_rcClient.bottom, RGB565(FFFFFF));	
		x += BIOS::LCD::Print(x, 0, RGB565(000000), RGBTRANS, pItems[nMenu].m_pName);
	} else {
		x += BIOS::LCD::Draw(x, 0, RGB565(0020ff), RGB565(000000), CShapes::corner_left);
		BIOS::LCD::Bar( x, m_rcClient.top, x + nExtentx, m_rcClient.bottom, RGB565(0020ff));
		x += BIOS::LCD::Print(x, 0, RGB565(ffffff), RGBTRANS, pItems[nMenu].m_pName);
	}

	CRect rcSub( m_rcClient );
	rcSub.left = x;

	// gradient background
	CDesign::GradientTop( rcSub );

	if ( nFocus == nMenu && HasFocus() )
		x += BIOS::LCD::Draw(x, 0, RGB565(ffffff), RGBTRANS, CShapes::corner_right);
	else
		x += BIOS::LCD::Draw(x, 0, RGB565(0020ff), RGBTRANS, CShapes::corner_right);
	x += 10;

	for (int i=nMenu+1; pItems[i].m_eType == CBarItem::ISub; i++)
	{
		ui8 bSelected = (i==nFocus);
		u16 clr = bSelected ? clrSelected : clrNormal;
		u16 bgr = bSelected ? 0 : RGBTRANS;

		if ( HasFocus() && bSelected )
		{
			clr = clrSelectedFocus;
			bgr = bgrSelectedFocus;
		}
		
		if ( bSelected )
			x += BIOS::LCD::Draw( x, m_rcClient.top, bgr, RGBTRANS, CShapes::corner_left);
		else
			x += 8;
		
		if ( bSelected )	                                           
			BIOS::LCD::Bar( x, m_rcClient.top, x + (strlen(pItems[i].m_pName)<<3), m_rcClient.bottom, bgr);

		x += BIOS::LCD::Print( x, m_rcClient.top, clr, bgr, pItems[i].m_pName);

		if ( bSelected )
			x += BIOS::LCD::Draw( x, m_rcClient.top, bgr, RGBTRANS, CShapes::corner_right);
		else
			x += 8;
	}

}

/*virtual*/ void CWndToolBar::OnKey(ui16 nKey)
{
	CWndToolBar::CBarItem* pItems = GetMenuItems();
	ui8 oldFocus = m_nFocus;
	if ( nKey & BIOS::KEY::KeyLeft)
	{
		if ( pItems[m_nFocus].m_eType == CBarItem::IMain ) 
		{
			m_nFocus++;
			while((pItems[m_nFocus].m_eType != CBarItem::IMain)&&(pItems[m_nFocus].m_eType != CBarItem::IEnd))
			{	// Find the last item
				m_nFocus++;
			}
			m_nFocus--;
		}
		else 
		{
			m_nFocus--;
		}
		CWndToolBar::ChangeFocus(oldFocus);
	}

	if ( nKey & BIOS::KEY::KeyRight)
	{
		if ((pItems[m_nFocus+1].m_eType == CBarItem::IMain)||((pItems[m_nFocus+1].m_eType == CBarItem::IEnd)))
		{
			m_nFocus--;
			while(pItems[m_nFocus].m_eType != CBarItem::IMain)
			{	// Find the first item
				m_nFocus--;
			}
		}
		else
		{
			m_nFocus++;
		}
		CWndToolBar::ChangeFocus(oldFocus);
	}

	if( nKey & BIOS::KEY::KeyEnter )
	{
		CWnd::OnKey( BIOS::KEY::KeyDown );
	}

	if( nKey & BIOS::KEY::KeyEscape )
	{	// Focus on first item
		if (pItems[m_nFocus].m_eType != CBarItem::IMain)
		{
			m_nFocus--;
			while(pItems[m_nFocus].m_eType != CBarItem::IMain)
			{	// Find the first item
				m_nFocus--;
			}
			CWndToolBar::ChangeFocus(oldFocus);
		}
	}
	CWnd::OnKey( nKey );
}

/*virtual*/ void CWndToolBar::ChangeFocus(ui8 oldFocus)
{
	CWndToolBar::CBarItem* pItems = GetMenuItems();
	SendMessage( GetParent(), ToWord('L', 'D'), (NATIVEPTR)pItems[oldFocus].m_pWndMenu );
	SendMessage( GetParent(), ToWord('L', 'E'), (NATIVEPTR)pItems[m_nFocus].m_pWndMenu );
	SendMessage( GetParent(), ToWord('L', 'R'), 0 );
	Settings.Runtime.m_nMenuItem = m_nFocus;
	if ( pItems[m_nFocus].m_eType != CBarItem::IMain )
	{	// Store sub item position
		Settings.Runtime.m_nSubMenuItems[pItems[m_nFocus].m_mainMenuIndex] = m_nFocus;
	}
}

/*virtual*/ void CWndToolBar::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	if ( code == ToWord('g', 'i') )
	{
		CBarItem *pItems = GetMenuItems();
		m_nFocus = (ui8)data;
		SendMessage( GetParent(), ToWord('L', 'E'), (NATIVEPTR)pItems[m_nFocus].m_pWndMenu );
		SetFocus();
	}

	if ( code == ToWord('g', 'o') )
	{
		const ui8 i = (const ui8)data;
		CBarItem *pItems = GetMenuItems();
		SendMessage( GetParent(), ToWord('L', 'D'), (NATIVEPTR)pItems[m_nFocus].m_pWndMenu );
		if ( pItems[i].m_eType == CBarItem::IMain ) 
		{
			int subMenuIndex = Settings.Runtime.m_nSubMenuItems[pItems[i].m_mainMenuIndex];
			if (subMenuIndex  != -1) 
			{
				m_nFocus = subMenuIndex;
			}
			else
			{
				m_nFocus = i+1;
			}
		}
		else
		{
			m_nFocus = i;
		}
		SendMessage( GetParent(), ToWord('L', 'E'), (NATIVEPTR)pItems[m_nFocus].m_pWndMenu );
		SetFocus();
		SendMessage( GetParent(), ToWord('L', 'R'), 0 );
		Settings.Runtime.m_nMenuItem = m_nFocus;
		Settings.Runtime.m_nSubMenuItems[pItems[m_nFocus].m_mainMenuIndex] = m_nFocus;
	}
}

CWnd* CWndToolBar::GetCurrentLayout()
{
	CWndToolBar::CBarItem* pItems = GetMenuItems();
	return pItems[m_nFocus].m_pWndMenu;
}

CWnd* CWndToolBar::GetLastActiveWindow()
{
	CWndToolBar::CBarItem* pItems = GetMenuItems();
	return pItems[m_nFocus].m_pWndMenu->GetLastActiveWindow();
}

