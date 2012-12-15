#ifndef __MENUMASK_H__
#define __MENUMASK_H__

#include <Source/Core/Controls.h>
#include <Source/Core/ListItems.h>
#include <Source/Core/Settings.h>
#include <Source/Gui/Oscilloscope/Controls/GraphOsc.h>
#include "../Disp/ItemDisp.h"

class CMIButton : public CWndMenuItem
{
public:
	virtual void Create(const char* pszId, ui16 clr, CWnd *pParent) 
	{
		CWndMenuItem::Create( pszId, clr, 2, pParent);
	}

	virtual void OnPaint()
	{
		CWndMenuItem::OnPaint();
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & ( BIOS::KEY::KeyEnter | BIOS::KEY::KeyLeft | BIOS::KEY::KeyRight ) )
			SendMessage( GetParent(), CWnd::WmKey, nKey );
		CWnd::OnKey( nKey );
	}
};

class CWndMenuMask : public CWnd
{
public:
	enum EAction 
	{
		ActionNone = 0,
		ActionBeep = 1,
		ActionBeepStop = 2,
		ActionStop = 3,
		ActionMax = ActionStop
	};
	static const char* const m_ppszTextAction[];

public:
	// Menu items
	CMIButton	m_btnSource;
	CMIButton	m_btnReset;
	CMIButton	m_btnCollect;
	CMIButton	m_btnStop;
	//CMIButton	m_btnAction;
	CMIButton	m_btnExpand;
	CMIButton	m_btnBlur;
	CMPItem		m_itmAction;
	CProviderEnum	m_proAction;

	EAction		m_Action;

	virtual void		Create(CWnd *pParent, ui16 dwFlags);
	virtual void		OnMessage(CWnd* pSender, ui16 code, ui32 data);
};

#endif