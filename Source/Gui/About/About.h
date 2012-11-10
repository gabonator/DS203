#ifndef __ABOUT_H__
#define __ABOUT_H__

#include <Source/Framework/Wnd.h>

class CWndAbout : public CWnd
{
public:
	CWndAbout()
	{
	}
	
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndAbout", dwFlags | CWnd::WsNoActivate, CRect(0, 16, 400, 240), pParent);
	}
	
	virtual void OnPaint()
	{
			const ui16 clrB = RGB565(b0b0b0);
			const ui16 clrA = RGB565(808080);

			BIOS::LCD::Bar( m_rcClient, RGB565(000000) );

			// logo 256x64
			BIOS::VER::DrawLogo((m_rcClient.Width()-256)/2, m_rcClient.top + 8 + 16 );

//			ui32 dwVersion = _Version;
//			char* strVer = (char*)dwVersion;

//			BIOS::LCD::Print (   4, 240-8*16, clrA, 0, "Firmware version:" );
//			BIOS::LCD::Printf( 160, 240-8*16, clrB, 0, "%c%c%c%c", strVer[3], strVer[2], strVer[1], strVer[0] );

			BIOS::LCD::Print (   4, 240-7*16, clrA, 0, "Firmware built:" );
			BIOS::LCD::Print ( 160, 240-7*16, clrB, 0, __DATE__ " " __TIME__ );

			BIOS::LCD::Print (   4, 240-6*16, clrA, 0, "Hardware version:" );
			BIOS::LCD::Print ( 160, 240-6*16, clrB, 0, BIOS::VER::GetHardwareVersion() );

			BIOS::LCD::Print (   4, 240-5*16, clrA, 0, "System version:" );
			BIOS::LCD::Print ( 160, 240-5*16, clrB, 0, BIOS::VER::GetSystemVersion() );

			BIOS::LCD::Print (   4, 240-4*16, clrA, 0, "FPGA version:" );
			BIOS::LCD::Print ( 160, 240-4*16, clrB, 0, BIOS::VER::GetFpgaVersion() );

			BIOS::LCD::Print (   4, 240-3*16, clrA, 0, "DFU version:" );
			BIOS::LCD::Print ( 160, 240-3*16, clrB, 0, BIOS::VER::GetDfuVersion() );

			BIOS::LCD::Print (   4, 240-2*16, clrA, 0, "Serial number:" );
			BIOS::LCD::Printf( 160, 240-2*16, clrB, 0, "%08x", BIOS::VER::GetSerialNumber() );
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		// LAYOUT ENABLE/DISABLE FROM TOP MENU BAR
		if (code == ToWord('L', 'D') )
		{
			return;
		}

		if (code == ToWord('L', 'E') )
		{
			return;
		}
	}
};

#endif