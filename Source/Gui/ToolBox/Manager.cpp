#include "ToolBox.h"
#include <Source/Gui/MainWnd.h>


const char* const CWndManager::tabs[] = {"Wave", "Bmp", "Csv", "Svg", NULL};
const char* const CWndManager::strTemplateDisplay[] = {"    WAVE%03d.DAT", "    IMAGE%03d.BMP", "    WAVE%03d.CSV", "    WAVE%03d.SVG"};
const char* const CWndManager::strTemplateFile[] = {"WAVE%03d DAT", "IMAGE%03dBMP", "WAVE%03d CSV", "WAVE%03d SVG"};

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
	m_itmTabs.Create( (const char**)tabs, RGB565(8080d0), this );
	m_proValue.Create( &m_nValue, 0, 100 );
	m_itmValue.Create( "File nr.", CWnd::WsVisible, &m_proValue, this );
	m_itmSpace1.Create( "", CWnd::WsVisible | CWnd::WsNoActivate, this);
	m_itmFile.Create( "    WAVE000.DAT", CWnd::WsVisible | CWnd::WsNoActivate, this);
//	m_itmSpace2.Create( "", CWnd::WsVisible | CWnd::WsNoActivate, this);
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
	m_bRunning = true;
	
	OnMessage( &m_itmTabs, ToWord('u', 'p'), NULL );
	StartModal( &m_itmValue );
	
	while ( IsRunning() )
	{
		Sleep(20);
	}
	StopModal();
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
	char strName[13];

	if ( pSender == &m_itmTabs && code == ToWord('u', 'p') )
	{
		// redirect to next function for redrawing the listbox
		pSender = &m_itmValue;
		// find nearest non used file
		for ( int i=0; i<10; i++ )
		{
			BIOS::DBG::sprintf( strName, strTemplateFile[m_itmTabs.GetFocus()], i );
			if ( !Exists(strName) )
			{
				m_nValue = i;
				break;
			}
		}
	}

	if ( pSender == &m_itmValue && code == ToWord('u', 'p') )
	{
		BIOS::DBG::sprintf( m_strCurrent, strTemplateDisplay[m_itmTabs.GetFocus()], m_nValue );
		m_itmFile.m_pszId = m_strCurrent;
		m_itmFile.Invalidate();
		
		BIOS::DBG::sprintf( strName, strTemplateFile[m_itmTabs.GetFocus()], m_nValue );
		bool bExists = Exists(strName);
		// set the flag even when the window is not displayed yet (instead of .IsVisible())
		if ( bExists != (m_itmLoad.m_dwFlags & CWnd::WsVisible) )
		{
			m_itmLoad.ShowWindow( bExists ? CWnd::SwShow : CWnd::SwHide );
			Invalidate();
		} 
	}
	if ( code == ToWord('l', 'e') && data == (NATIVEPTR)&m_proLoad )
	{
		// load
		BIOS::DBG::sprintf( strName, strTemplateFile[m_itmTabs.GetFocus()], m_nValue );

		switch ( m_itmTabs.GetFocus() )
		{
			case 0:	// Wave
				CImport::LoadBinary( strName );
				{
					CRect rcSafe = m_rcOverlay;
					MainWnd.Invalidate(); // to redraw the graph
					m_rcOverlay = rcSafe;
				}
				//Invalidate(); // Why it forgets to redraw current window!?
				break;
			case 1: // Bmp
			case 2: // Csv
			case 3: // Svg
				MainWnd.m_wndMessage.Show(&MainWnd, "Sorry...", "This feature is not\nimplemented yet", RGB565(ffff00));
				break;
		}
		// load
	}
	if ( code == ToWord('l', 'e') && data == (NATIVEPTR)&m_proSave )
	{
		// save
		BIOS::DBG::sprintf( strName, strTemplateFile[m_itmTabs.GetFocus()], m_nValue );

		switch ( m_itmTabs.GetFocus() )
		{
			case 0:	// Wave
				CExport::SaveBinary( strName );
				MainWnd.m_wndMessage.Show(&MainWnd, "Information", "Successfully saved", RGB565(ffff00));
				break;
			case 1: // Bmp
				CExport::SaveScreenshot( strName );
				MainWnd.m_wndMessage.Show(&MainWnd, "Information", "Successfully saved", RGB565(ffff00));
				break;
			case 2: // Csv
				CExport::SaveCsv( strName );
				MainWnd.m_wndMessage.Show(&MainWnd, "Information", "Successfully saved", RGB565(ffff00));
				break;
			case 3: // Svg
				MainWnd.m_wndMessage.Show(&MainWnd, "Sorry...", "This feature is not implemented yet", RGB565(ffff00));
				break;
		}
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
