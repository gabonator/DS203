#define ENABLE_MODULE_FULLVIEW
ADD_MODULE( "FView", CWndFullView )

#if !defined(__FULLVIEW_H__) && defined(ENABLE_MODULE_FULLVIEW)
#define __FULLVIEW_H__

#include <Source/Framework/Wnd.h>

class CWndFullView : public CWnd
{

		enum {
			FIFO_CLR = 6,     // FIFO指针复位     Value = 1/0: W_PTR/R_PTR
			R_PTR = 0,       // FIFO读地址指针复位
			W_PTR = 1,       // FIFO写地址指针复位
			FIFO_DIGIT = 0
		};

	CWndButton m_btn1;
	//CWndButton m_btn2;

public:
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndFullView", dwFlags | CWnd::WsListener /*| CWnd::WsNoActivate*/, CRect(0, 16, 400, 240), pParent);
		CRect rc1( 240, m_rcClient.bottom-16, 320, m_rcClient.bottom );
		m_btn1.Create("reset W_PTR", CWnd::WsVisible, rc1, this);
		//CRect rc2( 280, m_rcClient.bottom-16, 330, m_rcClient.bottom );
		//m_btn2.Create("R_PTR", CWnd::WsVisible, rc2, this);
	}

	virtual void OnTimer()
	{
		DrawWave();
	}

	virtual void OnPaint()
	{
		DrawWave();
		BIOS::LCD::Bar(0, m_rcClient.bottom-20, 400, m_rcClient.bottom, RGB565(808080));
	}

	void DrawWave()
	{
		ui16 col[200];
		int nLast = 0;

		BIOS::ADC::Copy( BIOS::ADC::GetCount() );
		int nPtr = BIOS::ADC::GetPointer();

		for ( int y = 0; y < (int)COUNT(col); y++ )
			col[y] = RGB565(101010);

		BIOS::LCD::Printf(m_rcClient.right-8*5, m_rcClient.bottom-16-22, RGB565(808080), RGB565(000000), "Draw");

		for (ui16 x=0; x<BIOS::ADC::GetCount(); x++)
		{
			BIOS::ADC::SSample Sample;

			Sample.nValue = BIOS::ADC::GetAt(x);

			int ch1 = Sample.CH1*COUNT(col)/256;
			col[ch1] = RGB565(ffff00);

			int nX = x*400/BIOS::ADC::GetCount();
			if ( x == nPtr )
			{
				for (int i=0; i<(int)COUNT(col)-2; i+=2)
					col[i] = RGB565(ff0000);
			}
			if ( nLast != nX )
			{
				BIOS::LCD::Buffer( nLast, m_rcClient.top, col, COUNT(col));	
				nLast = nX;
				for ( int y = 0; y < (int)COUNT(col); y++ )
					col[y] = RGB565(101010);
			}
		}

		BIOS::LCD::Printf(0, m_rcClient.bottom-16, RGB565(ffffff), RGB565(808080), "adc state = %d (%02x), %d    ", 
			BIOS::ADC::GetState(), BIOS::SYS::Get( FIFO_DIGIT ), nPtr );
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		if (code == ToWord('L', 'D') )
		{
			KillTimer();
			return;
		}

		if (code == ToWord('L', 'E') )
		{
			SetTimer(100);
			return;
		}

		if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
		{
			DrawWave();
			return;
		}
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( nKey == BIOS::KEY::KeyEnter && GetFocus() == &m_btn1 )
		{
			//BIOS::SYS::Set( FIFO_CLR, W_PTR );
			BIOS::ADC::Restart();
		}
		/*
		if ( nKey == BIOS::KEY::KeyEnter &&  GetFocus() == &m_btn2 )
		{
			//BIOS::SYS::Set( FIFO_CLR, R_PTR );
		}
		if ( nKey == BIOS::KEY::KeyEnter &&  GetFocus() == &m_btn3 )
		{
			if ( strcmp(m_btn3.m_pszId, "on") == 0 )
				m_btn3.m_pszId = "off";
			else
				m_btn3.m_pszId = "on";
			m_btn3.Invalidate();
		}*/
		CWnd::OnKey( nKey );
	}

};

#endif