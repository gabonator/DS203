#ifndef __MENUSPECTMARKER_H__
#define __MENUSPECTMARKER_H__

#include <Source/Core/Controls.h>
#include <Source/Core/ListItems.h>
#include <Source/Core/Settings.h>
#include "ItemMarker.h"

class CWndMenuSpectMarker : public CWnd
{
public:
	// Menu items
	CWndMenuItem m_itmSource;
	CWndMenuItem m_itmTrack;
	CItemSpecMarker m_itmValue;

	CWndMenuSpectMarker();

	virtual void Create(CWnd *pParent, ui16 dwFlags);
	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);
};

#endif