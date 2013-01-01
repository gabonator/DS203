#include "ItemBeepOnOff.h"
#include "ItemShortcut.h"
#include "Source/Gui/Toolbar.h"

class CWndMenuKeySettings : public CWnd
{
public:
	CItemBeepOnOff		m_itmBeepOnOff;
	CProviderShortcut	m_proShortcutCircle;
	CItemShortcut		m_itmShortcutCircle;
	CProviderShortcut	m_proShortcutTriangle;
	CItemShortcut		m_itmShortcutTriangle;
	CProviderShortcut	m_proShortcutS1;
	CItemShortcut		m_itmShortcutS1;
	CProviderShortcut	m_proShortcutS2;
	CItemShortcut		m_itmShortcutS2;

	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndMenuKeySettings", dwFlags, CRect(60, 40, 280, 240), pParent);

		m_itmBeepOnOff.Create(this);

		m_proShortcutCircle.Create( &Settings.Runtime.m_nShortcutCircle );
		m_itmShortcutCircle.Create( this, &m_proShortcutCircle, "Circle Button" );

		m_proShortcutTriangle.Create( &Settings.Runtime.m_nShortcutTriangle );
		m_itmShortcutTriangle.Create( this, &m_proShortcutTriangle, "Triangle Button" );

		m_proShortcutS1.Create( &Settings.Runtime.m_nShortcutS1 );
		m_itmShortcutS1.Create( this, &m_proShortcutS1, "S1 Button" );

		m_proShortcutS2.Create( &Settings.Runtime.m_nShortcutS2 );
		m_itmShortcutS2.Create( this, &m_proShortcutS2, "S2 Button" );
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		//if ( code == ToWord('m', 'o') && pSender == &m_itmReboot )
		//	BIOS::SYS::Execute( BIOS::SYS::EDfu );
	}
};
