#ifndef __MEASURE_H__
#define __MEASURE_H__

#include <Source/Core/Settings.h>

class CMeasStatistics
{
	friend class CSerialDecoder;

	float m_fMin, m_fMax, m_fSum, m_fSumR, m_fSum2;
	int m_nCount;
	int m_nRawMin, m_nRawMax;
	CSettings::Measure::ESource m_curSrc;
	CSettings::Measure::ERange m_curRange;
	CSettings::Calibrator::FastCalc fastCalc1;
	CSettings::Calibrator::FastCalc fastCalc2;

public:
	bool Process( CSettings::Measure::ESource src, CSettings::Measure::ERange range );
	float GetPeriod();
	float GetFreq(); 
	float GetSigma(); // variance
	float GetBaud();
	float GetMin();
	float GetMax();
	float GetAvg();
	float GetRms();
	float GetRectAvg();
	float GetVpp();
	float GetFormFactor();
	float GetDispersion();
	float GetPwm();

private:
	bool _GetRange( int& nBegin, int& nEnd, CSettings::Measure::ERange range );
	float _GetSamplef( BIOS::ADC::TSample& nSample );
	int _GetSample( BIOS::ADC::TSample nSample );
};

#endif