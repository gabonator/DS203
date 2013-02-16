#define ENABLE_MODULE_GEIGER
ADD_MODULE( "Geiger", CWndGeiger )

#if !defined(__GEIGER_H__) && defined(ENABLE_MODULE_GEIGER)
#define __GEIGER_H__

#include <Source/Framework/Wnd.h>

class CWndGeiger : public CWnd
{
	enum {
		TIMEMUL = 1
	};

	char m_arrCounts[60];
	int m_nTickStart;
	int m_nLastSlot;
	int m_nCounter;

public:
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndGeiger", dwFlags | CWnd::WsListener | CWnd::WsNoActivate, CRect(0, 16, 400, 240), pParent);
	}

	virtual void OnTimer()
	{
		int nCurSlot = GetSlot();
		if ( nCurSlot != m_nLastSlot )
		{
			m_arrCounts[nCurSlot] = 0;
			m_nLastSlot = nCurSlot;
		}
		Invalidate();
	}

	int GetSlot()
	{
		return (((BIOS::SYS::GetTick() - m_nTickStart)/1000)/TIMEMUL)%60;
	}

	int GetSum()
	{
		int nSum = 0;
		for (int i=0; i<60; i++)
			nSum += m_arrCounts[i];
		return nSum;
	}

	int GetSum2(int q)
	{
		int nSum = 0;
		for (int i=0; i<60; i++)
			if ( i != q )
				nSum += m_arrCounts[i];
		return nSum;
	}

	void Clear()
	{
		memset( m_arrCounts, 0, 60 );
	}

	virtual void OnPaint()
	{
		int lTick = BIOS::SYS::GetTick();
		int nPassed = ((BIOS::SYS::GetTick() - m_nTickStart)/1000)/TIMEMUL;
		if ( nPassed > 60 )
			nPassed = 60;
		if ( nPassed <= 0 )
			nPassed = 1;
		int nPassed2 = min( 59, nPassed ); //(nPassed > 1) ? (nPassed-1) : 1;

		int nSlot = GetSlot();
		int nSum2 = GetSum2(nSlot);
		float fHPS = (float)nSum2*60.0f/TIMEMUL/nPassed2;
		float fRate = fHPS / 16.0f * 0.09f;


		static int nLastSend = 0;
		if ( BIOS::SYS::GetTick() - nLastSend > 5000 )
		{
			nLastSend = BIOS::SYS::GetTick();
			char msg[20];
			BIOS::DBG::sprintf(msg, "\nGeiger=%d\n", m_nCounter);
			BIOS::SERIAL::Send(msg);  
			BIOS::DBG::sprintf(msg, "Rate=%f\n", fRate);
			BIOS::SERIAL::Send(msg);
		}

		// 0.186 => 0.09
		BIOS::LCD::Printf( 20, 30, RGB565(ffffff), RGB565(000000), "Count = %d   ", GetSum() );
		BIOS::LCD::Printf( 20, 30+16, RGB565(ffffff), RGB565(000000), "HPM = %f   ", (float)fHPS );
		BIOS::LCD::Printf( 20, 30+32, RGB565(ffffff), RGB565(000000), "Dose rate = %f " "\xE6" "Sv/h  ", (float)fRate );
		char num[3]= {0, 0, 0};
		for ( int j=0; j<2; j++)
		{
			int i = (j + nSlot+59)%60;
			if ( m_arrCounts[i] < 10 )
			{
				num[0] = '0' + m_arrCounts[i];
				num[1] = 0;
			}
			else
			{
				num[0] = '0';
				num[1] = '0';
				num[2] = 0;
				int nC = m_arrCounts[i];
				while ( nC >= 10 )
				{
					num[0]++;
					nC -= 10;
				}
				num[1] +=nC;
			}
			BIOS::LCD::Print( 20 + (i%10)*30, 100+(i/10)*16, RGB565(ffffff), i==nSlot ? RGB565(0000b0) : RGB565(000000), num );	
		}
		int lPassed = BIOS::SYS::GetTick() - lTick;

		BIOS::LCD::Printf(340, 30, RGB565(808080), 0, "%d ms", lPassed);
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
			m_nTickStart = BIOS::SYS::GetTick();
			m_nLastSlot = 0;
			m_nCounter = 0;
			Clear();
			SetTimer(330);
			return;
		}

		if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
		{
			m_nCounter++;
			BIOS::SYS::Beep(20);
			int nCurSlot = GetSlot();
			if ( nCurSlot != m_nLastSlot )
			{
				m_arrCounts[nCurSlot] = 0;
				m_nLastSlot = nCurSlot;
			}
			m_arrCounts[nCurSlot]++;
			return;
		}
	}

	virtual void OnKey(ui16 nKey)
	{
		CWnd::OnKey( nKey );
	}

};

#endif