#ifndef __LISTMARKER_H__
#define __LISTMARKER_H__

#include "Source/Core/Controls.h"
#include "Source/Core/ListItems.h"
#include "Source/Core/Settings.h"

class CWndListMarker : public CListBox
{
public:
	CSettings::Marker* m_pMarker;

	CProviderEnum	m_proMode;
	CProviderEnum	m_proSource;
	CProviderEnum	m_proDisplay;
	CProviderNumAny<int>	m_proValueRaw;

	CLPItem			m_itmMode;
	CLPItem			m_itmSource;
	CLPItem			m_itmDisplay;
	CLPItem			m_itmValueRaw;
	CLStaticItem	m_itmValue;

	CSettings::Marker::EFind m_modeFind;
	CProviderEnum	m_proFind;
	CLPItem			m_itmFind;

	CProviderBtn	m_proSetView;
	CLPItem			m_itmSetView;

public:
	CWndListMarker();
	void Create( CSettings::Marker* pMarker, CWnd* pParent );
	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);
};

#endif