#ifndef __TOOLBOX_EXPORT_H__
#define __TOOLBOX_EXPORT_H__

#include <Source/Core/Serialize.h>
#include <Source/Core/Settings.h>
#include <Source/Gui/Oscilloscope/Controls/GraphBase.h>
#include "BufferedIo.h"
#include <Source/Core/Utils.h>

class CExport
{
#pragma pack(push)
#pragma pack(2)
	struct BmpHdr 
	{
		ui16 wBfType;
		ui32 dwBfSize;
		ui16 wBfReserved1;
		ui16 wBfReserved2;
		ui32 dwBfOffset;

		ui32 dwBiSize;
		ui32 dwBiWidth;
		ui32 dwBiHeight;
		ui16 wBiPlanes;
		ui16 wBiBitDepth;
		ui32 dwBiCompression;
		ui32 dwBiSizeImage;
		ui32 dwBiXPels;
		ui32 dwBiYPels;

		ui32 dwBiClrUsed;
		ui32 dwBiClrImportant;
	};
#pragma pack(pop)

public:
	void FindUnusedFile(char* strName, int nNumbers)
	{
		bool bExists = false;
		FILEINFO f;
		do {
			bExists = false;
			if ( BIOS::DSK::Open( &f, strName, BIOS::DSK::IoRead ) )
			{
				BIOS::DSK::Close( &f );
				if ( ++strName[nNumbers+2] > '9' )
				{
					strName[nNumbers+2] = '0';
					if ( ++strName[nNumbers+1] > '9' )
					{
						strName[nNumbers+1] = '0';
						strName[nNumbers]++;
						_ASSERT( strName[nNumbers] <= '9' );
					}
				}
				continue;
			}
			break;
		} while (1);
	}

	void SaveScreenshot(char* strName_ = NULL)
	{
		char strNameUnique[] = "IMG000  BMP";
		char* strName = strName_ ? strName_ : strNameUnique;
		if ( !strName_ )
			FindUnusedFile( strNameUnique, 3 );
		
		FILEINFO f;
		// strName contains unique non existent file name 
		if ( !BIOS::DSK::Open( &f, strName, BIOS::DSK::IoWrite ) )
		{
			_ASSERT(0);
			return;
		}
		int nSize = 0, nOffset = 0;
		ui8* pData = (ui8*)BIOS::DSK::GetSharedBuffer();

		BmpHdr* pHdr = (BmpHdr*)pData;
		pHdr->wBfType = 'B' | ('M'<<8);
		pHdr->dwBfSize = sizeof(BmpHdr) + BIOS::LCD::LcdWidth * BIOS::LCD::LcdHeight * 3;  // no need to align row to multiply of 4
		pHdr->wBfReserved1 = 0;
		pHdr->wBfReserved2 = 0;
		pHdr->dwBfOffset = 0x36;

		pHdr->dwBiSize = 0x28;
		pHdr->dwBiWidth = BIOS::LCD::LcdWidth;
		pHdr->dwBiHeight = BIOS::LCD::LcdHeight;
		pHdr->wBiPlanes = 1;
		pHdr->wBiBitDepth = 24;
		pHdr->dwBiCompression = 0;
		pHdr->dwBiSizeImage = BIOS::LCD::LcdWidth * BIOS::LCD::LcdHeight * 3;
		pHdr->dwBiXPels = 0;
		pHdr->dwBiYPels = 0;
		pHdr->dwBiClrUsed = 0;
		pHdr->dwBiClrImportant = 0;

		nOffset = sizeof(BmpHdr);
		for ( int y = BIOS::LCD::LcdHeight-1; y >= 0; y-- )
			for ( int x = 0; x < BIOS::LCD::LcdWidth; x++ )
			{
				unsigned char arrColors[3];
				ui16 wPixel = BIOS::LCD::GetPixel( x, y );
				if (((x>>2)+(y>>2))&1)
				{
					BIOS::LCD::PutPixel( x, y, wPixel^0x18e3);
				}
				arrColors[2] = Get565R(wPixel);
				arrColors[1] = Get565G(wPixel);
				arrColors[0] = Get565B(wPixel);
				for ( int c=0; c<3; c++ )
				{
					pData[nOffset] = arrColors[c];
					if ( ++nOffset >= FILEINFO::SectorSize )
					{
						nSize += FILEINFO::SectorSize;
						nOffset = 0;
						BIOS::DSK::Write( &f, pData );
					}

				}
			}

		if ( nOffset > 0 )
			BIOS::DSK::Write( &f, pData );

		BIOS::DSK::Close( &f, nSize + nOffset );
		// Display message
		//Sleep(500);

		for ( int y = BIOS::LCD::LcdHeight-1; y >= 0; y-- )
			for ( int x = 0; x < BIOS::LCD::LcdWidth; x++ )
			{
				if (((x>>2)+(y>>2))&1)
				{
					ui16 wPixel = BIOS::LCD::GetPixel( x, y );
					BIOS::LCD::PutPixel( x, y, wPixel^0x18e3);
					// ..... ...... .....
					// ...11 ...111 ...11
				}
			}

	}

	void SaveBinary(char* strName_ = NULL)
	{
		char strNameUnique[] = "WAVE000 DAT";
		char* strName = strName_ ? strName_ : strNameUnique;
		if ( !strName_ )
			FindUnusedFile( strNameUnique, 4 );

		FILEINFO f;

		// strName contains unique non existent file name 
		if ( !BIOS::DSK::Open( &f, strName, BIOS::DSK::IoWrite ) )
		{
			_ASSERT(0);
			return;
		}
		int nSize = 0, nOffset = 0;
		ui8* pData = (ui8*)BIOS::DSK::GetSharedBuffer();

		for (int i=0; i</*BIOS::ADC::Length()*/ (int)BIOS::ADC::GetCount(); i++)
		{
			unsigned int nValue = BIOS::ADC::GetAt(i);

			for (int j=0; j<4; j++)
			{
				pData[nOffset] = (unsigned char)( nValue >> (j<<3) );
				if ( ++nOffset >= FILEINFO::SectorSize )
				{
					nSize += FILEINFO::SectorSize;
					nOffset = 0;
					BIOS::DSK::Write( &f, pData );
				}
			}
		}

		if ( nOffset > 0 )
			BIOS::DSK::Write( &f, pData );

		BIOS::DSK::Close( &f, nSize + nOffset );
	}

	void SaveCsv(char* strName_ = NULL)
	{
		char strNameUnique[] = "WAVE000 CSV";
		char* strName = strName_ ? strName_ : strNameUnique;
		if ( !strName_ )
			FindUnusedFile( strName, 4 );

		CBufferedWriter writer;
		writer.Open( strName );

		float fTimeRes = Settings.Runtime.m_fTimeRes / CWndGraph::BlkX;
	
		CSettings::Calibrator::FastCalc fastCalc1, fastCalc2;
		Settings.CH1Calib.Prepare( &Settings.CH1, fastCalc1 );
		Settings.CH2Calib.Prepare( &Settings.CH2, fastCalc2 );

		for (int i=0; i<(int)BIOS::ADC::GetCount(); i++)
		{
			char line[64];
			unsigned int nValue = BIOS::ADC::GetAt(i);
			int nCH1 = (ui8)((nValue) & 0xff);
			int nCH2 = (ui8)((nValue>>8) & 0xff);
	
			float fTime = fTimeRes * ( i - ( Settings.Trig.nTime - Settings.Time.Shift ) );
			float fCH1 = Settings.CH1Calib.Voltage( fastCalc1, (float)nCH1 );
			float fCH2 = Settings.CH2Calib.Voltage( fastCalc2, (float)nCH2 );

			BIOS::DBG::sprintf(line, "%4d, %08x, %6f, %6f, %6f\n", i, nValue, fTime, fCH1, fCH2);
			writer << line;
		}

		writer.Close();
	}

	void SaveSvg(char* strName_ = NULL);
	void SaveWav(char* strName_ = NULL);
		/*
	{
		char strNameUnique[] = "WAVE000 SVG";
		char* strName = strName_ ? strName_ : strNameUnique;
		if ( !strName_ )
			FindUnusedFile( strName, 4 );

		CBufferedWriter writer;
		writer.Open( strName );

		float fTimeRes = Settings.Runtime.m_fTimeRes / CWndGraph::BlkX;
	
		CSettings::Calibrator::FastCalc fastCalc1, fastCalc2;
		Settings.CH1Calib.Prepare( &Settings.CH1, fastCalc1 );
		Settings.CH2Calib.Prepare( &Settings.CH2, fastCalc2 );

		int nBegin = 0;
		int nEnd = 0;
		MainWnd.m_wndGraph.GetCurrentRange( nBegin, nEnd );

		int nCount = (int)BIOS::ADC::GetCount();
		writer << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
		writer << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n";
		writer << "<svg width=\"" << CUtils::itoa(nCount) << "\" height=\"256\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n";
		writer << "<g>\n";
		writer << "<path stroke=\"white\" fill=\"black\" stroke-width=\"2\" d=\"M0,0 H" << CUtils::itoa(nCount) << " V256 H0 V0\"/>\n";
		writer << "<path stroke=\"white\" fill=\"black\" stroke-width=\"2\" d=\"M" << 
			CUtils::itoa(nBegin) << ",0 H" << CUtils::itoa(nEnd) << " V256 H" <<
			CUtils::itoa(nEnd) << " V0\"/>\n";
		for (int y=0; y<=256; y+=32)
			writer << "<path stroke=\"white\" stroke-width=\"0.1\" d=\"M0," << CUtils::itoa(y) << " H" << CUtils::itoa(nCount) << "\"/>\n";
		for (int x=0; x<nCount; x+=32)
			writer << "<path stroke=\"white\" stroke-width=\"0.1\" d=\"M" << CUtils::itoa(x) << ",0 V256\"/>\n";

		writer << "<path stroke=\"yellow\" fill=\"none\" stroke-width=\"1\" d=\"";
		for (int i=0; i<(int)BIOS::ADC::GetCount(); i++)
		{
			char line[64];
			unsigned int nValue = BIOS::ADC::GetAt(i);
			int nCH1 = (ui8)((nValue) & 0xff);
			int nCH2 = (ui8)((nValue>>8) & 0xff);

			BIOS::DBG::sprintf(line, i==0 ? "M%d,%d " : "L%d,%d", i, 256-nCH2);
			writer << line;
		}
		writer << "\"/>\n";

		writer << "<path stroke=\"cyan\" fill=\"none\" stroke-width=\"1\" d=\"";
		for (int i=0; i<(int)BIOS::ADC::GetCount(); i++)
		{
			char line[64];
			unsigned int nValue = BIOS::ADC::GetAt(i);
			int nCH1 = (ui8)((nValue) & 0xff);
			int nCH2 = (ui8)((nValue>>8) & 0xff);

			BIOS::DBG::sprintf(line, i==0 ? "M%d,%d " : "L%d,%d", i, 256-nCH2);
			writer << line;
		}
		writer << "\"/>\n";

		writer << "</g>\n";
		writer << "</svg>\n";

		writer.Close();
	}
	*/
};

#endif