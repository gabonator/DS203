#define ENABLE_MODULE_ZOOMVIEW
ADD_MODULE( "Zoom", CWndZoomView )

#if !defined(__ZOOMVIEW_H__) && defined(ENABLE_MODULE_ZOOMVIEW)
#define __ZOOMVIEW_H__

#include <Source/Framework/Wnd.h>

class CWndZoomView : public CWnd
{
	CWndButton m_btnMove;
	CWndButton m_btnZoom;

	int nViewBegin;
	int nViewEnd;

public:
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndZoomView", dwFlags | CWnd::WsListener /*| CWnd::WsNoActivate*/, CRect(0, 16, 400, 240), pParent);

		CRect rcZoom( 300, m_rcClient.bottom-16, 330, m_rcClient.bottom );
		m_btnZoom.Create("Zoom", CWnd::WsVisible, rcZoom, this);

		CRect rcMove( 360, m_rcClient.bottom-16, 390, m_rcClient.bottom );
		m_btnMove.Create("Move", CWnd::WsVisible, rcMove, this);

		nViewBegin = 0;
		nViewEnd = BIOS::ADC::GetCount()-1;
	}

	virtual void OnTimer()
	{
		DrawWave();
	}

	virtual void OnPaint()
	{
		DrawWave();
		DrawProgress();
		BIOS::LCD::Bar(0, m_rcClient.bottom-20, 400, m_rcClient.bottom, RGB565(808080));
	}

	int GetInterpolatedLinear(int nIndex)
	{
		int nLeft = nIndex>>8;
		int nRight = nLeft+1;
		int nPart = nIndex & 0xff;

		int nValLeft = BIOS::ADC::GetAt( nLeft ) & 0xff;
		int nValRight = BIOS::ADC::GetAt( nRight ) & 0xff;
		return nValLeft + (nValRight - nValLeft)*nPart/256;
	}

	int GetInterpolatedSincAnalytic(int nIndex)
	{
		int nMid = nIndex/256;
		int nMin = nMid-10;
		int nMax = nMid+10;
		nMin = max(0, nMin);
		nMax = min(4095, nMax);

		float fSum = 0;
		for (int i=nMin; i<=nMax; i++)
		{
			int nSample = BIOS::ADC::GetAt( i ) & 0xff;
			float fRelTime = i - nIndex / 256.0f;
			float fSi;
			if ( fRelTime == 0 )
				fSi = 1;
			else
			{
				fRelTime *= 3.141592f;
				fSi = sin(fRelTime)/fRelTime;
			}
			fSum += fSi * nSample;
		}
		int nSum = (int) fSum;
		UTILS.Clamp<int>( nSum, 0, 255 );
		return nSum;
	}

	int GetInterpolated(int nIndex)
	{
		int nMid = nIndex/256;
		int nMin = nMid-10;
		int nMax = nMid+10;
		nMin = max(0, nMin);
		nMax = min(4095, nMax);
		
		float fSin = sin( (nIndex & 0xff)/256.0f * 3.14159265f );

		float fSum = 0;
		for (int i=nMin; i<=nMax; i++)
		{
			int nSample = BIOS::ADC::GetAt( i ) & 0xff;
			float fRelTime = i - nIndex / 256.0f;
			float fSi;
			if ( fRelTime == 0 )
				fSi = 1;
			else
			{
				fRelTime *= 3.141592f;
				if ((i-nMid+1)&1)
					fSi = -fSin/fRelTime;
				else
					fSi = fSin/fRelTime;
			}
			fSum += fSi * nSample;
		}
		int nSum = (int) fSum;
		UTILS.Clamp<int>( nSum, 0, 255 );
		return nSum;
	}

	void DrawWave()
	{
		ui16 col[198];

		BIOS::ADC::Copy( BIOS::ADC::GetCount() );
		int nPtr = BIOS::ADC::GetPointer();

		for ( int y = 0; y < (int)COUNT(col); y++ )
			col[y] = RGB565(101010);

		BIOS::LCD::Printf(m_rcClient.right-8*5, m_rcClient.bottom-16-22, RGB565(808080), RGB565(000000), "Draw");

		if ( nViewEnd - nViewBegin + 1 > 400 )
		{
			int __x = (nPtr - nViewBegin)*400/(nViewEnd - nViewBegin+1);
			int nLastVal = -1;
			for (int x=0; x<400; x++)
			{
				int nBegin = nViewBegin + ( nViewEnd - nViewBegin + 1 ) * x / 400;
				int nEnd = nViewBegin + ( nViewEnd - nViewBegin + 1 ) * (x+1) / 400;
				int nMin = -1, nMax = -1;

				for ( int y = 0; y < (int)COUNT(col); y++ )
					col[y] = RGB565(101010);

				if ( x == __x )
				{
					for (int i=0; i<(int)COUNT(col)-2; i+=2)
						col[i] = RGB565(ff0000);
				} 

				for ( int i=nBegin; i<nEnd; i++ )
				{
					BIOS::ADC::SSample Sample;
					Sample.nValue = BIOS::ADC::GetAt(i);
					int ch1 = Sample.CH1*COUNT(col)/256;
					col[ch1] = RGB565(ffff00);

					if ( i==nBegin )
						nMin = nMax = ch1;
					else
					{
						nMin = min(nMin, ch1);
						nMax = max(nMax, ch1);
					}
				}
				
				if ( x > 0 )
				{
					if ( nLastVal > nMax )
						nMax = nLastVal;
					if ( nLastVal < nMin )
						nMin = nLastVal;
				}
				nLastVal = (nMin + nMax)/2;

				for ( int i=nMin; i<nMax; i++ )
					if ( col[i] == RGB565(101010) )
						col[i] = RGB565(808000);

				BIOS::LCD::Buffer( x, m_rcClient.top, col, COUNT(col));	
			}
		} else
		{
			int __x = (nPtr - nViewBegin)*400/(nViewEnd - nViewBegin+1);
			int nLastIndex = -1;
			int nLastVal = 0;
			int nMark = -1;
			for (int x=0; x<=400; x++)
			{
				BIOS::ADC::SSample Sample;

				int nIndex = x*(nViewEnd - nViewBegin+1)/400 + nViewBegin;
				int nFixIndex = x*(nViewEnd - nViewBegin+1)*256/400 + nViewBegin*256;
				bool bMatch = (nLastIndex != nIndex);
				nLastIndex = nIndex;

				if ( bMatch )
					Sample.nValue = BIOS::ADC::GetAt( nIndex );
				else
					Sample.CH1 = GetInterpolated(nFixIndex);

				int ch1 = Sample.CH1*COUNT(col)/256;
				
				for ( int y = 0; y < (int)COUNT(col); y++ )
					col[y] = RGB565(101010);

				if ( x == __x )
				{
					for (int i=0; i<(int)COUNT(col)-2; i+=2)
						col[i] = RGB565(ff0000);
				} 

				if ( x == 0 )
					nLastVal = ch1;
				else if ( nLastVal < ch1 )
					for (; nLastVal<ch1; nLastVal++)
						col[nLastVal] = RGB565(808000);
				else
					for (; nLastVal>ch1; nLastVal--)
						col[nLastVal] = RGB565(808000);
				if ( bMatch )
				{
					col[ch1] = RGB565(ffff00);
					if ( ch1 > 0 )
						col[ch1-1] = RGB565(ffff00);
				}
				else
					col[ch1] = RGB565(808000);

				if ( nMark >= 0 )
				{
					col[nMark] = RGB565(ffff00);
					if ( nMark > 0 )
						col[nMark-1] = RGB565(ffff00);
						
					nMark = ch1;
					nMark = -1;
				}
				if ( bMatch )
					nMark = ch1;

				BIOS::LCD::Buffer( x, m_rcClient.top, col, COUNT(col));	
			}
		}

		BIOS::LCD::Printf(0, m_rcClient.bottom-16, RGB565(ffffff), RGB565(808080), "view %d-%d   ", 
			nViewBegin, nViewEnd );
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
			SetTimer(200);
			return;
		}

		if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
		{
			DrawWave();
			return;
		}
	}

	void DrawProgress()
	{
		CRect rcBar;
		rcBar.top = m_rcClient.bottom - 20 - 4;
		rcBar.bottom = m_rcClient.bottom - 20 - 1;
		rcBar.left = (ui16)(nViewBegin*400/BIOS::ADC::GetCount());
		rcBar.right = (ui16)((nViewEnd+1)*400/BIOS::ADC::GetCount());
		BIOS::LCD::Bar( rcBar, RGB565(ffffff) );
		if ( rcBar.left > 0 )
		{
			CRect rcLeft(rcBar);
			rcLeft.right = rcLeft.left;
			rcLeft.left = 0;
			BIOS::LCD::Bar( rcLeft, RGB565(b0b0b0) );
		}
		if ( rcBar.right < 400 )
		{
			CRect rcRight(rcBar);
			rcRight.left = rcRight.right;
			rcRight.right = 400;
			BIOS::LCD::Bar( rcRight, RGB565(b0b0b0) );
		}
	}

	virtual void OnKey(ui16 nKey)
	{
		if ( nKey == BIOS::KEY::KeyEnter && GetFocus() == &m_btnMove )
		{
			nViewBegin = 0;
			nViewEnd = BIOS::ADC::GetCount();
			BIOS::ADC::Restart();
		}

		if ( nKey == BIOS::KEY::KeyLeft && GetFocus() == &m_btnMove )
		{
			int nWidth = nViewEnd - nViewBegin;
			int nMove = max(1, nWidth/16);
			nViewBegin -= nMove;            
			UTILS.Clamp<int>( nViewBegin, 0, BIOS::ADC::GetCount()-1 );
			nViewEnd = nViewBegin + nWidth;
			DrawWave();
			DrawProgress();
		}

		if ( nKey == BIOS::KEY::KeyRight && GetFocus() == &m_btnMove )
		{
			int nWidth = nViewEnd - nViewBegin;
			int nMove = max(1, nWidth/16);
			nViewEnd += nMove;            
			UTILS.Clamp<int>( nViewEnd, 0, BIOS::ADC::GetCount()-1 );
			nViewBegin = nViewEnd - nWidth;
			DrawWave();
			DrawProgress();
		}

		if ( nKey == BIOS::KEY::KeyLeft && GetFocus() == &m_btnZoom )
		{
			int nWidth = nViewEnd - nViewBegin;
			int nNewWidth = nWidth / 2;
			int nCenter = ( nViewBegin + nViewEnd ) / 2;
			nViewBegin = nCenter - nNewWidth/2;
			nViewEnd = nCenter + nNewWidth/2;
			
			UTILS.Clamp<int>( nViewBegin, 0, BIOS::ADC::GetCount()-1 );
			UTILS.Clamp<int>( nViewEnd, 0, BIOS::ADC::GetCount()-1 );
			DrawWave();
			DrawProgress();
		}

		if ( nKey == BIOS::KEY::KeyRight && GetFocus() == &m_btnZoom )
		{
			int nWidth = nViewEnd - nViewBegin;
			int nNewWidth = 1 + nWidth * 2;
			int nCenter = ( nViewBegin + nViewEnd ) / 2;
			nViewBegin = nCenter - nNewWidth/2;
			nViewEnd = nCenter + (nNewWidth+1)/2;
			
			UTILS.Clamp<int>( nViewBegin, 0, BIOS::ADC::GetCount()-1 );
			UTILS.Clamp<int>( nViewEnd, 0, BIOS::ADC::GetCount()-1 );
			DrawWave();
			DrawProgress();
		}
		CWnd::OnKey( nKey );
	}

};

#endif