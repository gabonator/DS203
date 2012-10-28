#include "MenuMeas.h"

#include <Source/Gui/MainWnd.h>
#include <math.h>

class CMeasStatistics
{
	float m_fMin, m_fMax, m_fSum, m_fSumR, m_fSum2;
	int m_nCount;
	int m_nRawMin, m_nRawMax;
	CSettings::Measure::ESource m_curSrc;
	CSettings::Measure::ERange m_curRange;

public:
	bool Process( CSettings::Measure::ESource src, CSettings::Measure::ERange range )
	{
		int nBegin = 0, nEnd = 0;
		if ( !GetRange( nBegin, nEnd, range ) )
			return false;

		CSettings::Calibrator::FastCalc fastCalc;

		if ( src == CSettings::Measure::_CH1 )
			Settings.CH1Calib.Prepare( &Settings.CH1, fastCalc );
		else
			Settings.CH2Calib.Prepare( &Settings.CH2, fastCalc );

		m_curSrc = src;
		m_curRange = range;
		m_nRawMin = 0;
		m_nRawMax = 0;
		m_fMin = 0;
		m_fMax = 0;
		m_fSum = 0;
		m_fSumR = 0;
		m_fSum2 = 0;
		m_nCount = 0;

		for ( int i = nBegin; i < nEnd; i++ )
		{
			unsigned int nSample = BIOS::ADC::GetAt( i );
			float fSample, fSample2;
			
			if ( src == CSettings::Measure::_CH1 )
				nSample = (ui8)((nSample) & 0xff);
			else
				nSample = (ui8)((nSample>>8) & 0xff);
			
			fSample = Settings.CH1Calib.Voltage( fastCalc, (float)nSample );
			fSample2 = fSample * fSample;

			if ( i == nBegin )
			{
				m_fMin = m_fMax = fSample;
				m_nRawMin = m_nRawMax = nSample;
			}

			if ( fSample > m_fMax )
				m_fMax = fSample;
			if ( fSample < m_fMin )
				m_fMin = fSample;
			if ( (int)nSample > m_nRawMax )
				m_nRawMax = nSample;
			if ( (int)nSample < m_nRawMin )
				m_nRawMin = nSample;
			
			m_fSum += fSample;
			m_fSum2 += fSample2;
			m_fSumR += abs(fSample);
			
			m_nCount++;
		}

		return m_nCount > 0 ? true : false;
	}

	float GetFreq() 
	{
		int nBegin = 0, nEnd = 0;
		if ( !GetRange( nBegin, nEnd, m_curRange ) )
			return 0;
		if ( m_nRawMax - m_nRawMin < 16 )
			return 0;
		
		int nThresh = ( m_nRawMax - m_nRawMin ) / 4;
		int nTrigMin = m_nRawMin + nThresh;
		int nTrigMax = m_nRawMax - nThresh;
		int nTrigState = -1, nNewState = -1;
		int nLastRise = -1, nLastFall = -1;
		int nTotalRise = 0, nTotalFall = 0;
		int nSumRise = 0, nSumFall = 0;

		for ( int i = nBegin; i < nEnd; i++ )
		{
			int nSample = BIOS::ADC::GetAt(i);
			if ( m_curSrc == CSettings::Measure::_CH1 )
				nSample = (ui8)((nSample) & 0xff);
			else
				nSample = (ui8)((nSample>>8) & 0xff);

			if ( nSample > nTrigMax )
				nNewState = 1;
			if ( nSample < nTrigMin )
				nNewState = 0;

			if ( nTrigState != -1 && nNewState != nTrigState )
			{
				// trigger changing it's output
				if ( nNewState == 1 ) // rising edge
				{
					if ( nLastRise != -1 )
					{
						nSumRise += i - nLastRise; // period length
						nTotalRise++;
					}
					nLastRise = i;
				} else
				{	// falling edge
					if ( nLastFall != -1 )
					{
						nSumFall += i - nLastFall; // period length
						nTotalFall++;
					}
					nLastFall = i;
				}
			}
 			nTrigState = nNewState;
		}
			
		if ( nTotalRise + nTotalFall == 0 )
			return 0;

		float fAvgPeriod = (nSumRise + nSumFall) / (float)(nTotalRise + nTotalFall);
		// period in samples -> time in seconds
		float fTimeRes = Settings.Runtime.m_fTimeRes / CWndGraph::BlkX;
		float fValue = fTimeRes * fAvgPeriod;
		if ( fValue == 0 )
			return 0;
		return 1.0f / fValue;
	}

	float GetPeriod() 
	{ 
		float f = GetFreq(); 
		if ( f <= 0 )
			return 0;
		return 1.0f / f;
	}

	float GetSigma() // variance
	{
		// subject to rounding error, but faster than two-pass formula
		return (m_fSum2 - m_fSum*m_fSum/2) / m_nCount;
	}

	float GetMin() { return m_fMin; }
	float GetMax() { return m_fMax; }
	float GetAvg() { return m_fSum/m_nCount; }
	float GetRms() { return sqrt(m_fSum2/m_nCount); }
	float GetRectAvg() { return m_fSumR/m_nCount; }
	float GetVpp() { return m_fMax - m_fMin; }
	float GetFormFactor() { return GetRms() / GetRectAvg(); }
	float GetDispersion() { float f = GetSigma(); return f*f; }

private:
	bool GetRange( int& nBegin, int& nEnd, CSettings::Measure::ERange range )
	{
		switch ( range )
		{
			case CSettings::Measure::_View:
				MainWnd.m_wndGraph.GetCurrentRange( nBegin, nEnd );
				break;
			case CSettings::Measure::_Selection:
				if ( Settings.MarkT1.Mode != CSettings::Marker::_On || 
					 Settings.MarkT2.Mode != CSettings::Marker::_On ||
					 Settings.MarkT2.nValue <= Settings.MarkT1.nValue )
				{
					return false;
				}
				nBegin = Settings.MarkT1.nValue;
				nEnd = Settings.MarkT2.nValue;
				break;
			case CSettings::Measure::_All:
				nBegin = 0;
				nEnd = BIOS::ADC::GetCount();
				break;
		}
		return true;
	}

};

CMeasStatistics		m_Stat;

/*virtual*/ void CWndMenuMeas::Create(CWnd *pParent, ui16 dwFlags) 
{
	CWnd::Create("CWndMenuMeas", dwFlags | CWnd::WsListener, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
	m_itmMeas[0].Create( &Settings.Meas[0], this );
	m_itmMeas[1].Create( &Settings.Meas[1], this );
	m_itmMeas[2].Create( &Settings.Meas[2], this );
	m_itmMeas[3].Create( &Settings.Meas[3], this );
	m_itmMeas[4].Create( &Settings.Meas[4], this );
	m_itmMeas[5].Create( &Settings.Meas[5], this );
}

/*virtual*/ void CWndMenuMeas::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	// LAYOUT ENABLE/DISABLE FROM TOP MENU BAR
	if (code == ToWord('L', 'D') )
	{
		MainWnd.m_wndGraph.ShowWindow( SwHide );
		MainWnd.m_wndInfoBar.ShowWindow( SwHide );
		MainWnd.m_wndLReferences.ShowWindow( SwHide );
		MainWnd.m_wndTReferences.ShowWindow( SwHide );
		return;
	}

	if (code == ToWord('L', 'E') )
	{
		MainWnd.m_wndGraph.ShowWindow( SwShow );
		MainWnd.m_wndInfoBar.ShowWindow( SwShow );
		MainWnd.m_wndLReferences.ShowWindow( SwShow );
		MainWnd.m_wndTReferences.ShowWindow( SwShow );
		return;
	}

	// new waveform acquired, update the Y values 	
	if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
	{
		_UpdateAll();
		return;
	}

	// something was changed in listbox
	if (code == ToWord('u', 'p') )
	{
		if ( m_wndListMeas.m_pMeas == m_itmMeas[0].m_pMeas )
			m_itmMeas[0].Invalidate();
		if ( m_wndListMeas.m_pMeas == m_itmMeas[1].m_pMeas )
			m_itmMeas[1].Invalidate();
		if ( m_wndListMeas.m_pMeas == m_itmMeas[2].m_pMeas )
			m_itmMeas[2].Invalidate();
		if ( m_wndListMeas.m_pMeas == m_itmMeas[3].m_pMeas )
			m_itmMeas[3].Invalidate();
		if ( m_wndListMeas.m_pMeas == m_itmMeas[4].m_pMeas )
			m_itmMeas[4].Invalidate();
		if ( m_wndListMeas.m_pMeas == m_itmMeas[5].m_pMeas )
			m_itmMeas[5].Invalidate();
		return;
	}

	// Show listbox - cursor details
	if (code == ToWord('m', 'c') )	
	{
		m_wndListMeas.Create( (CSettings::Measure*)(NATIVEPTR)data, this );
		m_wndListMeas.StartModal( &m_wndListMeas.m_itmEnabled );
		return;
	}
	if ( code == ToWord('o', 'k') && pSender == &m_wndListMeas )
	{
		// done
		m_wndListMeas.StopModal();
		return;
	}
	if ( code == ToWord('e', 'x') && pSender == &m_wndListMeas )
	{
		// done
		m_wndListMeas.StopModal();
		return;
	}
}

void CWndMenuMeas::_UpdateAll()
{
	for ( int nFilter = CSettings::Measure::_CH1; nFilter <= CSettings::Measure::_CH2; nFilter++ )
	{
		int nLastRange = -1;

		for ( int i = 0; i < (int)COUNT( m_itmMeas ); i++ )
		{
			CSettings::Measure& meas = Settings.Meas[i];
			if ( meas.Enabled == CSettings::Measure::_Off )
				continue;
			if ( meas.Source != nFilter )
				continue;

			if ( nLastRange != meas.Range )
			{
				if ( !m_Stat.Process( (CSettings::Measure::ESource) nFilter, meas.Range ) )
				{
					continue;
				}
				nLastRange = meas.Range;
			}
			
			float fPrev = meas.fValue;
			meas.fValue = -1;
			switch ( meas.Type )
			{
				case CSettings::Measure::_Min:		meas.fValue = m_Stat.GetMin(); break;
				case CSettings::Measure::_Max:		meas.fValue = m_Stat.GetMax(); break;
				case CSettings::Measure::_Avg:		meas.fValue = m_Stat.GetAvg(); break;
				case CSettings::Measure::_Rms:		meas.fValue = m_Stat.GetRms(); break;
				case CSettings::Measure::_RectAvg:	meas.fValue = m_Stat.GetRectAvg(); break;
				case CSettings::Measure::_Vpp:		meas.fValue = m_Stat.GetVpp(); break;
				case CSettings::Measure::_Freq:		meas.fValue = m_Stat.GetFreq() / 1000.0f; break; // khz
				case CSettings::Measure::_Period:	meas.fValue = m_Stat.GetPeriod() * 1000.0f; break; // ms
				case CSettings::Measure::_FormFactor:
					meas.fValue = m_Stat.GetFormFactor(); break;
				case CSettings::Measure::_Sigma:	meas.fValue = m_Stat.GetSigma(); break;
				case CSettings::Measure::_Dispersion:
					meas.fValue = m_Stat.GetDispersion(); break;
				default:
					_ASSERT( !!!"Unknown measurement type" );
			}
			if ( fPrev != meas.fValue )
				m_itmMeas[i].Invalidate();
		}
	}
}

