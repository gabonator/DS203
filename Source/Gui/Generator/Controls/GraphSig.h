#include <Source/Gui/Oscilloscope/Controls/GraphBase.h>

//#define _GENERATOR_MULTIVIEW

class CWndSigGraph : public CWnd
{
public:
	const ui16* m_pSignal;
	ui16 m_nCount;

	enum {
		BlkX = 25,
		BlkY = 25,
#ifdef _GENERATOR_MULTIVIEW
		DivsX = 9,
#else
		DivsX = 10,
#endif
		DivsY = 8
	};

public:
	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndGraph", dwFlags, CRect(12, 22, 12+DivsX*BlkX, 22+DivsY*BlkY), pParent);
	}

	CWndSigGraph()
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
		CRect rc = m_rcClient;
		rc.Inflate( 1, 1, 1, 1 );
		BIOS::LCD::Rectangle( rc, RGB565(b0b0b0) );
		BIOS::LCD::Bar( m_rcClient, RGB565(000000) );	

		const ui16 grid[] = {RGB565(808080), 0, 0, 0, 0};

		for (int x=1; x<10; x++)
			BIOS::LCD::Pattern(m_rcClient.left+BlkX*x, m_rcClient.top, m_rcClient.left+BlkX*x+1, m_rcClient.bottom-4, grid, COUNT(grid));
		for (int y=1; y<8; y++)
			BIOS::LCD::Pattern(m_rcClient.left, m_rcClient.top+BlkY*y, m_rcClient.right, m_rcClient.top+BlkY*y+1, grid, COUNT(grid));

		if (!m_pSignal)
			return;

		int nWidth = m_rcClient.Width();
		int nHeight = m_rcClient.Height();

		nHeight -= BlkY*2;
		int nBaseY = m_rcClient.bottom-BlkY;
		ui16 oy = 0;
		for (ui16 i=0; i<nWidth; i++)
		{
#ifdef _GENERATOR_MULTIVIEW
			int nIndex = i*3*m_nCount/nWidth;
			int nSubIndex = nIndex % m_nCount;
			int nDivIndex = nIndex / m_nCount;
#else
			int nSubIndex = i*m_nCount/nWidth;
#endif
			ui16 y = m_pSignal[nSubIndex];
			_ASSERT( y < 0x1000 );
			y = y*nHeight/0x1000;
			if (i==0)
				oy = y;
#ifdef _GENERATOR_MULTIVIEW
			ui16 clr = nDivIndex == 1 ? RGB565(ffffff) : RGB565(808080);
#else
			ui16 clr = RGB565(ffffff);
#endif
			BIOS::LCD::Line( 
				m_rcClient.left + i-1, nBaseY - oy, 
				m_rcClient.left + i, nBaseY - y, 
				clr );
			oy = y;
		}

	}
};

