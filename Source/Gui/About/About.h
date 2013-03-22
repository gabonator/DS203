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

#ifdef _VERSION2
			const char *strVersion = "2.0";
#else
			const char *strVersion = "1.0";
#endif

			BIOS::LCD::Bar( m_rcClient, RGB565(000000) );

			BIOS::LCD::Print (   4, 240-7*16, clrA, 0, "Firmware version:" );
#ifdef GIT_REVISION
			BIOS::LCD::Printf( 160, 240-7*16, clrB, 0, "%a rev. %d", strVersion, GIT_REVISION );
#else
			BIOS::LCD::Print ( 160, 240-7*16, clrB, 0, strVersion );
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

class CAverageFilter 
{
	int nAvg, nAvg2, nAvg3;

public:
	CAverageFilter()
	{
		Reset();
	}

	void Reset()
	{
		nAvg = 0;
		nAvg2 = 0;
		nAvg3 = 0;
	}

	int operator()(int nInput)
	{
		if ( nAvg3 == 0 )
			nAvg3 = nAvg2;
		nAvg3 = (nAvg3*220 + nAvg2*(256-220))/256;

		if ( nAvg2 == 0 )
			nAvg2 = nAvg;
		nAvg2 = (nAvg2*220 + nAvg*(256-220))/256;

		if ( nAvg == 0 )
			nAvg = nInput*256;
		nAvg = (nAvg*220 + nInput*256*(256-220))/256;
		
		return nAvg3/256;
	}
};

class CWndAboutStatus : public CWnd
{
	enum {
		USB_POWER = 5,
		V_BATTERY = 6
	};

public:
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{                        
		CWnd::Create("CWndAboutStatus", dwFlags | CWnd::WsNoActivate, CRect(0, 16, 400, 240), pParent);
	}
	
	virtual void OnPaint()
	{
		const ui16 clrA = RGB565(808080);
		const ui16 clrC = RGB565(404040);

		BIOS::LCD::Bar( m_rcClient, RGB565(000000) );

		BIOS::LCD::Print (   4, 240-12*16, clrA, 0, "Cold boot:" );
		BIOS::LCD::Print (   4, 240-11*16, clrA, 0, "Battery voltage:" );
		BIOS::LCD::Print (   4, 240-10*16, clrA, 0, "USB Powered:" );

		BIOS::LCD::Print (   8, 240-9*16, clrC, 0, "\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4"
			"\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4" );

		BIOS::LCD::Print (   4, 240-8*16, clrA, 0, "CPU Temperature (raw):" );
		BIOS::LCD::Print (   4, 240-7*16, clrA, 0, "CPU Temperature (filtered):" );
		BIOS::LCD::Print (   4, 240-6*16, clrA, 0, "CPU Temperature:" );

		BIOS::LCD::Print (   8, 240-4*16, clrC, 0, "\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4"
			"\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4" );

		BIOS::LCD::Print (   4, 240-3*16, clrA, 0, "CPU Voltage (raw):" );
		BIOS::LCD::Print (   4, 240-2*16, clrA, 0, "CPU Voltage (filtered):" );
		BIOS::LCD::Print (   4, 240-1*16, clrA, 0, "CPU Voltage:" );

		// logo 256x64
//		BIOS::VER::DrawLogo((m_rcClient.Width()-256)/2, m_rcClient.top + 8 + 16 );
	}	

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{	
  		if (code == ToWord('L', 'D') )
  		{
			KillTimer();
  		}

  		if (code == ToWord('L', 'E') )
  		{
			SetTimer(100);
  		}
	}

	virtual void OnTimer()
	{
		const ui16 clrB = RGB565(b0b0b0);
		int nTemperature = BIOS::SYS::GetTemperature();
		int nCoreVoltage = BIOS::SYS::GetCoreVoltage();

		static CAverageFilter fltTemp;
		int nFilteredTemp = fltTemp(nTemperature);
		BIOS::LCD::Printf( 240, 240-8*16, clrB, 0, "%d  ", nTemperature );
		if ( nFilteredTemp != 0 )
		{
			BIOS::LCD::Printf( 240, 240-7*16, clrB, 0, "%d  ", nFilteredTemp );

			// 21.3 -> 2068
			//  3.2 -> 2169
			float fTemp = 3.2f - (nFilteredTemp - 2169)*(21.3f-3.2f)/(2169-2068);
			BIOS::LCD::Printf( 240, 240-6*16, clrB, 0, "%2f \xf8" "C  ", fTemp+0.005f );
			fTemp = fTemp*1.8f + 32.0f;
			BIOS::LCD::Printf( 240, 240-5*16, clrB, 0, "%2f \xf8" "F  ", fTemp+0.005f );
		}

		static CAverageFilter fltCore;
		int nFilteredCore = fltCore(nCoreVoltage);
		BIOS::LCD::Printf( 240, 240-3*16, clrB, 0, "%d", nCoreVoltage );
		if ( nFilteredCore != 0 )
		{
			BIOS::LCD::Printf( 240, 240-2*16, clrB, 0, "%d", nFilteredCore );

			// 0 -> 0 
			// 1756 -> 2.8v
			float fVolt = nFilteredCore * 2.8f / 1756.0f;
			BIOS::LCD::Printf( 240, 240-1*16, clrB, 0, "%2f V  ", fVolt+0.005f );
		}

		float fBattery = BIOS::SYS::Get( V_BATTERY ) * 0.001f;
		int nUsbPower = BIOS::SYS::Get( USB_POWER );
		BIOS::LCD::Printf( 240, 240-11*16, clrB, 0, "%f V  ", fBattery );
		BIOS::LCD::Printf( 240, 240-10*16, clrB, 0, "%s  ", nUsbPower ? "Yes" : "No" );
		BIOS::LCD::Printf( 240, 240-12*16, clrB, 0, "%s  ", BIOS::SYS::IsColdBoot() ? "Yes" : "No");
	}
};

class CWndAboutModules : public CWnd
{
public:
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{                        
		CWnd::Create("CWndAboutModules", dwFlags | CWnd::WsNoActivate, CRect(0, 16, 400, 240), pParent);
	}
	
	virtual void OnPaint()
	{
			const ui16 clrB = RGB565(b0b0b0);
			const ui16 clrA = RGB565(808080);

			BIOS::LCD::Bar( m_rcClient, RGB565(000000) );

			const char *strName = NULL;

			int y = 40;
			for ( int i = BIOS::SYS::EApp1; i <= BIOS::SYS::EApp4; i++ )
			{
				strName = (const char*)BIOS::SYS::IdentifyApplication( i );
				if ( strName && strcmp(strName, "Unknown") == 0 )
					BIOS::LCD::Printf(   4, y, clrA, 0, "APP%d (valid vector table found)", i - BIOS::SYS::EApp1 + 1 );
				else if ( strName )
					BIOS::LCD::Printf(   4, y, clrA, 0, "APP%d (identification found at 0x%x)", i - BIOS::SYS::EApp1 + 1, (ui32)strName );
				else
					BIOS::LCD::Printf(   4, y, clrA, 0, "APP%d (identification not found)", i - BIOS::SYS::EApp1 + 1 );

/*
				ui32 dwAddr = 0;
				switch(i)
				{
					case BIOS::SYS::EApp1: dwAddr = 0x0800C000; break;
					case BIOS::SYS::EApp2: dwAddr = 0x08014000; break;
					case BIOS::SYS::EApp3: dwAddr = 0x0801C000; break;
					case BIOS::SYS::EApp4: dwAddr = 0x08024000; break;
				}
				ui32* pData = (ui32*)dwAddr;

				BIOS::LCD::Printf(4,y+32, 0xffff, 0, "%x:%x, %x, %x, %x", dwAddr, pData[0], pData[1], pData[2], pData[3]);
*/

				if (!strName)
					strName = "Not installed";

				char strNameShort[52] = {0};
				memcpy(strNameShort, strName, 51);
				char* pDelim = strstr(strNameShort, ";");
				if ( pDelim )
					*pDelim = 0;
				BIOS::LCD::Print(   4, y+16, clrB, 0, strNameShort );
				y += 48;
			}
	}
};


#endif