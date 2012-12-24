#include "Export.h"
#include <Source/Gui/MainWnd.h>
//#include <Source/Gui/Oscilloscope/Controls/GraphOsc.h>

LINKERSECTION(".extra")
void CExport::SaveSvg(char* strName_ /*= NULL*/)
{
	char strNameUnique[] = "WAVE000 SVG";
	char* strName = strName_ ? strName_ : strNameUnique;
	if ( !strName_ )
		FindUnusedFile( strName, 4 );

	CBufferedWriter writer;
	writer.Open( strName );

	CSettings::Calibrator::FastCalc fastCalc1, fastCalc2;
	Settings.CH1Calib.Prepare( &Settings.CH1, fastCalc1 );
	Settings.CH2Calib.Prepare( &Settings.CH2, fastCalc2 );

	int nBegin = 0;
	int nEnd = 0;
	MainWnd.m_wndGraph.GetCurrentRange( nBegin, nEnd );
	int nMessed = Settings.Time.InvalidFirst;

	int nCount = (int)BIOS::ADC::GetCount();
	writer << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
	writer << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n";
	writer << "<svg width=\"" << CUtils::itoa(nCount) << "\" height=\"256\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\""
		<< " onload=\"init(evt)\" onmousemove=\"msMove(evt)\"" 
		<< ">\n";

	writer << "<path stroke=\"none\" fill=\"black\" d=\"M0,0 H" << CUtils::itoa(nCount) << " V256 H0 V0\"/>\n";
	writer << "<path stroke=\"none\" fill=\"#000040\" d=\"M" << 
		CUtils::itoa(nBegin) << ",0 H" << CUtils::itoa(nEnd) << " V256 H" <<
		CUtils::itoa(nBegin) << " V0\"/>\n";

	writer << "<path stroke=\"none\" fill=\"#400000\" d=\"M0,0 " << 
		"H" << CUtils::itoa(nMessed) << " V256 H0 V0\"/>\n";

	for (int y=0; y<=256; y+=32)
		writer << "<path stroke=\"#404040\" d=\"M0," << CUtils::itoa(y) << " H" << CUtils::itoa(nCount) << "\"/>\n";
	for (int x=0; x<nCount; x+=32)
		writer << "<path stroke=\"#404040\" d=\"M" << CUtils::itoa(x) << ",0 V256\"/>\n";

	writer << "<path stroke=\"yellow\" fill=\"none\" stroke-width=\"1\" d=\"";
	for (int i=0; i<(int)BIOS::ADC::GetCount(); i++)
	{
		char line[64];
		unsigned int nValue = BIOS::ADC::GetAt(i);
		int nCH1 = (ui8)((nValue) & 0xff);

		BIOS::DBG::sprintf(line, i==0 ? "M%d,%d " : "L%d,%d ", i, 256-nCH1);
		writer << line;
	}
	writer << "\"/>\n";

	writer << "<path stroke=\"cyan\" fill=\"none\" stroke-width=\"1\" d=\"";
	for (int i=0; i<(int)BIOS::ADC::GetCount(); i++)
	{
		char line[64];
		unsigned int nValue = BIOS::ADC::GetAt(i);
		int nCH2 = (ui8)((nValue>>8) & 0xff);

		BIOS::DBG::sprintf(line, i==0 ? "M%d,%d " : "L%d,%d ", i, 256-nCH2);
		writer << line;
	}
	writer << "\"/>\n";

	writer << "<text font-family=\"Arial\" x=\"638\" y=\"230\" fill=\"yellow\" font-size=\"20\" text-anchor=\"end\" id=\"CH1s\">CH1: "
		<< (char*) CSettings::AnalogChannel::ppszTextResolution[ Settings.CH1.Resolution ] << "/div "
		<< "(" << (char*) CSettings::AnalogChannel::ppszTextProbe[ Settings.CH1.Probe] << ") "
		<< (char*) CSettings::TimeBase::ppszTextResolution[ Settings.Time.Resolution ] << "/div</text>\n";
	writer << "<text font-family=\"Arial\" x=\"2\" y=\"230\" fill=\"yellow\" font-size=\"20\" text-anchor=\"start\" id=\"CH1\"> </text>\n";
  	writer << "<text font-family=\"Arial\" x=\"638\" y=\"250\" fill=\"cyan\" font-size=\"20\" text-anchor=\"end\" id=\"CH2s\">CH2: "
		<< (char*) CSettings::AnalogChannel::ppszTextResolution[ Settings.CH2.Resolution ] << "/div "
		<< "(" << (char*) CSettings::AnalogChannel::ppszTextProbe[ Settings.CH2.Probe] << ") "
		<< (char*) CSettings::TimeBase::ppszTextResolution[ Settings.Time.Resolution ] << "/div</text>\n";
	writer << "<text font-family=\"Arial\" x=\"2\" y=\"250\" fill=\"cyan\" font-size=\"20\" text-anchor=\"start\" id=\"CH2\"> </text>\n";

	float fScale1 = Settings.Runtime.m_fCH1Res*1000;
	float fScale2 = Settings.Runtime.m_fCH2Res*1000;
	int nOfs1 = 255-Settings.CH1.u16Position;
	int nOfs2 = 255-Settings.CH2.u16Position;
	float fTime = Settings.Runtime.m_fTimeRes*1000;

	writer << "<script type=\"text/ecmascript\"><![CDATA[\n";
	writer << "function init(evt) { if ( window.svgDocument == null ) svgDocument = evt.target.ownerDocument; }\n";
	writer << "function fmtT(val, div) {\n";
 	writer << " if (div > 100) return Math.floor(val*div/32) + \" ms\";\n";
  	writer << "if (div > 0.025) return Math.floor(val*div*1000/32)/1000 + \" ms\";\n";
 	writer << " return Math.floor(val*div*1000*1000/32)/1000 + \" us\";\n";
	writer << "}\n";
	writer << "function fmtU(val, div, shift) { return Math.floor((shift-val)*div/32) + \" mV\"; }\n";
	writer << "function msMove (evt)\n";
	writer << "{\n";
	writer << "	var x = evt.clientX;\n";
	writer << "	var y = evt.clientY;\n";
	writer << "	if (x < 0 || y < 0 || x >= " << CUtils::itoa(nCount) << " || y >= 256)\n";
	writer << "		return;\n";
	writer << "	var hscroll = (document.all ? document.scrollLeft : window.pageXOffset);\n";
	writer << "	var width = window.innerWidth;\n";
	writer << "\n";
	writer << "	svgDocument.getElementById(\"CH1\").setAttribute(\"x\", hscroll+20);\n";
	writer << "	svgDocument.getElementById(\"CH2\").setAttribute(\"x\", hscroll+20);\n";
	writer << "	svgDocument.getElementById(\"CH1s\").setAttribute(\"x\", hscroll+width-20);\n";
	writer << "	svgDocument.getElementById(\"CH2s\").setAttribute(\"x\", hscroll+width-20);\n";
	writer << "\n";
	writer << "	svgDocument.getElementById(\"CH1\").firstChild.data = \"CH1: \"+fmtU(y, " << CUtils::ftoa(fScale1) 
		<< ", " << CUtils::itoa(nOfs1) << ") + \" \" + fmtT(x, " << CUtils::ftoa(fTime) << ");\n";
	writer << "	svgDocument.getElementById(\"CH2\").firstChild.data = \"CH2: \"+fmtU(y, " << CUtils::ftoa(fScale2) 
		<<", " << CUtils::itoa(nOfs2) << ") + \" \" + fmtT(x, " << CUtils::ftoa(fTime) << ");\n";
	writer << "}\n";
	writer << "]]></script>\n";
	writer << "</svg>\n";

	writer.Close();
}


void CExport::SaveWav(char* strName_ /*= NULL*/)
{
	char strNameUnique[] = "WAVE000 WAV";
	char* strName = strName_ ? strName_ : strNameUnique;
	if ( !strName_ )
		FindUnusedFile( strName, 4 );

	CBufferedWriter writer;
	writer.Open( strName );
	
	typedef ui16 WORD;
	typedef ui32 DWORD;
	struct WAVEFORMATEX 
	{
	  WORD  wFormatTag;
	  WORD  nChannels;
	  DWORD nSamplesPerSec;
	  DWORD nAvgBytesPerSec;
	  WORD  nBlockAlign;
	  WORD  wBitsPerSample;
	  WORD  cbSize;
	} WaveFormat;

	int nSamplingSpeed = (int)( CWndGraph::BlkX / Settings.Runtime.m_fTimeRes + 0.5f );

	WaveFormat.wFormatTag = 1; // WAVE_FORMAT_PCM
	WaveFormat.nChannels = 2;
	WaveFormat.nSamplesPerSec = nSamplingSpeed;	
	WaveFormat.wBitsPerSample = 8;
	WaveFormat.cbSize = 0;

	WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.wBitsPerSample / 8;
	WaveFormat.nBlockAlign = WaveFormat.nChannels * WaveFormat.wBitsPerSample / 8;

	int nJunkSize = 14*4;

	ui32 dwSize = sizeof( WAVEFORMATEX ) + 4096*2 + 6*4 + nJunkSize;

	writer 
		<< ToDword('R', 'I', 'F', 'F')
		<< dwSize
			<< ToDword('W', 'A', 'V', 'E')
				<< ToDword('f', 'm', 't', ' ')
				<< (ui32)sizeof(WaveFormat)
				<< CStream( &WaveFormat, sizeof(WAVEFORMATEX) )

				<< ToDword('d', 'a', 't', 'a')
				<< (ui32)BIOS::ADC::GetCount()*2;

	for (int i=0; i< (int)BIOS::ADC::GetCount(); i++)
	{
		BIOS::ADC::SSample Sample;
		Sample.nValue = BIOS::ADC::GetAt(i);
		writer 
			<< Sample.CH1 
			<< Sample.CH2;
	}

	writer 
			<< ToDword('J', 'U', 'N', 'K')
			<< (ui32)nJunkSize;

	writer << ToDword('D', 'S', 'W', '1');
	writer << ToDword('C', 'H', '1', ' ');
	writer << (ui32)Settings.CH1.Resolution;
	writer << (ui32)Settings.CH1.Coupling;
	writer << (ui32)Settings.CH1.u16Position;

	writer << ToDword('C', 'H', '2', ' ');
	writer << (ui32)Settings.CH2.Resolution;
	writer << (ui32)Settings.CH2.Coupling;
	writer << (ui32)Settings.CH2.u16Position;

	writer << ToDword('T', 'I', 'M', 'E');
	writer << (ui32)Settings.Time.Resolution;

	writer << ToDword('E', 'N', 'D', 27);


	writer.Close();
}

void CExport::SaveScreenshot32(char* strName_ /*= NULL*/)
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

void CExport::SaveScreenshot16(char* strName_ /*= NULL*/)
{
	char strNameUnique[] = "IMG000  BMP";
	char* strName = strName_ ? strName_ : strNameUnique;
	if ( !strName_ )
		FindUnusedFile( strName, 3 );

	CBufferedWriter writer;
	writer.Open( strName );


	BmpHdr hdr;
	BmpHdr* pHdr = &hdr;
	pHdr->wBfType = 'B' | ('M'<<8);
	pHdr->dwBfSize = sizeof(BmpHdr) + BIOS::LCD::LcdWidth * BIOS::LCD::LcdHeight * 3;  // no need to align row to multiply of 4
	pHdr->wBfReserved1 = 0;
	pHdr->wBfReserved2 = 0;
	pHdr->dwBfOffset = 0x36;

	pHdr->dwBiSize = 0x28;
	pHdr->dwBiWidth = BIOS::LCD::LcdWidth;
	pHdr->dwBiHeight = BIOS::LCD::LcdHeight;
	pHdr->wBiPlanes = 1;
	pHdr->wBiBitDepth = 16;
	pHdr->dwBiCompression = 0;
	pHdr->dwBiSizeImage = BIOS::LCD::LcdWidth * BIOS::LCD::LcdHeight * 3;
	pHdr->dwBiXPels = 0;
	pHdr->dwBiYPels = 0;
	pHdr->dwBiClrUsed = 0;
	pHdr->dwBiClrImportant = 0;

	writer << CStream( &hdr, sizeof(BmpHdr) );

	for ( int y = BIOS::LCD::LcdHeight-1; y >= 0; y-- )
		for ( int x = 0; x < BIOS::LCD::LcdWidth; x++ )
		{
			ui16 wPixel = BIOS::LCD::GetPixel( x, y );

			if (((x>>2)+(y>>2))&1)
				BIOS::LCD::PutPixel( x, y, wPixel^0x18e3);

			int r = Get565R( wPixel ) >> 3;
			int g = Get565G( wPixel ) >> 3;
			int b = Get565B( wPixel ) >> 3;

			// RGB555
			wPixel = b | (g << 5) | (r << 10);
			writer << wPixel;
		}

	writer.Close();

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

