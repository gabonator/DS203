#ifndef __SPECTGRAPH_H__
#define __SPECTGRAPH_H__

#include <Source/Framework/Wnd.h>
#include <Source/Core/Settings.h>
#include <Source/Gui/Oscilloscope/Controls/GraphBase.h>

class CWndSpectrumGraphTempl : public CWnd
{
public:
	enum {
		//BlkX = BLKX, //32,
		//BlkY = BLKY, //25,
		DivsX = 8,
		DivsY = 4
	};
	int m_nBlkX;
	int m_nBlkY;

public:
	CWndSpectrumGraphTempl(int nBlkX, int nBlkY)
	{
		m_nBlkX = nBlkX;
		m_nBlkY = nBlkY;
	}

	virtual void Create(CWnd *pParent, ui16 dwFlags);

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
		{
			Invalidate();
			return;
		}
	}

	void _PrepareColumn( ui16 *column, ui16 n, ui16 clr )
	{
		memset( column, clr, DivsY*m_nBlkY*2 );
		if ( n == 0 )
			return;
		if ( n % 51 == 0 ) //(n % m_nBlkX) == 0)
		{
			for (ui16 y=5; y<DivsY*m_nBlkY; y += 5)
				column[y] = RGB565(808080);
		} else
		if ( (n%4) == 0 )
		{
			for (ui16 y=m_nBlkY; y<DivsY*m_nBlkY-1; y += m_nBlkY)
				column[y] = RGB565(808080);
		}
	}

	virtual void OnPaint();
};

class CWndTimeGraphTempl : public CWnd
{
public:
	enum {
		//BlkX = BLKX, //32,
		//BlkY = BLKY, //25,
		DivsX = 8,
		DivsY = 8
	};
	int m_nBlkX;
	int m_nBlkY;

public:
	CWndTimeGraphTempl(int nBlkX, int nBlkY)
	{
		m_nBlkX = nBlkX;
		m_nBlkY = nBlkY;
	}

	virtual void Create(CWnd *pParent, ui16 dwFlags);

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
		{
			Invalidate();
			return;
		}
	}

	void _PrepareColumn( ui16 *column, ui16 n, ui16 clr )
	{
		for (int i=0; i<DivsY*m_nBlkY; i++)
			column[i] = clr;

		if ( n == 0 )
			return;
		if ( (n % m_nBlkX) == 0)
		{
			for (ui16 y=1; y<DivsY*m_nBlkY; y += 3)
				column[y] = RGB565(808080);
		} else
		if ( (n%4) == 0 )
		{
			for (ui16 y=m_nBlkY; y<DivsY*m_nBlkY-1; y += m_nBlkY)
				column[y] = RGB565(808080);
		}
	}

	virtual void OnPaint();
};

class CWndSpectrographTempl : public CWnd
{
public:
	int m_nWidth;
	int m_nHeight;
	int m_nY;

public:
	CWndSpectrographTempl(int nBlkX, int nBlkY)
	{
		m_nWidth = 8*nBlkX;
		m_nHeight = 8*nBlkY;
	}

	virtual void Create(CWnd *pParent, ui16 dwFlags);

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
		{
			Invalidate();
			return;
		}
	}
	virtual void OnPaint();
};

class CWndSpectrumGraph : public CWndSpectrumGraphTempl
{
public:
	CWndSpectrumGraph() : CWndSpectrumGraphTempl(32, 25*2)
	{
	}
};

class CWndSpectrumGraphMini : public CWndSpectrumGraphTempl
{
public:
	CWndSpectrumGraphMini() : CWndSpectrumGraphTempl(32, 16*2)
	{
	}
};

class CWndSpectrumGraphTime : public CWndTimeGraphTempl
{
public:
	CWndSpectrumGraphTime() : CWndTimeGraphTempl(32, 7)
	{
	}
};

class CWndSpectrumGraphSpec : public CWndSpectrographTempl
{
public:
	CWndSpectrumGraphSpec() : CWndSpectrographTempl(32, 16)
	{
	}
};

#endif