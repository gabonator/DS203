#define ENABLE_MODULE_GPIOTEST
ADD_MODULE( "OneWire", CWndGpioTest )

#if !defined(__GPIO_H__) && defined(ENABLE_MODULE_GPIOTEST)
#define __GPIO_H__

#include <Source/Framework/Wnd.h>
#include <Source/Library/DS1820.h>
#include <Source/Library/DHT11.h>
#include <Source/Library/BMP085.h>

class CWndGpioTest : public CWnd
{
	CDS1820	m_Thermometer;
	CDHT11	m_Hygrometer;
	CBMP085	m_Barometer;
	bool		m_bFocus;

public:

	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndGpioTest", dwFlags | CWnd::WsNoActivate, CRect(0, 16, 400, 240), pParent);
		m_Thermometer.Create( CPin::PortA + 9 );
		m_Hygrometer.Create( CPin::PortA + 9 );
		m_Barometer.Wire.Create( CPin::PortA + 9, CPin::PortA + 10 );
	}

	void PrintBits(int x, int y, ui32 value)
	{
		for (int i=0; i<16; i++)
		{
			char str[2] = {'0', 0};
			value <<= 1;
			if ( value & 0x10000 )
				str[0] = '1';
			x += BIOS::LCD::Print(x, y, RGB565(ffffff), RGB565(000000), str);
		}
	}

	void Update()
	{
		if ( m_Barometer.begin() )
		{
			UpdateBmp085();
			return;
		}
		if ( m_Thermometer.Init() )
		{
			UpdateDs1820();
			return;
		}
		if ( m_Hygrometer.Init() )
		{
			UpdateDht11();
			return;
		}
		BIOS::LCD::Printf(20, 0x90, RGB565(ff0000), 0, "No device attached          ");
	}

	void UpdateBmp085()
	{
		BIOS::LCD::Printf(20, 0x90, RGB565(ffff00), 0, "I2C: BMP085 device found");

		int nP = m_Barometer.readRawPressure();
		int nT = m_Barometer.readRawTemperature();
		float fTemp = m_Barometer.readTemperature();
		float fAlt = m_Barometer.readAltitude(102250);
	  int nPressure = m_Barometer.readPressure();

		BIOS::LCD::Printf(20, 0xA0, RGB565(00ff00), 0, "RawP = %d RawT = %d  ", nP, nT);
		BIOS::LCD::Printf(20, 0xB0, RGB565(00ff00), 0, "Pressure = %2f hPa    ", nPressure/100.0f);
		BIOS::LCD::Printf(20, 0xC0, RGB565(00ff00), 0, "Temperature = %2f " "\xf8" "C  ", fTemp );
		BIOS::LCD::Printf(20, 0xD0, RGB565(00ff00), 0, "Altitude = %1f m  ", fAlt );
	}

	void UpdateDht11()
	{
		ui8* pRom = m_Hygrometer.GetBuffer();
		bool bRead = m_Hygrometer.Read();
		bool bCrc = m_Hygrometer.CheckCrc();

		ui16 clr = RGB565(ffffff);

		if ( !bRead )
			BIOS::LCD::Printf(20, 0x90, clr, 0, "DHT11 Read error");
		else
			BIOS::LCD::Printf(20, 0x90, clr, 0, "DHT11 Read ok!  ");

		BIOS::LCD::Printf(20, 0xA0, clr, 0, "DHT11 ROM = ");
		for (int i=0; i<5; i++)
			BIOS::LCD::Printf(20+12*8+i*24, 0xA0, clr, 0, "%c%c", 
				CUtils::tohex(pRom[i]>>4), CUtils::tohex(pRom[i] & 0xf));

		if ( !bRead )
			return;

		int nTemp = m_Hygrometer.GetTemperature();
		int nHumidity = m_Hygrometer.GetHumidity();

		BIOS::LCD::Printf(20, 0xB0, clr, 0, "CRC = %s    ", bCrc ? "Valid" : "Error" );
		BIOS::LCD::Printf(20, 0xC0, clr, 0, "Temperature = %d " "\xf8" "C  ", nTemp );
		BIOS::LCD::Printf(20, 0xD0, clr, 0, "Humidity    = %d%%   ", nHumidity );
	}

	void UpdateDs1820()
	{
		bool bInit = false, bConv = false, bCrc = false, bTemp = false, bRom = false;
		float fTemp = 0;
		ui8* pSP = m_Thermometer.GetScratchpad();
		ui16 clr;
		ui8 nFamilyCode = 0;
		bInit = m_Thermometer.Init();
		memset( pSP, 0, 9 );

		if ( bInit )
			bRom = m_Thermometer.ReadRom();

		if ( bRom )
		{
			if ( m_Thermometer.Crc(8) )
				nFamilyCode = pSP[0];
		}

		clr = (bRom && nFamilyCode) ? RGB565(ffff00) : RGB565(808080);
		BIOS::LCD::Printf(20, 0xA0, clr, 0, "ROM = ");
		for (int i=0; i<8; i++)
			BIOS::LCD::Printf(20+6*8+i*24, 0xA0, clr, 0, "%c%c", 
				CUtils::tohex(pSP[i]>>4), CUtils::tohex(pSP[i]&0xf));

		const char* strFamily = "Unknown";
		switch ( nFamilyCode )
		{
			case CDS1820::DS18S20_FAMILY_CODE: strFamily = "DS18S20"; break;
			case CDS1820::DS18B20_FAMILY_CODE: strFamily = "DS18B20"; break;
			case CDS1820::DS1822_FAMILY_CODE: strFamily = "DS1822"; break;
		}

		BIOS::LCD::Printf(20, 0xB0, clr, 0, "Family code = (%c%c) %s  ", 
			CUtils::tohex(nFamilyCode>>4), CUtils::tohex(nFamilyCode&0xf), strFamily);

		memset( pSP, 0, 9 );
		if ( bInit )
			bConv = m_Thermometer.Convert(nFamilyCode);
		if ( bConv )
			bCrc = m_Thermometer.Crc(9);
		if ( bCrc && nFamilyCode )
			bTemp = m_Thermometer.Temperature(fTemp, nFamilyCode);

		const char* strStatus = "Ok!";
		if (!bInit)
			strStatus = "Err:Init";
		else if (!bConv)        
			strStatus = "Err:Conv";
		else if (!bCrc)
			strStatus = "Err:Crc";
		else if (!bTemp)
			strStatus = "Err:Temp";

		BIOS::LCD::Printf(20, 0x90, RGB565(ffFF00), 0, "OneWire status = %s     ", strStatus );
		
		clr = (bConv && bCrc) ? RGB565(ffff00) : RGB565(808080);

		BIOS::LCD::Printf(20, 0xC0, clr, 0, "Scratchpad = ");
		for (int i=0; i<9; i++)
			BIOS::LCD::Printf(20+13*8+i*24, 0xC0, clr, 0, "%c%c", 
				CUtils::tohex(pSP[i]>>4), CUtils::tohex(pSP[i]&0xf));

		clr = bTemp ? RGB565(ffff00) : RGB565(808080);
		BIOS::LCD::Printf(20, 0xD0, clr, 0, "Temperature = %2f" "\xf8" "C   ", fTemp );

		if ( nFamilyCode == CDS1820::DS18B20_FAMILY_CODE && pSP[4] != 0x7f ) 
		{
			m_Thermometer.SetConfig( 0, 0, 0x7f );	// set 12 bits resolution
		}
	}

	virtual void OnTimer()
	{
		if ( HasOverlay() )
			return;

		ui32* pCrl = BIOS::GPIO::GetRegister( BIOS::GPIO::PortA, BIOS::GPIO::RegCrl );
		ui32* pCrh = BIOS::GPIO::GetRegister( BIOS::GPIO::PortA, BIOS::GPIO::RegCrh );
		ui32* pIdr = BIOS::GPIO::GetRegister( BIOS::GPIO::PortA, BIOS::GPIO::RegIdr );
		ui32* pOdr = BIOS::GPIO::GetRegister( BIOS::GPIO::PortA, BIOS::GPIO::RegOdr );
//		ui32* pBsrr = BIOS::GPIO::GetRegister( BIOS::GPIO::PortA, BIOS::GPIO::RegBsrr );
//		ui32* pBrr = BIOS::GPIO::GetRegister( BIOS::GPIO::PortA, BIOS::GPIO::RegBrr );
		ui32* pLckr = BIOS::GPIO::GetRegister( BIOS::GPIO::PortA, BIOS::GPIO::RegLckr );
		                                                                   
		BIOS::LCD::Print ( 20, 0x20, RGB565(ffffff), RGB565(000000), "--- GPIOA ----------------------" );
		BIOS::LCD::Printf( 20, 0x30, RGB565(ffffff), RGB565(000000), "CRL  (0x%x) = 0x%x", pCrl, *pCrl);
		BIOS::LCD::Printf( 20, 0x40, RGB565(ffffff), RGB565(000000), "CRH  (0x%x) = 0x%x", pCrh, *pCrh);
		BIOS::LCD::Printf( 20, 0x50, RGB565(ffffff), RGB565(000000), "IDR  (0x%x) =", pIdr);
		PrintBits( 20+20*8, 0x50, *pIdr);
		BIOS::LCD::Printf( 20, 0x60, RGB565(ffffff), RGB565(000000), "ODR  (0x%x) =", pOdr);
		PrintBits( 20+20*8, 0x60, *pOdr);
		BIOS::LCD::Printf( 20, 0x70, RGB565(ffffff), RGB565(000000), "LCKR (0x%x) = 0x%x", pLckr, *pLckr);

		Update();
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		if (code == ToWord('L', 'D') )
		{
			KillTimer();
			BIOS::GPIO::SetState( BIOS::GPIO::PortA, 9, 0xA );
			return;
		}

		if (code == ToWord('L', 'E') )
		{
			m_Thermometer.Float();
			SetTimer(200);
			return;
		}
	}
};

#endif
