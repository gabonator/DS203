#ifndef __ABOUT_H__
#define __ABOUT_H__

#include <Source/Framework/Wnd.h>

class CWndAboutDevice : public CWnd
{
public:
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndAboutDevice", dwFlags | CWnd::WsNoActivate, CRect(0, 16, 400, 240), pParent);
	}

#if 0
	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{	
  		if (code == ToWord('L', 'D') )
  		{
				KillTimer();
  		}

  		if (code == ToWord('L', 'E') )
  		{
				SetTimer(20);
  		}
	}

	virtual void OnTimer()
	{
		int nTemp = BIOS::SYS::GetTemperature();
		float fTemp = (1.42f - nTemp*2.8f/4096)*1000/4.35f + 25;

		static int nAvg = 0;
		static int nAvg2 = 0;
		static int nAvg3 = 0;

		if ( nAvg3 == 0 )
			nAvg3 = nAvg2;
		nAvg3 = (nAvg3*220 + nAvg2*(256-220))/256;

		if ( nAvg2 == 0 )
			nAvg2 = nAvg;
		nAvg2 = (nAvg2*220 + nAvg*(256-220))/256;

		if ( nAvg == 0 )
			nAvg = nTemp*256;
		nAvg = (nAvg*220 + nTemp*256*(256-220))/256;

		//float fTemp2 = (1.42f - nAvg2/256.0f*2.8f/4096)*1000/4.35f + 25;
		//float fTemp3 = (1.42f - nAvg3/256.0f*2.8f/4096)*1000/4.35f + 25;


		static int nx = 0;
		int ycur = (nTemp-1800)*240/300;
		int yavg = (nAvg3/256-1800)*240/300;

		UTILS.Clamp<int>( ycur, 0, 240-1);
		UTILS.Clamp<int>( yavg, 0, 240-1);


		BIOS::LCD::PutPixel(nx/10, ycur, RGB565(0000ff));
		BIOS::LCD::PutPixel(nx/10, yavg, RGB565(00ff00));
		if (++nx >= 400*10 )
		{
			nx = 0;
			BIOS::LCD::Bar( m_rcClient, 0 );			
		}
		fTemp = 3.2f - (nAvg3/256 - 2169)*(21.3f-3.2f)/(2169-2068);
//		fTemp += 22 + 19;
//		int nA = *((ui8*)0x1ff8007a);
//		int nB = *((ui8*)0x1ff8007e);
		BIOS::LCD::Printf(20, 20, RGB565(ff0000), 0, "Temp=%d %d %f    ", nTemp, nAvg3/256, fTemp);
//		BIOS::LCD::Printf(20, 36, RGB565(ff0000), 0, "%d %d", nA, nB);
	}
#endif

	virtual void OnPaint()
	{
			const ui16 clrB = RGB565(b0b0b0);
			const ui16 clrA = RGB565(808080);

			BIOS::LCD::Bar( m_rcClient, RGB565(000000) );


			BIOS::LCD::Print (   4, 240-7*16, clrA, 0, "Hardware version:" );
			BIOS::LCD::Print ( 160, 240-7*16, clrB, 0, BIOS::VER::GetHardwareVersion() );

			BIOS::LCD::Print (   4, 240-6*16, clrA, 0, "System version:" );
			BIOS::LCD::Print ( 160, 240-6*16, clrB, 0, BIOS::VER::GetSystemVersion() );

			BIOS::LCD::Print (   4, 240-5*16, clrA, 0, "FPGA version:" );
			BIOS::LCD::Print ( 160, 240-5*16, clrB, 0, BIOS::VER::GetFpgaVersion() );

			BIOS::LCD::Print (   4, 240-4*16, clrA, 0, "DFU version:" );
			BIOS::LCD::Print ( 160, 240-4*16, clrB, 0, BIOS::VER::GetDfuVersion() );

			ui32 dwDriver = BIOS::VER::GetDisplayType();

			BIOS::LCD::Print (   4, 240-3*16, clrA, 0, "Display driver:" );
			switch (dwDriver)
			{
				case ToDword('v', 'g', 'a', 0):
					BIOS::LCD::Print ( 160, 240-3*16, clrB, 0, "VGA" );
					break;

				case 0x02049327:
					BIOS::LCD::Print ( 160, 240-3*16, clrB, 0, "ILI9327" );
					break;

				case 0x00000000:
					BIOS::LCD::Print ( 160, 240-3*16, clrB, 0, "R61509V" );

				default:
					BIOS::LCD::Printf( 160, 240-3*16, clrB, 0, "%08x", dwDriver );
			}

			BIOS::LCD::Print (   4, 240-2*16, clrA, 0, "Serial number:" );
			BIOS::LCD::Printf( 160, 240-2*16, clrB, 0, "%08x", BIOS::VER::GetSerialNumber() );

			// logo 256x64
			BIOS::VER::DrawLogo((m_rcClient.Width()-256)/2, m_rcClient.top + 8 + 16 );
	}
};

class CWndAboutFirmware : public CWnd
{
public:
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{                        
		CWnd::Create("CWndAboutFw", dwFlags | CWnd::WsNoActivate, CRect(0, 16, 400, 240), pParent);
	}
	
	virtual void OnPaint()
	{
			const ui16 clrB = RGB565(b0b0b0);
			const ui16 clrA = RGB565(808080);

			BIOS::LCD::Bar( m_rcClient, RGB565(000000) );

			BIOS::LCD::Print (   4, 240-7*16, clrA, 0, "Firmware version:" );
#ifdef GIT_REVISION
			BIOS::LCD::Printf( 160, 240-7*16, clrB, 0, "1.0 rev. %d", GIT_REVISION );
#else
			BIOS::LCD::Print ( 160, 240-7*16, clrB, 0, "1.0" );
#endif

#ifdef GIT_HASH
			BIOS::LCD::Print (   4, 240-6*16, clrA, 0, "Git hash:" );
			BIOS::LCD::Print ( 160, 240-6*16, clrB, 0, GIT_HASH );
#endif

#ifdef GIT_BUILDER
			BIOS::LCD::Print (   4, 240-5*16, clrA, 0, "Built by:" );
			BIOS::LCD::Print ( 160, 240-5*16, clrB, 0, GIT_BUILDER );
#endif

			BIOS::LCD::Print (   4, 240-4*16, clrA, 0, "Firmware built:" );
			BIOS::LCD::Print ( 160, 240-4*16, clrB, 0, __DATE__ " " __TIME__ );

			BIOS::LCD::Print (   4, 240-3*16, clrA, 0, "Compiler version:" );
#ifdef __GNUC__
			BIOS::LCD::Printf( 160, 240-3*16, clrB, 0, "GCC %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__ );
#endif
#ifdef _WIN32
			BIOS::LCD::Printf( 160, 240-3*16, clrB, 0, "MSVC %d", _MSC_VER );
#endif

			// logo 256x64
			BIOS::VER::DrawLogo((m_rcClient.Width()-256)/2, m_rcClient.top + 8 + 16 );
	}
};

#endif