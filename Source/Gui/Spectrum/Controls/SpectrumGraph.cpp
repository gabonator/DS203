#include <Source/Core/Utils.h>
#include "SpectrumGraph.h"
#include "../Core/FFT.h"

#ifdef _TESTSIGNAL
#include <math.h> // for testing
#endif

/*virtual*/ void CWndSpectrumGraphTempl::Create(CWnd *pParent, ui16 dwFlags) 
{
	//CWnd::Create("CWndSpectrumGraph", dwFlags | CWnd::WsListener, CRect(34, 22, 34+DivsX*BlkX, 22+DivsY*BlkY), pParent);
	CWnd::Create("CWndSpectrumGraph", dwFlags | CWnd::WsListener, CRect(34+16, 222-DivsY*m_nBlkY, 34+16+DivsX*m_nBlkX, 222), pParent);
}

void CWndSpectrumGraphTempl::OnPaint()
{
	// maximum size
	ui16 column[CWndGraph::DivsY*CWndGraph::BlkY];

	if ( !CWnd::GetOverlay().IsValid() )
	{
		CRect rc = m_rcClient;
		rc.Inflate( 1, 1, 1, 1 );
		BIOS::LCD::Rectangle( rc, RGB565(b0b0b0) );
	}

	ui16 i;
	ui16 clr1 = Settings.CH1.u16Color;
	ui8 en1 = Settings.CH1.Enabled == CSettings::AnalogChannel::_YES;
	ui16 clr2 = Settings.CH2.u16Color;
	ui8 en2 = Settings.CH2.Enabled == CSettings::AnalogChannel::_YES;

	int nMarkerY = 0;
	int nMarkerMax = 0;
	int nLength = Settings.Spec.nWindowLength;
	int nMarkerX = nLength;

	_ASSERT( nLength == 512 );
	// share the ADC buffer with fft calculations
	si16* pWaveformR = (si16*)(PVOID)&BIOS::ADC::GetAt(4096-2048);
	si16* pWaveformI = pWaveformR + 512;
	si16* pDataOut1 =  (si16*)(PVOID)&BIOS::ADC::GetAt(1024);
	si16* pDataOut2 =  pDataOut1+256;

	bool bHann = Settings.Spec.Window == CSettings::Spectrum::_Hann;
	int nOffset = Settings.Time.InvalidFirst;

	int nSum[] = {0, 0};
	for ( int i = 0; i < 512; i++ )
	{
		BIOS::ADC::SSample Sample;
		Sample.nValue = BIOS::ADC::GetAt( nOffset + i );
		nSum[0] += Sample.CH1;
		nSum[1] += Sample.CH2;
	}
	nSum[0] /= 512;
	nSum[1] /= 512;

	CFft<512> fft;

	for ( int nInput = 2; nInput >= 1; nInput-- )
	{
		if ( nInput == 1 && !en1 )
			continue;
		if ( nInput == 2 && !en2 )
			continue;

		for ( int i = 0; i < 512; i++ )
		{
			BIOS::ADC::SSample Sample;
			Sample.nValue = BIOS::ADC::GetAt( nOffset + i );
			int nSample = nInput == 1 ? Sample.CH1 : Sample.CH2;
#ifdef _TESTSIGNAL
			float f = 10.0f; //(GetTickCount()/1000)&1 ? 30.0f : 60.0f;
			nSample = (int)(sin(i/512.0f*2.0f*3.141592*f)*128.0f+64);
#endif
			nSample -= nSum[nInput-1];
			// range -32768..32767
			int nWindow = fft.Hann( i );
			if ( bHann )
				nSample = ( nSample * nWindow ) >> (16-7);
			else
				nSample = nSample << 6;

			//_ASSERT( nSample >= 0 && nSample < 32767 );
			pWaveformR[i] = nSample;
			pWaveformI[i] = 0;
		}

		fft.Forward( pWaveformR, pWaveformI );

		for ( int i = 0; i < 512/2; i++ )
		{
			int nR = pWaveformR[i];
			int nI = pWaveformI[i];
			int nLengthSq = nR*nR + nI*nI;
			int nLength_ = fft.Sqrt( nLengthSq );
			// nLength = 4095 zodpoveda amplitude 128
			// div 32, bitshift 5
			//int nLength = nLength_ * DivsY * m_nBlkY / 32 / 256;
			int nLength = nLength_ * DivsY * m_nBlkY / 16 / 256;
			if ( i==0 )		// why the hell is the DC value 2x bigger?
				nLength /= 2;
			UTILS.Clamp<int>( nLength, 0, DivsY*m_nBlkY);

			if ( i > 3 && nLength_ > nMarkerMax && nLength > 5 )
			{
				nMarkerMax = nLength_;
				nMarkerX = i;
				nMarkerY = nLength;
			}

			if (nInput == 1)
				pDataOut1[i] = nLength;
			if (nInput == 2)
				pDataOut2[i] = nLength;
		}
	}

	if ( nMarkerX == nLength )
	{
		Settings.Spec.nMarkerX = 0;
		Settings.Spec.fMarkerX = 0;
		Settings.Spec.fMarkerY = 0;
	} else {	
		Settings.Spec.nMarkerX = nMarkerX;
		Settings.Spec.fMarkerX = nMarkerX / 256.0f * (1.0f/(Settings.Runtime.m_fTimeRes/30.0f)) * 0.5f;	
		Settings.Spec.fMarkerY = nMarkerMax/32.0f/32.0f*Settings.Runtime.m_fCH1Res;
	}

	if ( nMarkerY > DivsY*m_nBlkY - 4 )
		nMarkerY = DivsY*m_nBlkY - 4;

	for (i=0; i<DivsX*m_nBlkX; i++)
	{
		_PrepareColumn( column, i, 0x0101 );

		if ( i < 256 )
		{
			if ( en1 )
				for ( int t = 0; t < pDataOut1[i]; t++ )
					column[t] = clr1;
			if ( en2 )
				for ( int t = 0; t < pDataOut2[i]; t++ )
					column[t] = clr2;
		}

		BIOS::LCD::Buffer( m_rcClient.left + i, m_rcClient.top, column, DivsY*m_nBlkY );
		if ( i == nMarkerX+3 )
		{
			BIOS::LCD::Draw( m_rcClient.left+nMarkerX-3, m_rcClient.bottom-nMarkerY-4, RGB565(ff0000), RGBTRANS, CShapes::markerX);
		}
	}
}

/*virtual*/ void CWndTimeGraphTempl::Create(CWnd *pParent, ui16 dwFlags) 
{
	CWnd::Create("CWndTimeGraphTempl", dwFlags | CWnd::WsListener, CRect(34+16, 22, 34+16+DivsX*m_nBlkX, 22+DivsY*m_nBlkY), pParent);
}

void CWndTimeGraphTempl::OnPaint()
{
	#define LineTo(buf, ynew, ylast, clr) \
	if ( ynew > ylast )	\
		for (int _y = ylast; _y <= ynew; _y++)	\
			buf[_y] = clr;	\
	else	\
		for (int _y = ynew; _y <= ylast; _y++)	\
			buf[_y] = clr;	\
		ylast = ynew;	

	// maximum size
	ui16 column[CWndGraph::DivsY*CWndGraph::BlkY];

	if ( !CWnd::GetOverlay().IsValid() )
	{
		CRect rc = m_rcClient;
		rc.Inflate( 1, 1, 1, 1 );
		BIOS::LCD::Rectangle( rc, RGB565(b0b0b0) );
	}

	ui16 i;
	ui16 clr1 = Settings.CH1.u16Color;
	ui8 en1 = Settings.CH1.Enabled == CSettings::AnalogChannel::_YES;
	ui16 clr2 = Settings.CH2.u16Color;
	ui8 en2 = Settings.CH2.Enabled == CSettings::AnalogChannel::_YES;
	bool bHann = Settings.Spec.Window == CSettings::Spectrum::_Hann;
	int nOffset = Settings.Time.InvalidFirst;

	int nLength = Settings.Spec.nWindowLength;
	int nPixels = DivsX*m_nBlkX;
	int nSampleIndex = 0;
	int nLastY1 = -1;
	int nLastY2 = -1;

	int nSum[] = {0, 0};
	for ( int i = 0; i < 512; i++ )
	{
		BIOS::ADC::SSample Sample;
		Sample.nValue = BIOS::ADC::GetAt( nOffset + i );
		nSum[0] += Sample.CH1;
		nSum[1] += Sample.CH2;
	}
	nSum[0] /= 512;
	nSum[1] /= 512;

	CFft<512> fft;

	for (i=0; i<nPixels; i++)
	{
		int nCurSampleIndex = i*nLength/nPixels;
		int nWindow = 0x10000;
		if ( bHann )
			nWindow = fft.Hann(nCurSampleIndex);

		bool bSatur = false;


		for (int nTest=nSampleIndex; nTest < nCurSampleIndex && !bSatur; nTest++ )
		{
			BIOS::ADC::SSample Sample;
			Sample.nValue = BIOS::ADC::GetAt(nOffset + nSampleIndex);
			if ( en2 )
			{
				int y2 = Sample.CH2;
				if ( y2 == 0 || y2 == 255 )
					bSatur = true;
			}
			if ( en1 )
			{
				int y1 = Sample.CH1;
				if ( y1 == 0 || y1 == 255 )
					bSatur = true;
			}

		}

		_PrepareColumn( column, i, bSatur ? RGB565(800000) : 0x0101 );

		for (; nSampleIndex < nCurSampleIndex; nSampleIndex++ )
		{
			BIOS::ADC::SSample Sample;
			Sample.nValue = BIOS::ADC::GetAt(nOffset + nSampleIndex);
			if ( en2 )
			{
				int y2 = Sample.CH2;
				if ( bHann )
				{
					y2 -= nSum[1];
					y2 = (y2 * nWindow) >> 16;
					y2 += 128;
				}
				y2 = (y2*(DivsY*m_nBlkY))>>8;
				if ( nLastY2 == -1 )
					nLastY2 = y2;
				LineTo( column, y2, nLastY2, clr2 );
			}

			if ( en1 )
			{
				int y1 = Sample.CH1;
				if ( bHann )
				{
					y1 -= nSum[0];
					y1 = (y1 * nWindow) >> 16;
					y1 += 128;
				}
				y1 = (y1*(DivsY*m_nBlkY))>>8;
				if ( nLastY1 == -1 )
					nLastY1 = y1;
				LineTo( column, y1, nLastY1, clr1 );
			}
		}

		BIOS::LCD::Buffer( m_rcClient.left + i, m_rcClient.top, column, DivsY*m_nBlkY );
	}
}

/*virtual*/ void CWndSpectrographTempl::Create(CWnd *pParent, ui16 dwFlags) 
{
	CWnd::Create("CWndSpectrographTempl", dwFlags | CWnd::WsListener, CRect(34+16, 222-m_nHeight, 34+16+m_nWidth, 222), pParent);
	m_nY = 0;
}

/*virtual*/ void CWndSpectrographTempl::OnPaint()
{
	// maximum size
	ui16 column[256];
	_ASSERT( m_nWidth == 256 );

	if ( !CWnd::GetOverlay().IsValid() )
	{
		CRect rc = m_rcClient;
		rc.Inflate( 1, 1, 1, 1 );
		BIOS::LCD::Rectangle( rc, RGB565(b0b0b0) );
	}

	ui16 clr1 = Settings.CH1.u16Color;
	ui8 en1 = Settings.CH1.Enabled == CSettings::AnalogChannel::_YES;
	ui16 clr2 = Settings.CH2.u16Color;
	ui8 en2 = Settings.CH2.Enabled == CSettings::AnalogChannel::_YES;

	ui32 clr32A = RGB32( Get565R( clr1 ), Get565G( clr1 ), Get565B( clr1 ) );
	ui32 clr32B = RGB32( Get565R( clr2 ), Get565G( clr2 ), Get565B( clr2 ) );

	int nLength = Settings.Spec.nWindowLength;

	_ASSERT( nLength == 512 );
	// share the ADC buffer with fft calculations
	si16* pWaveformR = (si16*)(PVOID)&BIOS::ADC::GetAt(4096-2048);
	si16* pWaveformI = pWaveformR + 512;

	bool bHann = Settings.Spec.Window == CSettings::Spectrum::_Hann;
	int nOffset = Settings.Time.InvalidFirst;

	CFft<512> fft;

	memset( column, 0, sizeof(column) );
	for ( int nInput = 2; nInput >= 1; nInput-- )
	{
		if ( nInput == 1 && !en1 )
			continue;
		if ( nInput == 2 && !en2 )
			continue;

		for ( int i = 0; i < 512; i++ )
		{
			BIOS::ADC::SSample Sample;
			Sample.nValue = BIOS::ADC::GetAt( nOffset + i );
			int nSample = nInput == 1 ? Sample.CH1 : Sample.CH2;

			//float f = (GetTickCount()/1000)&1 ? 30.0f : 60.0f;
			//nSample = (int)(sin(i/512.0f*2.0f*3.141592*f)*128.0f+128);

			// range -32768..32767
			int nWindow = fft.Hann( i );
			if ( bHann )
				nSample = ( nSample * nWindow ) >> (16-7);
			else
				nSample = nSample << 6;

			//_ASSERT( nSample >= 0 && nSample < 32767 );
			pWaveformR[i] = nSample;
			pWaveformI[i] = 0;
		}

		fft.Forward( pWaveformR, pWaveformI );
		for ( int i = 0; i < 512/2; i++ )
		{
			int nR = pWaveformR[i];
			int nI = pWaveformI[i];
			int nLengthSq = nR*nR + nI*nI;
			int nLength_ = fft.Sqrt( nLengthSq );
			// nLength = 4095 zodpoveda amplitude 128
			// div 32, bitshift 5
	
			//int nLength = nLength_ / 16;
			int nLength = nLength_ / 8;	// amplify it 2x
			UTILS.Clamp<int>( nLength, 0, 255);

			// store vector lengths in column
			if ( nInput == 1 )
				column[i] |= nLength;
			else
				column[i] |= nLength << 8;
		}
	}

	// convert vector lengths to colors
	const ui16 clrBackground = 0x0101;
	for ( int i = 0; i < 256; i++ )
	{
		ui8 ch1 = (ui8)column[i];
		ui8 ch2 = (ui8)(column[i]>>8);

		ui8 color[3] = {Get565R(clrBackground), Get565G(clrBackground), Get565B(clrBackground)};
		if ( en1 )
		{
			int colorA[3] = { 
				(int) ((GetColorR(clr32A)*ch1) >> 8), 
				(int) ((GetColorG(clr32A)*ch1) >> 8),
				(int) ((GetColorB(clr32A)*ch1) >> 8)};
			color[0] = max( colorA[0], color[0] );
			color[1] = max( colorA[1], color[1] );
			color[2] = max( colorA[2], color[2] );
		}
		if ( en2 )
		{
			int colorB[3] = { 
				(int) ((GetColorR(clr32B)*ch2) >> 8), 
				(int) ((GetColorG(clr32B)*ch2) >> 8),
				(int) ((GetColorB(clr32B)*ch2) >> 8)};
			color[0] = max( colorB[0], color[0] );
			color[1] = max( colorB[1], color[1] );
			color[2] = max( colorB[2], color[2] );
		}
		column[i] = RGB565RGB( color[0], color[1], color[2] );
	}

	// blit
	BIOS::LCD::PutImage( CRect( m_rcClient.left, m_rcClient.top + m_nY, m_rcClient.right, m_rcClient.top + m_nY + 1 ), column );

	if ( ++m_nY >= m_nHeight )
		m_nY = 0;
}
