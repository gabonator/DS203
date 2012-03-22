
#include "MainMenu.h"
#include <Source/Gui/MainWnd.h>
#include <Source/Core/Design.h>
#include <Source/Core/Shapes.h>


/*virtual*/ void CWndModuleSelector::Create(CWnd *pParent, ui16 dwFlags) 
{
	CWnd::Create("CWndModuleSelector", dwFlags, CRect(0, 16, 400, 240), pParent);

	#define _BORDER 20
	#define _SPACING 8
	#define _LEFT(x) m_rcClient.left+_BORDER+(m_rcClient.Width()-_BORDER*2-_SPACING)*(x)/3
	#define _RIGHT(x) _LEFT(x+1)-_SPACING
	#define _TOP(y) m_rcClient.top+_BORDER+(m_rcClient.Height()-_BORDER*2-_SPACING)*(y)/3
	#define _BOTTOM(y) _TOP(y+1)-_SPACING
	#define _ITEM(x,y) CRect(_LEFT(x), _TOP(y), _RIGHT(x), _BOTTOM(y))

	CRect rcItem;

	rcItem = _ITEM(0, 0);
	m_itmOscilloscope.Create( "Oscillo\nscope", RGB565(ffffff), rcItem, this );
	rcItem = _ITEM(1, 0);
	m_itmSpectrum.Create( "Spectrum\nanalyser", RGB565(ffffff), rcItem, this );
	rcItem = _ITEM(2, 0);
	m_itmGenerator.Create( "Signal\nGenerator", RGB565(ffffff), rcItem, this );

	rcItem = _ITEM(0, 1);
	m_itmSettings.Create( "Settings", RGB565(ffffff), rcItem, this );
	rcItem = _ITEM(1, 1);
	m_itmAbout.Create( "About", RGB565(ffffff), rcItem, this );

}

/*virtual*/ void CWndModuleSelector::OnPaint()
{
	BIOS::LCD::Bar( m_rcClient.left, m_rcClient.top, m_rcClient.right, m_rcClient.bottom, RGB565(0020ff));
	CWnd::OnPaint();
}

/*virtual*/ void CWndModuleSelector::OnKey(ui16 nKey)
{
	if ( nKey & BIOS::KEY::KeyEnter )
	{
		const char* strTarget = NULL;
		if ( GetFocus() == &m_itmOscilloscope )
			strTarget = "Oscilloscope"; 
		if ( GetFocus() == &m_itmSpectrum )
			strTarget = "Spectrum";
		if ( GetFocus() == &m_itmGenerator )
			strTarget = "Generator"; 
		if ( GetFocus() == &m_itmSettings )
			strTarget = "Settings"; 
		if ( GetFocus() == &m_itmAbout )
			strTarget = "About";
		
		if (strTarget)
		{
			SendMessage( &MainWnd.m_wndToolBar, ToWord('g', 'o'), (NATIVEPTR)strTarget);
		}
		return;
	}
	CWnd::OnKey( nKey );
}
