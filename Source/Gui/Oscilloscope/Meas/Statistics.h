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

	float GetPeriod() 
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
		return fValue;
	}

	float GetFreq() 
	{ 
		float f = GetPeriod(); 
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
