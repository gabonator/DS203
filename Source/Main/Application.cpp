#include "Application.h"
#include <Source/Gui/MainWnd.h>

/*static*/ CApplication* CApplication::m_pInstance = NULL;

class CKeyProcessor
{
	ui16 keys;
	ui32 last;
	ui16 delay;
public:
	CKeyProcessor()
	{
		last = BIOS::GetTick();
		delay = 301;
	}
	void operator <<(const ui16& in)
	{
		if (keys == 0)
			delay = 301;
		keys = in;
	}
	void operator >>(ui16& out)
	{
		ui32 now = BIOS::GetTick();
		if (now-last > delay && keys)
		{
			out = keys;
			last = now;
			if ( delay == 301 )
				delay = 300;
			else if ( delay == 300 )
				delay = 100;
			else if (delay > 10)
				delay -= 10;
		}
		else
			out = 0;
	}
};

/* workaround class for the issue of global classes constructors being not called */

#define GLOBAL (*CInit::getInstance())

class CInit
{
public:
	CMainWnd m_wndMain;
	CSettings m_Settings;
	CKeyProcessor m_kp;

	static CInit* getInstance()
	{
		static CInit init;
		return &init;
	}
};

CApplication::CApplication()
{
	m_pInstance = this;

	BIOS::Init();

	GLOBAL.m_wndMain.Create();
	GLOBAL.m_wndMain.WindowMessage( CWnd::WmPaint );

	GLOBAL.m_wndMain.m_wndMenuInput.ConfigureAdc();
	GLOBAL.m_wndMain.m_wndMenuInput.ConfigureTrigger();
	BIOS::ADC::Restart();
}

CApplication::~CApplication()
{
}

bool CApplication::operator ()()
{
	static ui32 lLastTick = (ui32)-1;
	ui32 lCurTick = BIOS::GetTick();
	if ( lLastTick == (ui32)-1 )
		lLastTick = lCurTick;
	if ( lCurTick - lLastTick > 1000 )
	{
		int nSeconds = (lCurTick - lLastTick) / 1000;
		lLastTick += nSeconds * 1000;
		GLOBAL.m_Settings.Runtime.m_nUptime += nSeconds;
	}

	ui16 nKeys = BIOS::KEY::GetKeys();

	GLOBAL.m_kp << nKeys;
	GLOBAL.m_kp >> nKeys;

	if ( nKeys )
		GLOBAL.m_wndMain.WindowMessage( CWnd::WmKey, nKeys );
	if ( BIOS::ADC::Ready() )
	{
		GLOBAL.m_wndMain.WindowMessage( CWnd::WmBroadcast, ToWord('d', 'g') );
		BIOS::ADC::Restart();
	}
	GLOBAL.m_wndMain.WindowMessage( CWnd::WmTick, 0 );
#ifdef _WIN32
	Sleep(1);
#endif
	return true;
}

/*static*/ CApplication* CApplication::getInstance()
{
	return m_pInstance;	
}