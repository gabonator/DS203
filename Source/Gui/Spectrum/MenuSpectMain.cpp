#include "MenuSpectMain.h"

#include <Source/Gui/MainWnd.h>

CWndMenuSpectMain::CWndMenuSpectMain()
{
}

/*virtual*/ void CWndMenuSpectMain::Create(CWnd *pParent, ui16 dwFlags) 
{
	CWnd::Create("CWndMenuSpectMain", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
	m_itmCH1.Create(&Settings.CH1, this);
	m_itmCH2.Create(&Settings.CH2, this);
	m_itmWindow.Create("Hann\nN=1024", RGB565(0000ff), 2, this);
}

/*virtual*/ void CWndMenuSpectMain::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	// LAYOUT ENABLE/DISABLE FROM TOP MENU BAR
	if (code == ToWord('L', 'D') )
	{
		MainWnd.m_wndSpectrumGraph.ShowWindow( SwHide );
	}

	if (code == ToWord('L', 'E') )
	{
		MainWnd.m_wndSpectrumGraph.ShowWindow( SwShow );
	}
}


