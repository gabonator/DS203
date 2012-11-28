#include "ItemBeepOnOff.h"
#include "ItemShortcut.h"

class CWndMenuKeySettings : public CWnd
{
public:
	CItemBeepOnOff	m_itmBeepOnOff;
	CProviderEnum	m_proShortcutTriangle;
	CItemShortcut	m_itmShortcutTriangle;
	CProviderEnum	m_proShortcutS1;
	CItemShortcut	m_itmShortcutS1;
	CProviderEnum	m_proShortcutS2;
	CItemShortcut	m_itmShortcutS2;

	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndMenuKeySettings", dwFlags, CRect(60, 60, 280, 240), pParent);

		m_itmBeepOnOff.Create(this);

		m_proShortcutTriangle.Create( (const char**)CSettings::CRuntime::ppszTextShortcut,
			(NATIVEENUM*)&Settings.Runtime.ShortcutTriangle, CSettings::CRuntime::_ShortcutMax );
		m_itmShortcutTriangle.Create(this,&m_proShortcutTriangle,"Triangle Button");

		m_proShortcutS1.Create( (const char**)CSettings::CRuntime::ppszTextShortcut,
			(NATIVEENUM*)&Settings.Runtime.ShortcutS1, CSettings::CRuntime::_ShortcutMax );
		m_itmShortcutS1.Create(this,&m_proShortcutS1,"S1 Button");

		m_proShortcutS2.Create( (const char**)CSettings::CRuntime::ppszTextShortcut,
			(NATIVEENUM*)&Settings.Runtime.ShortcutS2, CSettings::CRuntime::_ShortcutMax );
		m_itmShortcutS2.Create(this,&m_proShortcutS2,"S2 Button");
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		//if ( code == ToWord('m', 'o') && pSender == &m_itmReboot )
		//	BIOS::SYS::Execute( BIOS::SYS::EDfu );
	}
};
