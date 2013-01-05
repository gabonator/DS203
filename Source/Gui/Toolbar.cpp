#include "Toolbar.h"
#include <Source/Gui/MainWnd.h>

/*static*/ const CWndToolBar::CBarItem* CWndToolBar::GetMenuItems()
{
	static const CBarItem arrMenuItems[] = {
		{ CBarItem::IMain,	(PSTR)"Oscilloscope", &MainWnd.m_wndModuleSel},
		{ CBarItem::ISub,	(PSTR)"Input", &MainWnd.m_wndMenuInput},
		{ CBarItem::ISub,	(PSTR)"Cursor", &MainWnd.m_wndMenuCursor},
		{ CBarItem::ISub,	(PSTR)"Meas", &MainWnd.m_wndMenuMeas},
		{ CBarItem::ISub,	(PSTR)"Math", &MainWnd.m_wndMenuMath},
		{ CBarItem::ISub,	(PSTR)"Disp", &MainWnd.m_wndMenuDisplay},
		{ CBarItem::ISub,	(PSTR)"Mask", &MainWnd.m_wndMenuMask},

		{ CBarItem::IMain,	(PSTR)"Spectrum", &MainWnd.m_wndModuleSel},
		{ CBarItem::ISub,	(PSTR)"FFT", &MainWnd.m_wndSpectrumMain},
		{ CBarItem::ISub,	(PSTR)"Marker", &MainWnd.m_wndSpectrumMarker},

		{ CBarItem::IMain,	(PSTR)"Generator", &MainWnd.m_wndModuleSel},
		{ CBarItem::ISub,	(PSTR)"Wave", &MainWnd.m_wndMenuGenerator},
		{ CBarItem::ISub,	(PSTR)"Edit", &MainWnd.m_wndMenuGeneratorEdit},
//		{ CBarItem::ISub,	(PSTR)"Modulation", &MainWnd.m_wndMenuGeneratorMod},
		
		{ CBarItem::IMain,	(PSTR)"Settings", &MainWnd.m_wndModuleSel},
		{ CBarItem::ISub,	(PSTR)"Main", &MainWnd.m_wndMenuSettings},
		{ CBarItem::ISub,	(PSTR)"Keys", &MainWnd.m_wndMenuKeySettings},
		{ CBarItem::ISub,	(PSTR)"Calib", &MainWnd.m_wndCalibration},
		
		{ CBarItem::IMain,	(PSTR)"About", &MainWnd.m_wndModuleSel},
		{ CBarItem::ISub,	(PSTR)"Device", &MainWnd.m_wndAboutDevice},
		{ CBarItem::ISub,	(PSTR)"Firmware", &MainWnd.m_wndAboutFirmware},
		{ CBarItem::ISub,	(PSTR)"Status", &MainWnd.m_wndAboutStatus},


		{ CBarItem::IMain,	(PSTR)"User app", &MainWnd.m_wndModuleSel},

#		define ADD_MODULE( strName, type ) { CBarItem::ISub, (PSTR)strName, &MainWnd.m_wndUser##type },
#		include "User/_Modules.h"
#		undef ADD_MODULE

		{ CBarItem::IEnd,		NULL, NULL }
	};

	return arrMenuItems;
}

/*virtual*/ void CWndToolBar::OnPaint()
{
	const CWndToolBar::CBarItem* pItems = GetMenuItems();
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

	int nIgnoreFirst = 1; // 1 -> first sub menu
	
	// calculate how many items we need to hide from left to reach the selected one
	int nRequired = 0;
	int nAvailable = BIOS::LCD::LcdWidth - 16 - x; // 16px reserved for arrows
	for ( int i = nFocus; i > 0 && pItems[i].m_eType != CBarItem::IMain; i-- )
	{
		nRequired += strlen(pItems[i].m_pName)*8 + 16;
		if ( nRequired > nAvailable )
			nIgnoreFirst++;
	}

	if ( nIgnoreFirst > 1 )
		x += BIOS::LCD::Print( x, m_rcClient.top, RGB565(b0b0b0), RGBTRANS, "\x11");

	for ( int i = nMenu+nIgnoreFirst; pItems[i].m_eType == CBarItem::ISub; i++ )
	{
		ui8 bSelected = (i==nFocus);
		u16 clr = bSelected ? clrSelected : clrNormal;
		u16 bgr = bSelected ? 0 : RGBTRANS;

		if ( HasFocus() && bSelected )
		{
			clr = clrSelectedFocus;
			bgr = bgrSelectedFocus;
		}

		if ( x + 16 + strlen(pItems[i].m_pName)*8 >= BIOS::LCD::LcdWidth )
		{
			x += BIOS::LCD::Print( x, m_rcClient.top, RGB565(b0b0b0), RGBTRANS, "\x10");
			break;
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
	const CWndToolBar::CBarItem* pItems = GetMenuItems();
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
	const CWndToolBar::CBarItem* pItems = GetMenuItems();
	SendMessage( GetParent(), ToWord('L', 'D'), (NATIVEPTR)pItems[oldFocus].m_pWndMenu );
	SendMessage( GetParent(), ToWord('L', 'E'), (NATIVEPTR)pItems[m_nFocus].m_pWndMenu );
	SendMessage( GetParent(), ToWord('L', 'R'), 0 );
	Settings.Runtime.m_nMenuItem = m_nFocus;
}

/*virtual*/ void CWndToolBar::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	if ( code == ToWord('g', 'i') )
	{
		const CBarItem *pItems = GetMenuItems();
		m_nFocus = (ui8)data;
		SendMessage( GetParent(), ToWord('L', 'E'), (NATIVEPTR)pItems[m_nFocus].m_pWndMenu );
		SetFocus();
	}

	if ( code == ToWord('g', 'o') )
	{
		const char* strId = (const char*)data;
		int nItem = Find( strId );
		
		code = ToWord('g', '2');
		data = nItem;
	}

	if ( code == ToWord('g', '2') )
	{
		const CBarItem *pItems = GetMenuItems();

		int nItem = data;
		if ( nItem == -1 )
			return;

		SendMessage( GetParent(), ToWord('L', 'D'), (NATIVEPTR)pItems[m_nFocus].m_pWndMenu );

		if ( pItems[nItem].m_eType == CBarItem::IMain ) 
			m_nFocus = nItem+1;
		else
			m_nFocus = nItem;

		SendMessage( GetParent(), ToWord('L', 'E'), (NATIVEPTR)pItems[m_nFocus].m_pWndMenu );
		SetFocus();
		SendMessage( GetParent(), ToWord('L', 'R'), 0 );
		Settings.Runtime.m_nMenuItem = m_nFocus;
	}

}

CWnd* CWndToolBar::GetCurrentLayout()
{
	const CWndToolBar::CBarItem* pItems = GetMenuItems();
	return (CWnd*)pItems[m_nFocus].m_pWndMenu;
}

int CWndToolBar::Find(const char* strId)
{	
	const CBarItem *pItems = GetMenuItems();

	for (int i=0; pItems[i].m_eType != CBarItem::IEnd; i++ )
	{
		if ( /*pItems[i].m_eType == CBarItem::IMain &&*/ 
				strcmp(pItems[i].m_pName, strId) == 0 )
		{
			return i;
		}
	}
	_ASSERT( 0 );
	return -1;
}