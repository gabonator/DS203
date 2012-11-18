#include "MenuGenEdit.h"
#include <Source/Gui/MainWnd.h>

/*virtual*/ void CWndMenuGeneratorEdit::Create(CWnd *pParent, ui16 dwFlags) 
{
	CWnd::Create("CWndMenuGeneratorEdit", dwFlags, CRect(280-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
	m_itm[0].Create("~Copy\nCH1", RGB565(b00040), 2, this );
	m_itm[1].Create("~Edit", RGB565(b00040), 1, this );
	m_itm[2].Create("~Load", RGB565(b00040), 1, this );
	m_itm[3].Create("~Save", RGB565(b00040), 1, this );
}

/*virtual*/ void CWndMenuGeneratorEdit::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
}

