#include "Monitor.h"
#ifdef ENABLE_MONITOR

#include <Source/Core/Settings.h>
#include <Source/Core/Utils.h>

/*static*/ const char* const CWndUserMonitor::m_ppszTextBaud[] =
	{"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"};

/*static*/ const int CWndUserMonitor::m_pnValueBaud[] =
	{1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};

void CWndUserMonitor::Create(CWnd *pParent, ui16 dwFlags)
{
	CWnd::Create("CWndCharMap", dwFlags | CWnd::WsTick, CRect(300, 20, 400, 240), pParent);
	m_proBaud.Create( (const char**)m_ppszTextBaud,
		(NATIVEENUM*)&m_nBaud, COUNT(m_ppszTextBaud)-1 );

	m_itmBaud.Create("Baudrate", RGB565(808080), &m_proBaud, this);
	m_itmFormat.Create("~Format\n8-N-1", RGB565(808080), 2, this);
	m_btnClear.Create("Clear", RGB565(8080ff), 2, this);
//	m_btnSend.Create("\"Hello\"", RGB565(8080ff), 2, this);

	m_nBaud = 7;
}

void CWndUserMonitor::OnTick()
{
	int ch;
	bool nLastCrLf = false;

	while ( (ch = BIOS::SERIAL::Getch()) >= 0 )
	{
		if ( ch == 0x0d || ch == 0x0a ) // cr
		{
			if ( nLastCrLf )
				continue;
			nLastCrLf = true;

			m_x = 0;
			if ( ++m_y >= 12 )
			{
				m_y = 0;
				BIOS::LCD::Bar( 4, 24, 4+32*8, 24 + 12*16, RGB565(000000) );
			}		
			continue;
		} else
			nLastCrLf = false;

		int x = m_x * 8 + 4;
		int y = m_y * 16 + 24;
		char str[2] = {(char)ch, 0};
		BIOS::LCD::Print(x, y, RGB565(ffffff), RGB565(000000), str);
		if ( ++m_x >= 32 )
		{
			m_x = 0;
			if ( ++m_y >= 12 )
			{
				m_y = 0;
				BIOS::LCD::Bar( 4, 24, 4+32*8, 24 + 12*16, RGB565(000000) );
			}		
		}	
	}
}

void CWndUserMonitor::OnTimer()
{
	static ui8 nCounter = 0;
	nCounter++;
	BIOS::LCD::Print(m_x*8 + 4, m_y*16 + 24, (nCounter & 1) ? RGB565(ffffff) : RGB565(000000), RGB565(000000), "_");
}

void CWndUserMonitor::OnPaint()
{
	//BIOS::LCD::Bar( m_rcClient, RGB565(ffffff) );
}

void CWndUserMonitor::OnKey(ui16 nKey)
{
}

void CWndUserMonitor::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	if ( code == ToWord('t', 'i') )
	{
		OnTick();
		return;
	}

	// LAYOUT ENABLE/DISABLE FROM TOP MENU BAR
	if (code == ToWord('L', 'D') )
	{
		KillTimer();
		return;
	}

	if (code == ToWord('L', 'E') )
	{
		m_x = 0;
		m_y = 0;
		SetTimer(500);
		return;
	}
	
	if ( pSender == &m_itmBaud && code == ToWord('u', 'p') )
	{
		int nBaudrate = m_pnValueBaud[m_nBaud];
		BIOS::SERIAL::Configure( nBaudrate );
	}

	if ( pSender == &m_btnClear && code == CWnd::WmKey && data == BIOS::KEY::KeyEnter )
	{
		m_x = 0;
		m_y = 0;
		BIOS::LCD::Bar( 4, 24, 4+32*8, 24 + 12*16, RGB565(000000) );
	}
	/*
	if ( pSender == &m_btnSend && code == CWnd::WmKey && data == BIOS::KEY::KeyEnter )
	{
		BIOS::SERIAL::Send("Hello");
	}
	*/
}

#endif