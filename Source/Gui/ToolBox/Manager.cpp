#include "ToolBox.h"
#include <Source/Gui/MainWnd.h>

CWndManager::CWndManager()
{
	m_bRunning = false;
}

void CWndManager::Create( CWnd* pParent )
{
	CListBox::Create("Wave Manager", CWnd::WsHidden | CWnd::WsModal, 
		CRect( (BIOS::LCD::LcdWidth-Width)/2 ,
		(BIOS::LCD::LcdHeight-Height)/2,
		(BIOS::LCD::LcdWidth+Width)/2,
		(BIOS::LCD::LcdHeight+Height)/2 ), 
		RGB565(8080d0), 
		pParent);

	m_nValue = 0;
	m_proValue.Create( &m_nValue, 0, 100 );
	m_itmValue.Create( "File nr.", CWnd::WsVisible, &m_proValue, this );
	m_itmSpace1.Create( "", CWnd::WsVisible | CWnd::WsNoActivate, this);
	m_itmFile.Create( "    WAVE000.DAT", CWnd::WsVisible | CWnd::WsNoActivate, this);
	m_itmSpace2.Create( "", CWnd::WsVisible | CWnd::WsNoActivate, this);
	m_proLoad.Create( "Load" );
	m_itmLoad.Create( "", CWnd::WsVisible, &m_proLoad, this );
	m_proSave.Create( "Save" );
	m_itmSave.Create( "", CWnd::WsVisible, &m_proSave, this );
}

/*virtual*/ BOOL CWndManager::IsRunning()
{
	return m_bRunning;
}

/*virtual*/ int CWndManager::GetResult()
{
	return 0;
}

void CWndManager::DoModal()
{
	BIOS::Beep(200);
	m_bRunning = true;

	CWnd* pSafeFocus = GetFocus();
	m_itmValue.SetFocus();
	ShowWindow( CWnd::SwShow );
	Invalidate();
	while ( IsRunning() )
	{
		Sleep(20);
	}
	ShowWindow( CWnd::SwHide );
/*
	switch ( GetResult() )
	{
	}
	*/

	pSafeFocus->SetFocus();
	MainWnd.Invalidate();
}

bool CWndManager::Exists(char *strName)
{
	FILEINFO f;
    if ( BIOS::DSK::Open( &f, strName, BIOS::DSK::IoRead ) )
	{
		BIOS::DSK::Close( &f );
		return true;
	}
	return false;
}

/*virtual*/ void CWndManager::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	if ( pSender == &m_itmValue && code == ToWord('u', 'p') )
	{
		char strName[13];
		BIOS::DBG::sprintf( m_strCurrent, "    WAVE%03d.DAT", m_nValue );
		m_itmFile.m_pszId = m_strCurrent;
		m_itmFile.Invalidate();

		BIOS::DBG::sprintf( strName, "WAVE%03d DAT", m_nValue );
		bool bExists = Exists(strName);
		if ( bExists != m_itmLoad.IsVisible() )
		{
			m_itmLoad.ShowWindow( bExists ? CWnd::SwShow : CWnd::SwHide );
			Invalidate();
		} 
	}
	if ( code == ToWord('l', 'e') && data == (NATIVEPTR)&m_itmLoad )
	{
		// load
	}
	if ( code == ToWord('l', 'e') && data == (NATIVEPTR)&m_itmSave )
	{
		// save
	}
	if ( code == ToWord('o', 'k') )
	{
		m_bRunning = false;
	}
	if ( code == ToWord('e', 'x') )
	{
		m_bRunning = false;
	}
}
