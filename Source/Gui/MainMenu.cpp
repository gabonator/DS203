
#include "MainMenu.h"
#include <Source/Gui/MainWnd.h>
#include <Source/Core/Design.h>
#include <Source/Core/Shapes.h>

const CWndModuleSelector::TMenuBlockStruct* CWndModuleSelector::GetLayout()
{
	static const CWndModuleSelector::TMenuBlockStruct arrLayout[10] = 
	{
		// CWnd, Label, color, target sent to Toolbar.cpp
		{ &m_itmOscilloscope,	"Oscillo\nscope",		RGB565(ffffff), "Oscilloscope" },
		{ &m_itmSpectrum,		"Spectrum\nanalyser",	RGB565(ffffff), "Spectrum" },
		{ &m_itmGenerator,		"Signal\ngenerator",	RGB565(ffffff), "Generator" },

		{ &m_itmSettings,		"Settings",				RGB565(ffffff), "Settings" },
		{ &m_itmUser,			"User\napplications",	RGB565(ffffff), "User app" },
		{ &m_itmAbout,			"About",				RGB565(ffffff), "About" },

		{ &m_itmDmm,			"Dmm",					RGB565(ffffff), "Dmm" },
		{ &m_itmResponse,		"Frequency\nresponse",	RGB565(808080), NULL },
		{ &m_itmLogic,			"Logic\nanalyser",		RGB565(808080), NULL },
		{ NULL,					NULL,					RGB565(808080), NULL }
	};

	return arrLayout;
}

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
	const CWndModuleSelector::TMenuBlockStruct* arrLayout = GetLayout();

	int nIndex = 0;
	for ( int y = 0; y < 3; y++ )
		for ( int x = 0; x < 3; x++, nIndex++ )
		{
			const CWndModuleSelector::TMenuBlockStruct* pItem = &arrLayout[nIndex];
			if ( pItem->m_pWnd )
			{
				CRect rcItem = _ITEM(x, y);          
				pItem->m_pWnd->Create( pItem->m_strLabel, pItem->m_clr, rcItem, this );
			}
		}
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
		const CWndModuleSelector::TMenuBlockStruct* arrLayout = GetLayout();
		int nId = _GetItemId( GetFocus() );
		const char* strTarget = arrLayout[nId].m_strTarget;
		
		if (strTarget)
		{
			SendMessage( &MainWnd.m_wndToolBar, ToWord('g', 'o'), (NATIVEPTR)strTarget);
		} else {
			MainWnd.m_wndMessage.Show(this, "Info", "Sorry, not implemented", RGB565(FFFF00));
		}
		return;
	}
	if ( nKey & ( BIOS::KEY::KeyLeft | BIOS::KEY::KeyRight | BIOS::KEY::KeyUp | BIOS::KEY::KeyDown ) )
	{
		CWnd* pCurrent = GetFocus();
		int nCurrentId = _GetItemId(pCurrent);
		_ASSERT( nCurrentId >= 0 && nCurrentId <= 9 );

		int _x = nCurrentId % 3;
		int _y = nCurrentId / 3;
		if (((nKey & BIOS::KEY::KeyUp) && _y == 0 && _x == 0) || ((nKey & BIOS::KEY::KeyDown) && _x == 2 && _y == 2))
		{
			MainWnd.m_wndToolBar.SetFocus();
			pCurrent->Invalidate();
			MainWnd.m_wndToolBar.Invalidate();
			return;
		}
		if ( nKey & BIOS::KEY::KeyLeft)
		{
			_x--;
			if(_x < 0) 
			{
				_x=2;
				_y--;
				if(_y < 0)
				{ 
					_y = 2;
				}
			}
		}
		if ( nKey & BIOS::KEY::KeyRight)
		{
			_x++;
			if(_x > 2)
			{
				_x=0;
				_y++;
				if(_y > 2)
				{
					_y = 0;
				}
			}
		}
		if ( nKey & BIOS::KEY::KeyUp)
		{
			_y--;
			if(_y < 0)
			{
				_y=2;
				_x--;
				if(_x < 0)
				{
					_x = 2;
				}
			}
		}
		if ( nKey & BIOS::KEY::KeyDown)
		{
			_y++;
			if(_y > 2)
			{
				_y=0;
				_x++;
				if(_x > 2)
				{
					_x = 0;
				}
			}
		}
		
		int nNewId = _y * 3 + _x;
		CWnd* pNew = NULL;
		
		if ( nNewId != nCurrentId ) 
			pNew = _GetWindowById( nNewId );
		if ( pNew )
		{
			pNew->SetFocus();
			pCurrent->Invalidate();
			pNew->Invalidate();
		}
		return;
	}

	CWnd::OnKey( nKey );
}

int CWndModuleSelector::_GetItemId(CWnd* pWnd)
{
	const CWndModuleSelector::TMenuBlockStruct* arrLayout = GetLayout();
	
	for (int i=0; i<9; i++)
		if ( arrLayout[i].m_pWnd == pWnd )
			return i;
	return -1;
}

CWnd* CWndModuleSelector::_GetWindowById(int nId)
{
	const CWndModuleSelector::TMenuBlockStruct* arrLayout = GetLayout();
	_ASSERT( nId >= 0 && nId <= 9 );
	return arrLayout[ nId ].m_pWnd;
}
