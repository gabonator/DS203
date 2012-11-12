#include <math.h>

class CMeasStatistics
{
	float m_fMin, m_fMax, m_fSum, m_fSumR, m_fSum2;
	int m_nCount;
	int m_nRawMin, m_nRawMax;
	CSettings::Measure::ESource m_curSrc;
	CSettings::Measure::ERange m_curRange;
	CSettings::Calibrator::FastCalc fastCalc1;
	CSettings::Calibrator::FastCalc fastCalc2;

public:
	float GetSamplef( BIOS::ADC::TSample& nSample )
	{
			float fSample = 0;
			if ( m_curSrc == CSettings::Measure::_CH1 )
			{
				nSample = (ui8)((nSample) & 0xff);
				fSample = Settings.CH1Calib.Voltage( fastCalc1, (float)nSample );
			}
			else if ( m_curSrc == CSettings::Measure::_CH2 )
			{
				nSample = (ui8)((nSample>>8) & 0xff);
				fSample = Settings.CH1Calib.Voltage( fastCalc1, (float)nSample );
			}
			else if ( m_curSrc == CSettings::Measure::_Math )
			{
				nSample = MainWnd.m_wndGraph.MathCalc( nSample );
				fSample = nSample / 32.0f;	// matches the vertical grid
			} else
			_ASSERT( 0 );
		return fSample;
	}

	int GetSample( BIOS::ADC::TSample nSample )
	{
			if ( m_curSrc == CSettings::Measure::_CH1 )
			{
				nSample = (ui8)((nSample) & 0xff);
			}
			else if ( m_curSrc == CSettings::Measure::_CH2 )
			{
				nSample = (ui8)((nSample>>8) & 0xff);
			}
			else if ( m_curSrc == CSettings::Measure::_Math )
			{
				nSample = MainWnd.m_wndGraph.MathCalc( nSample );
			}
		return nSample;
	}

	bool Process( CSettings::Measure::ESource src, CSettings::Measure::ERange range )
	{
		int nBegin = 0, nEnd = 0;
		if ( !GetRange( nBegin, nEnd, range ) )
			return false;

		if ( src == CSettings::Measure::_CH1 )
			Settings.CH1Calib.Prepare( &Settings.CH1, fastCalc1 );
		if ( src == CSettings::Measure::_CH2 )
			Settings.CH2Calib.Prepare( &Settings.CH2, fastCalc2 );
		if ( src == CSettings::Measure::_Math )
		{
			Settings.CH1Calib.Prepare( &Settings.CH1, fastCalc1 );
			Settings.CH2Calib.Prepare( &Settings.CH2, fastCalc2 );
			MainWnd.m_wndGraph.MathSetup( &fastCalc1, &fastCalc2 );
		}

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
			BIOS::ADC::TSample nSample = BIOS::ADC::GetAt( i );
			float fSample, fSample2;
			fSample = GetSamplef( nSample );
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
			int nSample_ = BIOS::ADC::GetAt(i);
			int nSample = GetSample( nSample_ );

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
		if ( m_nCount == 0 )
			return 0;
		float fAux = (m_fSum2 - m_fSum*m_fSum/2) / m_nCount;
		if ( abs(fAux) > 200 )
			return 0;
		return fAux;
	}

	float GetBaud()
	{
		class _Iterate
		{
		public:
			typedef bool (_Iterate::* TProcessFunc)(int, int);

		private:
			int nBegin, nEnd;
			int nLast, nThresh, nTrigMin, nTrigMax;
			int nBit;
			
			int nDecodeIndex;

		public:
			int nMinPeriod;
			CMeasStatistics* m_pThis;
			int nTotalBits, nTotalTime;
			int nDecoded;
			unsigned char strDecode[32];
			int nIdle;

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
				nLast = -1;
				nMinPeriod = -1;

				nTotalBits = 0;
				nTotalTime = 0;

				nDecodeIndex = -1;
				nDecoded = 0;
				strDecode[nDecoded] = 0;
				return true;
			}

			void Average()
			{
				int nHigh = 0, nLow = 0;
				for ( int i = nBegin; i < nEnd; i++ )
				{
					int nSample = BIOS::ADC::GetAt(i);
					nSample = This()->GetSample( nSample );

					if ( nSample > nTrigMax )
						nHigh++;
					if ( nSample < nTrigMin )
						nLow++;
				}
				nIdle = nHigh > nLow ? 1 : 0;
			}

			bool Do(TProcessFunc ProcessEdge)
			{
				int nNewState = -1, nTrigState = -1;
				for ( int i = nBegin; i < nEnd; i++ )
				{
					int nSample = BIOS::ADC::GetAt(i);
					nSample = This()->GetSample( nSample );

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
				//if ( !(this->*ProcessEdge)( nEnd-nLast, 1-nNewState ) )
				//	return false;
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

				int nMin = nMinPeriod;
				for ( int i = nMin; i <= nCount; i += nMin )
					if ( !Push( nIdle ? 1-nRising : nRising) )
						return false;
				return true;
			}

			bool Push(int nBit)
			{
				static int nPrev = 0;
				static int nByte = 0;

//				BIOS::DBG::Print("%d", nBit);
				// waiting for start bit
				if ( nDecodeIndex == -1 )
				{
					if ( nPrev == 1 && nBit == 0 )
					{
						nDecodeIndex = 0;
//						BIOS::DBG::Print("<");
					}
					nPrev = nBit;
					return true;
				}

				nByte >>= 1;
				if ( nBit )
					nByte |= 128;

				if ( ++nDecodeIndex >= 8 )
				{
//					BIOS::DBG::Print(">");
					if ( nDecoded <= (int)sizeof(strDecode) )
					{
						strDecode[nDecoded++] = nByte;
					} else
						return false;

//					BIOS::DBG::Print(" -> decod=%02x %d '%c'\n", nByte, nByte, nByte);
					nDecodeIndex = -1;
					nByte = 0;
				}
				return true;
			}
		} Iterate;

		//BIOS::DBG::Print( "Begin------\n");
		Iterate.m_pThis = this;
		if ( !Iterate.Prepare() )
			return 0.0f;
		Iterate.Average();
		Iterate.Do(&_Iterate::FindSmallest);
		Iterate.Do(&_Iterate::Sum);
		
		Iterate.Push(1);	// idle before start bit
		Iterate.Do(&_Iterate::Decode);
		for ( int i = 0; i < 8; i++ )
			Iterate.Push(0);	// finish byte when only single byte sent

		float fBaudPeriod = (Iterate.nTotalTime / (float) Iterate.nTotalBits) * Settings.Runtime.m_fTimeRes / CWndGraph::BlkX;
		float fBaud = 1.0f / fBaudPeriod;

		bool bMidi = (fBaud > 30000.0f) && (fBaud < 32000.0f);
		bool bHex = bMidi;
		// display decoded characters
		if ( Iterate.nDecoded > 0 )
		{
			int x = 20;
			int y = 30 + ((int)m_curSrc)*16;
			if ( bMidi )
				x += BIOS::LCD::Print(x, y, RGB565(ffffff), 0x0101, "MIDI: " );
			else if ( Iterate.nIdle == 1 )
				x += BIOS::LCD::Print(x, y, RGB565(ffffff), 0x0101, "UART-TTL: " );
			else
				x += BIOS::LCD::Print(x, y, RGB565(ffffff), 0x0101, "UART-RS232: " );

			if ( bMidi )
			{
				for ( int i = 0; i < Iterate.nDecoded; i++ )
				{
					int ch = Iterate.strDecode[i];
					int atr1 = i < Iterate.nDecoded - 1 ? Iterate.strDecode[i+1] : -1;
					int atr2 = i < Iterate.nDecoded - 2 ? Iterate.strDecode[i+2] : -1;

					if ( (ch & 0xf0) >= 0x80 )
					{
						if ( (ch & 0x0f) > 0 )
							x += 8*BIOS::LCD::Printf(x, y, RGB565(ff0000), 0x0101, "%d_", 
								ch & 0x0f );
					}
					if ( (ch & 0xf0) == 0x90 )
					{
						if ( atr2 == 0 )
							x += 8*BIOS::LCD::Printf(x, y, RGB565(ff0000), 0x0101, "NoteOff(%s)", 
								CUtils::MidiNote(atr1), atr2);
						else
							x += 8*BIOS::LCD::Printf(x, y, RGB565(ff0000), 0x0101, "NoteOn(%s,%d)", 
								CUtils::MidiNote(atr1), atr2);
						i += 2;
					}
					if ( (ch & 0xf0) == 0x80 )
					{
						x += 8*BIOS::LCD::Printf(x, y, RGB565(ff0000), 0x0101, "NoteOff(%s)", 
							ch & 0x0f, CUtils::MidiNote(atr1));
						i += 2;
					}
					if ( (ch & 0xf0) == 0xE0 )
					{
						int nValue = (atr1|(atr2<<7)) - 8192;
						x += 8*BIOS::LCD::Printf(x, y, RGB565(ff0000), 0x0101, "Pitch(%d)", 
							nValue);
						i += 2;
					}
					if ( (ch & 0xf0) == 0xB0 )
					{
						x += 8*BIOS::LCD::Printf(x, y, RGB565(ff0000), 0x0101, "Ctl(%d,%d)", 
							atr1, atr2);
						i += 2;
					}
					if ( (ch & 0xf0) == 0xC0 )
					{
						int nInstr = 0;
						if ( atr1 >= 0 )
							nInstr += atr1;
						if ( atr2 >= 0 )
							nInstr += atr1*128;

						x += 8*BIOS::LCD::Printf(x, y, RGB565(ff0000), 0x0101, "Patch(%d)", 
							nInstr);
						i += 2;
					}

					x += 8;

				}
			} else
			if ( bHex )
			{
				for ( int i = 0; i < Iterate.nDecoded; i++ )
				{
					unsigned char ch = Iterate.strDecode[i];
					x += 8*BIOS::LCD::Printf(x, y, RGB565(ff0000), 0x0101, "%02x ", ch );
				}
			} else
			{
				for ( int i = 0; i < Iterate.nDecoded; i++ )
				{
					unsigned char ch = Iterate.strDecode[i];
					if ( ch >= ' ' && ch <= 127 )	// readable char
						x += 8*BIOS::LCD::Printf(x, y, RGB565(ff0000), 0x0101, "%c", ch );
					else
						x += 8*BIOS::LCD::Printf(x, y, RGB565(ff0000), 0x0101, "[%02x]", ch );
				}
			}
		}
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
