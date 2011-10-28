#include "Application.h"
#include <Source/Gui/MainWnd.h>

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

// Arm M3
void Run()
{
	BIOS::Init();
	//ADC::Configure();

	CSettings m_Settings;
	CMainWnd m_wndMain;
	m_wndMain.Create();
	m_wndMain.WindowMessage( CWnd::WmPaint );

	m_wndMain.m_wndMenuInput.ConfigureAdc();
	m_wndMain.m_wndMenuInput.ConfigureTrigger();
	BIOS::ADC::Restart();
	CKeyProcessor kp;

	while (1)
	{
		ui16 nKeys = BIOS::KEY::GetKeys();
		
		kp << nKeys;
		kp >> nKeys;

		if ( nKeys )
			m_wndMain.WindowMessage( CWnd::WmKey, nKeys );
		if ( BIOS::ADC::Ready() )
		{
			m_wndMain.OnMessage(NULL, ToWord('d', 'g'), 0);
			BIOS::ADC::Restart();
		}
	}
}

// Win32
CMainWnd m_wndMain;
CSettings m_Settings;
CKeyProcessor kp;

CApplication::CApplication()
{
	BIOS::LCD::Print(100, 100, 0xff00, 0, __DATE__);
	BIOS::LCD::Print(100, 120, 0xff00, 0, __TIME__);
}

CApplication::~CApplication()
{
}

void CApplication::operator ()()
{
	static ui8 bInit = 1;
	if (bInit)
	{
		bInit = 0;
		BIOS::Init();
		m_wndMain.Create();
		m_wndMain.WindowMessage( CWnd::WmPaint );
		m_wndMain.m_wndMenuInput.ConfigureAdc();
	}
	
	ui16 nKeys = BIOS::KEY::GetKeys();
	
	kp << nKeys;
	kp >> nKeys;

	if ( nKeys )
		m_wndMain.WindowMessage( CWnd::WmKey, nKeys );
	// if (m_wndMain.m_dwFlags & CWnd::WmNeedUpdate) m_wndMain.Update();
	if ( BIOS::ADC::Ready() )
	{
		m_wndMain.OnMessage(NULL, ToWord('d', 'g'), 0);
		BIOS::ADC::Restart();
	}
}
