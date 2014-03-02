#include "ListMarker.h"

#include <Source/Gui/MainWnd.h>

CWndListMarker::CWndListMarker()
{
	m_modeFind = CSettings::Marker::_MaxFind;
}

void CWndListMarker::Create( CSettings::Marker* pMarker, CWnd* pParent )
{
	m_pMarker = pMarker;
	CListBox::Create( pMarker->strFullName, WsVisible | WsModal, CRect(100, 30, 300, 201), pMarker->u16Color, pParent );

	if ( pMarker->Type == CSettings::Marker::_Time )
	{
		m_proMode.Create( (const char**)CSettings::Marker::ppszTextMode,
			(NATIVEENUM*)&pMarker->Mode, CSettings::Marker::_ModeMaxTime );
	} else
	{
		m_proMode.Create( (const char**)CSettings::Marker::ppszTextMode,
			(NATIVEENUM*)&pMarker->Mode, CSettings::Marker::_ModeMaxVoltage );
		m_proSource.Create( (const char**)CSettings::Marker::ppszTextSource,
			(NATIVEENUM*)&pMarker->Source, CSettings::Marker::_SourceMax );
		m_proFind.Create( (const char**)CSettings::Marker::ppszTextFind,
			(NATIVEENUM*)&m_modeFind, CSettings::Marker::_FindMax );
	}

	m_proDisplay.Create( (const char**)CSettings::Marker::ppszTextDisplay,
		(NATIVEENUM*)&pMarker->Display, CSettings::Marker::_DisplayMax );

	m_proValueRaw.Create( &pMarker->nValue, 0, 1000 );

	m_itmMode.Create( "Mode", CWnd::WsVisible, &m_proMode, this );	
	if ( pMarker->Type == CSettings::Marker::_Voltage )
		m_itmSource.Create( "Source", CWnd::WsVisible, &m_proSource, this );
	m_itmDisplay.Create( "Display", CWnd::WsVisible, &m_proDisplay, this );
	m_itmValueRaw.Create( "Raw value", CWnd::WsVisible, &m_proValueRaw, this );
	m_itmValue.Create( "Real value    0.000", CWnd::WsVisible | CWnd::WsNoActivate, this);
	if ( pMarker->Type == CSettings::Marker::_Voltage )
		m_itmFind.Create( "Find", CWnd::WsVisible, &m_proFind, this );

	m_proSetView.Create( "View" );
	m_itmSetView.Create( "Set range", CWnd::WsVisible, &m_proSetView, this );
}

void CWndListMarker::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	if ( pSender == &m_itmSetView && code == ToWord('l', 'e') )	// provider selector combo box
	{
		int nBegin, nEnd;
		MainWnd.m_wndGraph.GetCurrentRange( nBegin, nEnd );
		Settings.MarkT1.nValue = nBegin;
		Settings.MarkT2.nValue = nEnd;
		return;
	}
	CListBox::OnMessage( pSender, code, data );
}
