#ifndef __CHANNELMATH_H__
#define __CHANNELMATH_H__

#include <Source/Core/Settings.h>

class CMathChannel
{
	CSettings::Calibrator::FastCalc* m_pFC1;
	CSettings::Calibrator::FastCalc* m_pFC2;
	int m_nTemp;
	//void (CMathChannel::*)m_pEvalFunc();
	//int m_nSample;
public:
	//CMathChannel();
	void			MathSetup( CSettings::Calibrator::FastCalc* pFC1, CSettings::Calibrator::FastCalc* pFC2 );
	int				MathCalc(ui32 nSample);

private:
	int				MathGet(CSettings::MathOperand& op, int nSample);
	int				MathFx( int nParam, int nScale );
	/*
	int				_GetA();
	int				_GetB();
	int				_GetC();
	int				_GetApBpC();
	int				_GetApBpC();
	*/
};

#endif