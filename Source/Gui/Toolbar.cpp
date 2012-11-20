#include "Toolbar.h"
#include <Source/Gui/MainWnd.h>

CWndToolBar::CBarItem* CWndToolBar::GetMenuItems()
{
	static CBarItem arrMenuItems[] = 
	{
		{ CBarItem::IMain,	"Oscilloscope", &MainWnd.m_wndModuleSel},
		{ CBarItem::ISub,	"Input", &MainWnd.m_wndMenuInput},
		{ CBarItem::ISub,	"Cursor", &MainWnd.m_wndMenuCursor},
		{ CBarItem::ISub,	"Meas", &MainWnd.m_wndMenuMeas},
		{ CBarItem::ISub,	"Math", &MainWnd.m_wndMenuMath},
		{ CBarItem::ISub,	"Disp", &MainWnd.m_wndMenuDisplay},

		{ CBarItem::IMain,	"Spectrum", &MainWnd.m_wndModuleSel},
		{ CBarItem::ISub,	"FFT", &MainWnd.m_wndSpectrumMain},
		{ CBarItem::ISub,	"Marker", &MainWnd.m_wndSpectrumMarker},

		{ CBarItem::IMain,	"Generator", &MainWnd.m_wndModuleSel},
		{ CBarItem::ISub,	"Wave", &MainWnd.m_wndMenuGenerator},
		{ CBarItem::ISub,	"Edit", &MainWnd.m_wndMenuGeneratorEdit},
		{ CBarItem::ISub,	"Modulation", &MainWnd.m_wndMenuGeneratorMod},
		
		{ CBarItem::IMain,	"Settings", &MainWnd.m_wndModuleSel},
		{ CBarItem::ISub,	"Main", &MainWnd.m_wndMenuSettings},
		
		{ CBarItem::IMain,	"About", &MainWnd.m_wndModuleSel},
		{ CBarItem::ISub,	"Device", &MainWnd.m_wndAbout},

		{ CBarItem::IMain,	"User app", &MainWnd.m_wndModuleSel},
		{ CBarItem::ISub,	"Demo", &MainWnd.m_wndScreenSaver},
		{ CBarItem::ISub,	"Snake", &MainWnd.m_wndUserGame},
		{ CBarItem::ISub,	"Balls", &MainWnd.m_wndUserBalls},
		{ CBarItem::ISub,	"Calib", &MainWnd.m_wndUserCalibration},
		{ CBarItem::ISub,	"Meter", &MainWnd.m_wndUserMeter},

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
		x += BIOS::LCD::Draw(x, 0, RGB565(ffffff), RGB565(000000), corner_left);
		BIOS::LCD::Bar( x, m_rcClient.top, x + nExtentx, m_rcClient.bottom, RGB565(FFFFFF));	
		x += BIOS::LCD::Print(x, 0, RGB565(000000), RGBTRANS, pItems[nMenu].m_pName);
	} else {
		x += BIOS::LCD::Draw(x, 0, RGB565(0020ff), RGB565(000000), corner_left);
		BIOS::LCD::Bar( x, m_rcClient.top, x + nExtentx, m_rcClient.bottom, RGB565(0020ff));
		x += BIOS::LCD::Print(x, 0, RGB565(ffffff), RGBTRANS, pItems[nMenu].m_pName);
	}

	CRect rcSub( m_rcClient );
	rcSub.left = x;

	// gradient background
	CDesign::GradientTop( rcSub );

	if ( nFocus == nMenu && HasFocus() )
		x += BIOS::LCD::Draw(x, 0, RGB565(ffffff), RGBTRANS, corner_right);
	else
		x += BIOS::LCD::Draw(x, 0, RGB565(0020ff), RGBTRANS, corner_right);
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
			x += BIOS::LCD::Draw( x, m_rcClient.top, bgr, RGBTRANS, corner_left);
		else
			x += 8;
		
		if ( bSelected )	                                           
			BIOS::LCD::Bar( x, m_rcClient.top, x + (strlen(pItems[i].m_pName)<<3), m_rcClient.bottom, bgr);

		x += BIOS::LCD::Print( x, m_rcClient.top, clr, bgr, pItems[i].m_pName);

		if ( bSelected )
			x += BIOS::LCD::Draw( x, m_rcClient.top, bgr, RGBTRANS, corner_right);
		else
			x += 8;
	}

}

/*virtual*/ void CWndToolBar::OnKey(ui16 nKey)
{
	CWndToolBar::CBarItem* pItems = GetMenuItems();

	if ( nKey & BIOS::KEY::KeyLeft && m_nFocus > 0 )
	{
		if ( pItems[m_nFocus].m_eType == CBarItem::IMain )
			return;

		SendMessage( GetParent(), ToWord('L', 'D'), (NATIVEPTR)pItems[m_nFocus].m_pWndMenu );
		m_nFocus--;
		SendMessage( GetParent(), ToWord('L', 'E'), (NATIVEPTR)pItems[m_nFocus].m_pWndMenu );
		SendMessage( GetParent(), ToWord('L', 'R'), 0 );
		Settings.Runtime.m_nMenuItem = m_nFocus;
	}

	if ( nKey & BIOS::KEY::KeyRight && GetMenuItems()[m_nFocus+1].m_eType != CBarItem::IEnd )
	{
		if ( pItems[m_nFocus+1].m_eType == CBarItem::IMain )
			return;

		SendMessage( GetParent(), ToWord('L', 'D'), (NATIVEPTR)pItems[m_nFocus].m_pWndMenu );
		m_nFocus++;
		SendMessage( GetParent(), ToWord('L', 'E'), (NATIVEPTR)pItems[m_nFocus].m_pWndMenu );
		SendMessage( GetParent(), ToWord('L', 'R'), 0 );
		Settings.Runtime.m_nMenuItem = m_nFocus;
	}
	CWnd::OnKey( nKey );
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
		const char* strId = (const char*)data;
		CBarItem *pItems = GetMenuItems();

		for (int i=0; pItems[i].m_eType != CBarItem::IEnd; i++ )
		{
			if ( /*pItems[i].m_eType == CBarItem::IMain &&*/ 
				 pItems[i].m_pName == strId )
			{
				SendMessage( GetParent(), ToWord('L', 'D'), (NATIVEPTR)pItems[m_nFocus].m_pWndMenu );
				if ( pItems[i].m_eType == CBarItem::IMain )
					m_nFocus = i+1;
				else 
					m_nFocus = i;
				SendMessage( GetParent(), ToWord('L', 'E'), (NATIVEPTR)pItems[m_nFocus].m_pWndMenu );
				SetFocus();
				SendMessage( GetParent(), ToWord('L', 'R'), 0 );
				Settings.Runtime.m_nMenuItem = m_nFocus;
				return;
			}
		}
		_ASSERT(0);
	}
}

CWnd* CWndToolBar::GetCurrentLayout()
{
	CWndToolBar::CBarItem* pItems = GetMenuItems();
	return pItems[m_nFocus].m_pWndMenu;
}


