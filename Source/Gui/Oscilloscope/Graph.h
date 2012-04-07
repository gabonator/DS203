#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <Source/Framework/Wnd.h>
#include <Source/Core/Settings.h>
#include <string.h>

class CWndGraph : public CWnd
{
public:
	enum {
		BlkX = 30,
		BlkY = 25,
		DivsX = 10,
		DivsY = 8
	};

//public:
//	static ui16	m_nMaxX;

public:
	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndGraph", dwFlags, CRect(12, 22, 12+DivsX*BlkX, 22+DivsY*BlkY), pParent);
//		CWndGraph::m_nMaxX = -1;
	}

	virtual void OnPaint()
	{
		CRect rc = m_rcClient;
		rc.Inflate( 1, 1, 1, 1 );
		BIOS::LCD::Rectangle( rc, RGB565(b0b0b0) );

		const ui16 grid[] = {RGB565(808080), 0, 0, 0, 0};

		for (int x=1; x<10; x++)
			BIOS::LCD::Pattern(m_rcClient.left+BlkX*x, m_rcClient.top, m_rcClient.left+BlkX*x+1, m_rcClient.bottom, grid, COUNT(grid));
		for (int y=1; y<8; y++)
			BIOS::LCD::Pattern(m_rcClient.left, m_rcClient.top+BlkY*y, m_rcClient.right, m_rcClient.top+BlkY*y+1, grid, COUNT(grid));
		//BIOS::Printf( m_rcClient.left+4, m_rcClient.top+4, RGB565(ffffff), RGBTRANS, 
		//	"Total CWnd instances %d", CWnd::m_nInstances);
		BIOS::LCD::Print( m_rcClient.left+4, m_rcClient.top+4, RGB565(404040), RGBTRANS, 
			"Valky.eu");

		BIOS::LCD::Print( m_rcClient.left+4, m_rcClient.bottom-16, RGB565(ffffff), RGBTRANS, 
			"Built on " __DATE__ " " __TIME__);
	}
};

class CWndZoomBar : public CWnd
{
public:
	virtual void Create(CWnd* pParent, CWnd* pGraph) 
	{
		CRect rcClient( pGraph->m_rcClient );
		rcClient.top = rcClient.bottom + 4;
		rcClient.bottom = rcClient.top + 7;
		CWnd::Create("CWndZoomBar", WsVisible | WsNoActivate, rcClient, pParent);
	}

	virtual void OnPaint()
	{
		ui16 clr = RGB565(808080);
		CRect rcBorder = m_rcClient;
		rcBorder.Inflate(1, 0, 1, 0);
		BIOS::LCD::Rectangle( rcBorder, clr );
		rcBorder.Deflate(1, 1, 1, 1);
		BIOS::LCD::Bar( rcBorder, RGB565(000000));
		
		CRect rc = rcBorder;
		//ui16 nMax = 4096;
		ui16 nBegin = Settings.Time.Shift;
		ui16 nView = CWndGraph::DivsX*CWndGraph::BlkX;

		ui32 nXBegin = (nBegin*m_rcClient.Width())>>12;
		ui32 nXEnd = ((nBegin+nView)*m_rcClient.Width())>>12;

		rc.left = m_rcClient.left + nXBegin;
		rc.right = m_rcClient.left + nXEnd;
		BIOS::LCD::Bar( rc, RGB565(b04000) );
	}

};

class CWndOscGraph : public CWndGraph
{
	void _PrepareColumn( ui16 *column, ui16 n, ui16 clr )
	{
		memset( column, clr, DivsY*BlkY*2 );
		if ( n == 0 )
			return;
		if ( (n % BlkX) == 0)
		{
			for (ui16 y=5; y<DivsY*BlkY; y += 5)
				column[y] = RGB565(808080);
		} else
		if ( (n%6) == 0 )
		{
			for (ui16 y=BlkY; y<DivsY*BlkY-1; y += BlkY)
				column[y] = RGB565(808080);
		}
	}

	virtual void OnPaint()
	{
		ui16 column[CWndGraph::DivsY*CWndGraph::BlkY];
		if ( !CWnd::m_rcOverlay.IsValid() )
		{
			CRect rc = m_rcClient;
			rc.Inflate( 1, 1, 1, 1 );
			BIOS::LCD::Rectangle( rc, RGB565(b0b0b0) );
		}

		if ( !BIOS::ADC::Ready() ) 
			return;


		ui16 i;
		ui16 clr1 = Settings.CH1.u16Color;
		ui16 clr2 = Settings.CH2.u16Color;
		ui8 en1 = Settings.CH1.Enabled == CSettings::AnalogChannel::_YES;
		ui8 en2 = Settings.CH2.Enabled == CSettings::AnalogChannel::_YES;
//		CSettings::ChannelCalibrationCurve& cal1 = Settings.calCH1[Settings.CH1.Resolution];
//		CSettings::ChannelCalibrationCurve& cal2 = Settings.calCH2[Settings.CH2.Resolution];

		int nMax = CWnd::m_rcOverlay.IsValid() ? CWnd::m_rcOverlay.left - m_rcClient.left : m_rcClient.Width();
//		int nMax = min(m_rcClient.Width(), m_nMaxX-m_rcClient.left);
//		nMax <<= 2;

		ui8 bTrigger = (BIOS::GetTick() - Settings.Trig.nLastChange) < 5000;
		ui16 nTriggerTime = (Settings.Trig.nTime - Settings.Time.Shift);
		if (!bTrigger)
			nTriggerTime = -1;

		for (i=0; i<Settings.Time.Shift; i++)
			BIOS::ADC::Get();

		for (ui16 x=0; x<nMax; x++)
		{
			_PrepareColumn( column, x, (nTriggerTime != x) ? 0x01 : 0x00 );

			ui32 val = BIOS::ADC::Get();
			if ( en2 )
			{
				si16 ch2 = (ui8)((val>>8) & 0xff);
				ch2 = CSettings::Correct(Settings.CH1.u16Position, ch2);
				if ( ch2 < 0 ) 
					ch2 = 0;
				if ( ch2 > 255 ) 
					ch2 = 255;
				ui16 y = (ch2*(DivsY*BlkY))>>8;
				column[y] = clr2;
			}

			if ( en1 )
			{
				si16 ch1 = (ui8)((val) & 0xff);
				ch1 = CSettings::Correct(Settings.CH1.u16Position, ch1);
				if ( ch1 < 0 ) 
					ch1 = 0;
				if ( ch1 > 255 ) 
					ch1 = 255;
				ui16 y = (ch1*(DivsY*BlkY))>>8;
				column[y] = clr1;
			}

			if ( bTrigger && (x & 1) == 1 )
			{
				ui16 y = (Settings.Trig.nLevel*(DivsY*BlkY))>>8;
				column[y] = RGB565(404040);
			}

			BIOS::LCD::Buffer( m_rcClient.left + x, m_rcClient.top, column, DivsY*BlkY );
		}

/*
		_PrepareColumn( column, 0, (nTriggerTime != 0) ? 0x01 : 0x00 );

		for (i=0; i<nMax; i++)
		{
			ui16 x = i>>2;
			ui16 nx = (i+1)>>2;

			ui32 val = BIOS::ADC::Get();
			if ( en2 )
			{
				si16 ch2 = (ui8)((val>>8) & 0xff);
				ch2 = Settings.calCH2.Apply( ch2 );
				ui16 y = (ch2*(DivsY*BlkY))>>8;
				column[y] = clr2;
			}

			if ( en1 )
			{
				si16 ch1 = (ui8)((val) & 0xff);
				ch1 = Settings.calCH2.Apply( ch1 );
				ui16 y = (ch1*(DivsY*BlkY))>>8;
				column[y] = clr1;
			}

			if (x!=nx)	
			{
				if ( bTrigger && (x & 1) == 1 )
				{
					ui16 y = (Settings.Trig.nLevel*(DivsY*BlkY))>>8;
					column[y] = RGB565(404040);
				}

				BIOS::LCD::Buffer( m_rcClient.left + x, m_rcClient.top, column, DivsY*BlkY );
				_PrepareColumn( column, nx, (nTriggerTime != nx) ? 0x01 : 0x00 );
				//memset( column, 0x01, sizeof(column) );
			}
		}
*/

	}
};

class CWndSigGraph : public CWndGraph
{
	const ui16* m_pSignal;
	ui16 m_nCount;

public:
	CWndSigGraph() : CWndGraph()
	{
		m_pSignal = NULL;
	}

	void Setup(const ui16* pSignal, ui16 nCount )
	{
		m_pSignal = pSignal;
		m_nCount = nCount;
		Invalidate();
	}

	virtual void OnPaint()
	{
		BIOS::LCD::Bar( m_rcClient, RGB565(000000) );	

		CWndGraph::OnPaint();
		if (!m_pSignal)
			return;

		int nWidth = m_rcClient.Width();
		int nHeight = m_rcClient.Height();

		nHeight -= BlkY*2;
		int nBaseY = m_rcClient.bottom-BlkY;
		ui16 oy = 0;
		for (ui16 i=0; i<nWidth; i++)
		{
			ui16 y = m_pSignal[i*m_nCount/nWidth];
			_ASSERT( y < 0x1000 );
			y = y*nHeight/0x1000;
			if (i==0)
				oy = y;
			
			BIOS::LCD::Line( 
				m_rcClient.left + i-1, nBaseY - oy, 
				m_rcClient.left + i, nBaseY - y, 
				RGB565(ffffff) );
			oy = y;
		}
	}
};

#endif