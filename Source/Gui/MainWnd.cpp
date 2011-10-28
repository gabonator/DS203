#include "MainWnd.h"

/*static*/ CMainWnd	*CMainWnd::m_pInstance = NULL;

void CMainWnd::Create()
{
	m_pInstance = this;

	// image correctly uploaded in ROM ?
	_ASSERT( 
		CSettings::TimeBase::ppszTextResolution[CSettings::TimeBase::_1s][0] == '1' &&
		CSettings::TimeBase::ppszTextResolution[CSettings::TimeBase::_1s][1] == 's' &&
		CSettings::TimeBase::ppszTextResolution[CSettings::TimeBase::_1s][2] == 0 );

	CWnd::Create("CMainWnd", WsVisible, CRect(0, 0, BIOS::LCD::LcdWidth, BIOS::LCD::LcdHeight), NULL );
	m_wndToolBar.Create( this );
	m_wndGraph.Create( this, WsVisible | WsNoActivate );
	m_wndSignalGraph.Create( this, WsNoActivate );

	m_wndMenuInput.Create( this, WsVisible );
	m_wndMenuCursor.Create( this, WsHidden );
	m_wndMenuMeas.Create( this, WsHidden );
	m_wndMenuMath.Create( this, WsHidden );
	m_wndMenuSettings.Create( this, WsHidden );
	m_wndMenuDisplay.Create( this, WsHidden );
	m_wndMenuGenerator.Create( this, WsHidden );
	m_wndMenuTools.Create( this, WsHidden );
	m_wndZoomBar.Create( this, &m_wndGraph );
	m_wndLReferences.Create( this, WsVisible );
	m_wndTReferences.Create( this, WsVisible );

	m_wndToolBar.SetFocus();
	//m_wndMenuInput.m_itmCH1.SetFocus();
	//m_wndMenuGenerator.m_itmBpm.SetFocus();
	//OnMessage( &m_wndToolBar, ToWord('f', 'c'), 0 ); // force update
}

/*virtual*/ void CMainWnd::OnPaint()
{
	BIOS::LCD::Clear(RGB565(000000));
}

/*virtual*/ void CMainWnd::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	if ( pSender == &m_wndToolBar )
	{
		if ( code == ToWord('f', 'c') )	// focus changed
		{
			m_wndMenuInput.ShowWindow( ( data == 0 ) ? SwShow : SwHide );
			m_wndMenuCursor.ShowWindow( ( data == 1 ) ? SwShow : SwHide );
			m_wndMenuMeas.ShowWindow( ( data == 2 ) ? SwShow : SwHide );
			m_wndMenuMath.ShowWindow( ( data == 3 ) ? SwShow : SwHide );
			m_wndMenuDisplay.ShowWindow( ( data == 4 ) ? SwShow : SwHide );
			m_wndMenuGenerator.ShowWindow( ( data == 5 ) ? SwShow : SwHide );
			m_wndMenuSettings.ShowWindow( ( data == 6 ) ? SwShow : SwHide );
			m_wndMenuTools.ShowWindow( ( data == 7 ) ? SwShow : SwHide );

			m_wndGraph.ShowWindow( ( data != 5 ) ? SwShow : SwHide );
			m_wndZoomBar.ShowWindow( ( data != 5 ) ? SwShow : SwHide );
			m_wndLReferences.ShowWindow( ( data != 5 ) ? SwShow : SwHide );
			m_wndTReferences.ShowWindow( ( data != 5 ) ? SwShow : SwHide );
			m_wndSignalGraph.ShowWindow( ( data == 5 ) ? SwShow : SwHide );
			Invalidate();
		}
	}
	if (code == ToWord('d', 'g'))
	{
		if ( m_wndGraph.m_dwFlags & WsVisible )
			m_wndGraph.Invalidate();
	}
}

