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

	float GetBaud()
	{
		class _Iterate
		{
		public:
			typedef bool (_Iterate::* TProcessFunc)(int, int);

		private:
			int nBegin, nEnd;
			int nLast, nThresh, nTrigMin, nTrigMax, nTrigState, nNewState;
			int nBit;
			
			bool bDecodeFirst;
			int nDecodeIndex;
			int nDecoded;

		public:
			int nMinPeriod;
			CMeasStatistics* m_pThis;
			int nTotalBits, nTotalTime;
			char strDecode[32];

		public:
			CMeasStatistics* This()
			{
				return m_pThis;
			}
			bool Prepare()
			{
				if ( !This()->GetRange( nBegin, nEnd, This()->m_curRange ) )
					return false;
				if ( This()->m_nRawMax - This()->m_nRawMin < 16 )
					return false;
		
				nThresh = ( This()->m_nRawMax - This()->m_nRawMin ) / 4;
				nTrigMin = This()->m_nRawMin + nThresh;
				nTrigMax = This()->m_nRawMax - nThresh;
				nTrigState = -1, nNewState = -1;
				nLast = -1;
				nMinPeriod = -1;

				nTotalBits = 0;
				nTotalTime = 0;

				bDecodeFirst = true;
				nDecodeIndex = -1;
				nDecoded = 0;
				strDecode[nDecoded] = 0;
				return true;
			}

			bool Do(TProcessFunc ProcessEdge)
			{
				for ( int i = nBegin; i < nEnd; i++ )
				{
					int nSample = BIOS::ADC::GetAt(i);
					if ( This()->m_curSrc == CSettings::Measure::_CH1 )
						nSample = (ui8)((nSample) & 0xff);
					else
						nSample = (ui8)((nSample>>8) & 0xff);

					if ( nSample > nTrigMax )
						nNewState = 1;
					if ( nSample < nTrigMin )
						nNewState = 0;

					if ( nTrigState != -1 && nNewState != nTrigState )
					{
						if ( nLast != -1 )
							if ( !(this->*ProcessEdge)( i-nLast, nNewState ) )
								return false;
						nLast = i;
					}
 					nTrigState = nNewState;
				}
				return true;
			}
			
			bool FindSmallest(int nCount, int nRising)
			{
				if ( nCount <= 2 )
					return true; // noise!?
				if ( nMinPeriod == -1 )
					nMinPeriod = nCount;
				if ( nCount < nMinPeriod )
					nMinPeriod = nCount;
				return true;
			}
			
			bool Sum(int nCount, int nRising)
			{
				if ( nCount <= 2 )
					return true;

				int nBits = 0;
				if ( nCount >= nMinPeriod*9 )
					return false;
				else if ( nCount >= nMinPeriod*8 )
					nBits = 8;
				else if ( nCount >= nMinPeriod*7 )
					nBits = 7;
				else if ( nCount >= nMinPeriod*6 )
					nBits = 6;
				else if ( nCount >= nMinPeriod*5 )
					nBits = 5;
				else if ( nCount >= nMinPeriod*4 )
					nBits = 4;
				else if ( nCount >= nMinPeriod*3 )
					nBits = 3;
				else if ( nCount >= nMinPeriod*2 )
					nBits = 2;
				else if ( nCount >= nMinPeriod*1 )
					nBits = 1;
				else
					return false;
				//int nMin = nMinPeriod;
				nTotalBits += nBits;
				nTotalTime += nCount;
				//BIOS::DBG::Print( "rel=%f %d/%d => %d\n", nCount/(float)nMinPeriod, nCount, nMinPeriod, nBits);
				return true;	
			}
			bool Decode(int nCount, int nRising)
			{
				if ( nCount <= 2 )
					return true;

				if ( bDecodeFirst )
				{
					// push single idle bit
					Push( nRising );
					bDecodeFirst = false;
				}
				int nMin = nMinPeriod;
				for ( int i = 0; i <= nCount-nMin; i += nMin )
					if ( !Push( 1-nRising ) )
						return false;
				return true;
			}

			bool Push(int nBit)
			{
				static int nPrev = 0;
				static int nByte = 0;

				// waiting for start bit
				if ( nDecodeIndex == -1 )
				{
					if ( nPrev == 0 && nBit == 1 )
						nDecodeIndex = 0;
					nPrev = nBit;
					return true;
				}

				//BIOS::DBG::Print("%d", nBit);
				nByte >>= 1;
				if ( !nBit )
					nByte |= 128;
				if ( ++nDecodeIndex >= 8 )
				{
					strDecode[nDecoded++] = nByte;
					strDecode[nDecoded] = 0;
					if ( nDecoded >= (int)sizeof(strDecode) )
						return false;

					//BIOS::DBG::Print(" -> decod=%d '%c'\n", nByte, nByte);
					nDecodeIndex = -1;
				}
				return true;
			}
		} Iterate;

		//BIOS::DBG::Print( "Begin------\n");
		Iterate.m_pThis = this;
		if ( !Iterate.Prepare() )
			return 0.0f;
		Iterate.Do(&_Iterate::FindSmallest);
		Iterate.Do(&_Iterate::Sum);
		Iterate.Do(&_Iterate::Decode);

		if ( Iterate.strDecode[0] )
			BIOS::LCD::Print(20, 30 + ((int)m_curSrc)*16, RGB565(ff0000), RGBTRANS, Iterate.strDecode );
		float fBaudPeriod = (Iterate.nTotalTime / (float) Iterate.nTotalBits) * Settings.Runtime.m_fTimeRes / CWndGraph::BlkX;
		float fBaud = 1.0f / fBaudPeriod;
		return fBaud;
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
