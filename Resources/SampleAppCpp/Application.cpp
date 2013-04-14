#include <Source/HwLayer/Types.h>
#include <Source/HwLayer/Bios.h>
#include "Application.h"
//#include "Test.h"
#include <Source/Framework/Wnd.h>
//#include <Source/Core/Settings.h>
//#include <Source/Core/Utils.h>


#define ADD_MODULE(name, class)
#define ENABLE_MODULE_DCF77
#include "Snake.h"
//#include "Dcf77.h"           

/*
// TODO: fix these
CApplicationProto::~CApplicationProto() {}
void CApplicationProto::Create() {}
void CApplicationProto::Destroy() {}
bool CApplicationProto::operator ()() { return false; }
*/
bool HasOverlay()
{
	return false;
}

class CMainWnd : public CWnd
{
public:
//	CSettings m_Settings;
	CWndSnake m_wndSnake;
//	CWndDcf77 m_wndDcf;
	CWnd*	m_pCurrent;

	void Create()
	{
//		m_Settings.Load();
		CWnd::Create("CMainWnd", CWnd::WsVisible, CRect(0, 0, 400, 240), NULL);
		m_wndSnake.Create( this, CWnd::WsHidden );
//		m_wndDcf.Create( this, CWnd::WsHidden );

		m_pCurrent = &m_wndSnake;
		m_pCurrent->ShowWindow( CWnd::SwShow );
		m_pCurrent->OnMessage( NULL, ToWord('L', 'E'), 0 );
		m_pCurrent->SetFocus();
	}	
	void Destroy()
	{
		_ASSERT( m_pCurrent );
		m_pCurrent->OnMessage( NULL, ToWord('L', 'D'), 0 );
		SetFocus();
	}
};

class CKeyProcessor
{
	ui16 keys;
	ui32 last;
	ui16 delay;
public:
	CKeyProcessor()
	{
		last = BIOS::SYS::GetTick();
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
		ui32 now = BIOS::SYS::GetTick();
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

CMainWnd m_wndMain;
CKeyProcessor m_kp;

CApplication::CApplication()
{
}

CApplication::~CApplication()
{
}

void CApplication::Create()
{
	m_wndMain.Create();
	m_wndMain.WindowMessage( CWnd::WmPaint );
}

void CApplication::Destroy()
{
	m_wndMain.Destroy();
}

bool CApplication::operator ()()
{
	static ui16 lLastKeys = (ui16)-1;

	ui16 nKeys = BIOS::KEY::GetKeys();
	if(!nKeys)
	{
		lLastKeys = 0;
	}

	m_kp << nKeys;
	m_kp >> nKeys;

	if ( nKeys )
	{
		m_wndMain.WindowMessage( CWnd::WmKey, nKeys );
		lLastKeys = nKeys;
	}

	m_wndMain.WindowMessage( CWnd::WmTick, 0 );

#ifdef _WIN32
	Sleep(1);
#endif
	return !(nKeys & BIOS::KEY::KeyEscape);
	//return true;
}
