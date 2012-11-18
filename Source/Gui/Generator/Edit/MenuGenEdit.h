#ifndef __GENERATOR_MENUMAINEDIT_H__
#define __GENERATOR_MENUMAINEDIT_H__

#include <Source/Gui/Generator/Core/CoreGenerator.h>
#include <Source/Core/Settings.h>
#include <Source/Framework/Wnd.h>
#include <Source/Core/ListItems.h>

class CWndMenuGeneratorEdit : public CWnd
{
public:
	CWndMenuItem	m_itm[5];

	virtual void Create(CWnd *pParent, ui16 dwFlags);
	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);
};

#endif