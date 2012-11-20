#include "MenuGenEdit.h"
#include <Source/Gui/MainWnd.h>

/*virtual*/ void CWndMenuGeneratorEdit::Create(CWnd *pParent, ui16 dwFlags) 
{
	CWnd::Create("CWndMenuGeneratorEdit", dwFlags, CRect(280-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
	m_itm[0].Create("Copy\nCH1", RGB565(b00040), 2, this );
	m_itm[1].Create("Normalize", RGB565(b00040), 1, this );
	m_itm[2].Create("~Edit", RGB565(b00040), 1, this );
	m_itm[3].Create("~Load", RGB565(b00040), 1, this );
	m_itm[4].Create("~Save", RGB565(b00040), 1, this );
}

/*virtual*/ void CWndMenuGeneratorEdit::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	if (code == ToWord('L', 'D') )
	{
		MainWnd.m_wndSignalGraph.ShowWindow( SwHide );
		return;
	}

	if (code == ToWord('L', 'E') )
	{
		MainWnd.m_wndSignalGraph.ShowWindow( SwShow );
		return;
	}

	// copy from oscilloscope
	if ( code == ToWord('m', 'o') && pSender == &m_itm[0] )
	{
		int nBegin, nEnd;

		if ( Settings.MarkT1.Mode != CSettings::Marker::_On || 
			 Settings.MarkT2.Mode != CSettings::Marker::_On ||
			 Settings.MarkT2.nValue <= Settings.MarkT1.nValue )
		{
			MainWnd.m_wndGraph.GetCurrentRange( nBegin, nEnd );
		} else
		{
			nBegin = Settings.MarkT1.nValue;
			nEnd = Settings.MarkT2.nValue;
		}

		int nTargetLen = nEnd - nBegin;
		nTargetLen = min(256, nTargetLen);
		CCoreGenerator::GetVolatileLen() = nTargetLen;
		ui8* pVolatile = CCoreGenerator::GetVolatile();
		for ( int i = 0; i < nTargetLen; i++ )
		{
			int nIndex = nBegin + i*(nEnd - nBegin)/nTargetLen;
			BIOS::ADC::SSample Sample;
			Sample.nValue = BIOS::ADC::GetAt( nIndex );
			*pVolatile++ = Sample.CH1;
		}
		Settings.Gen.Wave = CSettings::Generator::_Volatile;
		CCoreGenerator::Update();
		MainWnd.m_wndSignalGraph.Setup( CCoreGenerator::GetRamDac(), CCoreGenerator::GetRamLen() );
		MainWnd.m_wndSignalGraph.Invalidate();
	}

	// normalize
	if ( code == ToWord('m', 'o') && pSender == &m_itm[1] )
	{
		int nLength = CCoreGenerator::GetVolatileLen();
		ui8* pVolatile = CCoreGenerator::GetVolatile();
		int nMin = 0, nMax = 0;
		for ( int i = 0; i < nLength; i++ )
		{
			int nValue = *pVolatile++;
			if ( i == 0 )
			{
				nMin = nMax = nValue;
			} else
			{
				nMin = min(nMin, nValue);
				nMax = max(nMax, nValue);
			}
		}

		pVolatile = CCoreGenerator::GetVolatile();
		for ( int i = 0; i < nLength; i++ )
		{
			int nValue = *pVolatile;
			nValue = (nValue - nMin) * 255 / (nMax - nMin);
			_ASSERT( nValue >= 0 && nValue <= 255 );
			*pVolatile++ = nValue;
		}
		CCoreGenerator::Update();
		MainWnd.m_wndSignalGraph.Invalidate();
	}
}

