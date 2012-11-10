#ifndef __MENUSPECTMAIN_H__
#define __MENUSPECTMAIN_H__

#include <Source/Core/Controls.h>
#include <Source/Core/ListItems.h>
#include <Source/Core/Settings.h>
#include <Source/Gui/Oscilloscope/Controls/GraphBase.h>
#include <Source/Gui/Oscilloscope/Input/ItemAnalog.h>
#include <Source/Gui/Oscilloscope/Input/ItemTime.h>

#include "ItemWindow.h"
#include "ItemDisplay.h"

class CWndMenuSpectMain : public CWnd
{
public:
	// Menu items
	CItemAnalog		m_itmCH1;
	CItemAnalog		m_itmCH2;
	CItemTime		m_itmTime;

	CItemSpecWindow	m_itmWindow;
	CItemSpecDisplay	m_itmDisplay;
	CWndMenuItem	m_itmMarker;

	CWndMenuSpectMain();

	virtual void Create(CWnd *pParent, ui16 dwFlags);
	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);
};

#endif