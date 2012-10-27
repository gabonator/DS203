#ifndef __MENUSPECTMAIN_H__
#define __MENUSPECTMAIN_H__

#include <Source/Core/Controls.h>
#include <Source/Core/ListItems.h>
#include <Source/Core/Settings.h>
#include <Source/Gui/Oscilloscope/Controls/GraphBase.h>
#include <Source/Gui/Oscilloscope/Input/ItemAnalog.h>

class CWndMenuSpectMain : public CWnd
{
public:
	// Menu items
	CItemAnalog		m_itmCH1;
	CItemAnalog		m_itmCH2;
	CWndMenuItem	m_itmWindow;

	CWndMenuSpectMain();

	virtual void Create(CWnd *pParent, ui16 dwFlags);
	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);
};

#endif