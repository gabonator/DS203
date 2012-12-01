#ifndef __ITEMBEEPONOFF_H__
#define __ITEMBEEPONOFF_H__

class CItemBeepOnOff : public CItemProvider
{
	CProviderEnum	m_proBeepOnOff;

public:
	void Create( CWnd* pParent )
	{
		m_proBeepOnOff.Create( (const char**)CSettings::CRuntime::ppszTextBeepOnOff,
			(NATIVEENUM*)&Settings.Runtime.m_Beep, CSettings::CRuntime::_BeepMax );
		CItemProvider::Create( &m_proBeepOnOff, "Beep on keys", RGB565(808080), pParent );
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( ( nKey & BIOS::KEY::KeyLeft ) || ( nKey & BIOS::KEY::KeyRight ) )
		{
			CItemProvider::OnKey( nKey );
			Settings.Kick();
			return;
		}
		CItemProvider::OnKey( nKey );
	}
};

#endif