#ifndef __ITEMBEEPONOFF_H__
#define __ITEMBEEPONOFF_H__

class CItemBeepOnOff : public CItemProvider
{
	CProviderEnum	m_proBeepOnOff;

public:
	void Create( CWnd* pParent )
	{
		m_proBeepOnOff.Create( (const char**)CSettings::CRuntime::ppszTextBeepOnOff,
			(NATIVEENUM*)&Settings.Runtime.Beep, CSettings::CRuntime::_BeepMax );
		CItemProvider::Create( &m_proBeepOnOff, "Beep on keys", RGB565(808080), pParent );
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( nKey & BIOS::KEY::KeyLeft || BIOS::KEY::KeyRight)
		{
			Settings.Save();
		}
		CItemProvider::OnKey( nKey );
	}
};

#endif