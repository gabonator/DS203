#include "ChannelMath.h"
#include <Source/Gui/Oscilloscope/Controls/GraphOsc.h>
#include <math.h>

void CMathChannel::MathSetup(CSettings::Calibrator::FastCalc* pFC1, CSettings::Calibrator::FastCalc* pFC2)
{
	m_pFC1 = pFC1;
	m_pFC2 = pFC2;
	m_nTemp = 0;
}

int CMathChannel::MathCalc(ui32 nSample)
{
	switch ( Settings.Math.Type )
	{
	case CSettings::MathOperator::_A:
		return MathGet(Settings.MathA, nSample);
	case CSettings::MathOperator::_B:
		return MathGet(Settings.MathB, nSample);
	case CSettings::MathOperator::_C:
		return MathGet(Settings.MathC, nSample);
	case CSettings::MathOperator::_AplusBplusC:
		return 
			MathGet(Settings.MathA, nSample) +
			MathGet(Settings.MathB, nSample) +
			MathGet(Settings.MathC, nSample);
	case CSettings::MathOperator::_AminusBplusC:
		return 
			MathGet(Settings.MathA, nSample) -
			MathGet(Settings.MathB, nSample) +
			MathGet(Settings.MathC, nSample);
	case CSettings::MathOperator::_BminusAplusC:
		return 
			MathGet(Settings.MathB, nSample) -
			MathGet(Settings.MathA, nSample) +
			MathGet(Settings.MathC, nSample);
	case CSettings::MathOperator::_AgreaterBplusC:
		return (MathGet(Settings.MathA, nSample) > 
			MathGet(Settings.MathB, nSample) ? 32 : 0) +
			MathGet(Settings.MathC, nSample);
	case CSettings::MathOperator::_AlessBplusC:
		return (MathGet(Settings.MathA, nSample) <
			MathGet(Settings.MathB, nSample) ? 32 : 0) +
			MathGet(Settings.MathC, nSample);
	case CSettings::MathOperator::_minAB:
		{
			int fA = MathGet(Settings.MathA, nSample);
			int fB = MathGet(Settings.MathB, nSample);
			return min(fA, fB);
		}
	case CSettings::MathOperator::_maxAB:
		{
			int fA = MathGet(Settings.MathA, nSample);
			int fB = MathGet(Settings.MathB, nSample);
			return max(fA, fB);
		}
	default:
		_ASSERT(0);
		return 0;
	}
}

int CMathChannel::MathGet(CSettings::MathOperand& op, int nSample)
{
	int nAux;
	switch ( op.Type )
	{
	case CSettings::MathOperand::_CH1Corrected:
		nAux = ( nSample & 0xff );
		nAux = Settings.CH1Calib.Correct( *m_pFC1, nAux );
		if ( op.nScale != 100 )
			nAux = nAux * op.nScale / 100;
		return nAux;	
	case CSettings::MathOperand::_Ch1Raw:
		nAux = ( nSample & 0xff );
		if ( op.nScale != 100 )
			nAux = nAux * op.nScale / 100;
		return nAux;	
	case CSettings::MathOperand::_Ch2Corrected:
		nAux = ( (nSample >> 8) & 0xff );
		nAux = Settings.CH1Calib.Correct( *m_pFC2, nAux );
		if ( op.nScale != 100 )
			nAux = nAux * op.nScale / 100;
		return nAux;	
	case CSettings::MathOperand::_Ch2Raw:
		nAux = ( (nSample >> 8) & 0xff );
		if ( op.nScale != 100 )
			nAux = nAux * op.nScale / 100;
		return nAux;
	case CSettings::MathOperand::_Constant:
		return op.nConstant;
	case CSettings::MathOperand::_Fx:
		return MathFx(op.nConstant, op.nScale);
	}
	return 0;
}

int	CMathChannel::MathFx( int nParam, int nScale )
{
	int nTotal = CWndGraph::BlkX*CWndGraph::DivsX;
	int nValue = (int)(128+127*nScale/100.0f*sin( 2*3.141592653f * nParam * (m_nTemp / (float)nTotal)));
	m_nTemp++;
	return nValue;
}

#if 0
void CMathChannel::MathSetup(CSettings::Calibrator::FastCalc* pFC1, CSettings::Calibrator::FastCalc* pFC2)
{
	m_pFC1 = pFC1;
	m_pFC2 = pFC2;

	switch ( Settings.Math.Type )
	{
	case CSettings::MathOperator::_A: 
		m_pEval = GetEvalFunc( Settings.MathA ); 
	
		break;
	case CSettings::MathOperator::_B: 
		m_pEval = GetEvalFunc( Settings.MathB ); 
		break;
	case CSettings::MathOperator::_C: 
		m_pEval = GetEvalFunc( Settings.MathC ); 
		break;
	case CSettings::MathOperator::_AplusBplusC: 
		m_pEvalX = GetEvalFunc( Settings.MathA );
		m_pEvalY = GetEvalFunc( Settings.MathB );
		m_pEvalZ = GetEvalFunc( Settings.MathC );
		m_pEval = _EvalXYZ; 
		break;
	case CSettings::MathOperator::_AminusBplusC: 
		m_pEvalX = GetEvalFunc( Settings.MathA );
		m_pEvalY = GetEvalFunc( Settings.MathB, true );
		m_pEvalZ = GetEvalFunc( Settings.MathC );
		m_pEval = _EvalXYZ; 
		break;
	case CSettings::MathOperator::_BminusAplusC:
		m_pEvalX = GetEvalFunc( Settings.MathA, true );
		m_pEvalY = GetEvalFunc( Settings.MathB );
		m_pEvalZ = GetEvalFunc( Settings.MathC );
		m_pEval = _EvalXYZ; 
		break;
	default:
		_ASSERT(0);
		return 0;
	}

}

int CMathChannel::MathCalc(ui32 nSample)
{
	m_nSample = nSample;
	return this->*m_pEval();
}

int CMathChannel::_EvalXYZ()
{
	return this->*m_pEvalX() + this->*m_pEvalY() + this->*m_pEvalZ();
}

int CMathChannel::GetEvalFunc(CSettings::MathOperand& op, bool bNegate)
{
	switch ( op.Type )
	{
	case CSettings::MathOperand::_CH1Corrected:
		return bNegate ? _EvalCh1Corrected : _EvalCh1CorrectedNeg;
	case CSettings::MathOperand::_Ch1Raw:
		return bNegate ? _EvalCh1Raw : _EvalCh1RawNeg;
	case CSettings::MathOperand::_CH2Corrected:
		return bNegate ? _EvalCh2Corrected : _EvalCh2CorrectedNeg;
	case CSettings::MathOperand::_Ch2Raw:
		return bNegate ? _EvalCh2Raw : _EvalCh2RawNeg;
	case CSettings::MathOperand::_Constant:
		return op.nConstant;
	}

int CMathChannel::_EvalA()
{
}


	switch ( Settings.Math.Type )
	{
	case CSettings::MathOperator::_A:
		return MathGet(Settings.MathA, nSample);
	case CSettings::MathOperator::_B:
		return MathGet(Settings.MathB, nSample);
	case CSettings::MathOperator::_C:
		return MathGet(Settings.MathC, nSample);
	case CSettings::MathOperator::_AplusBplusC:
		return 
			MathGet(Settings.MathA, nSample) +
			MathGet(Settings.MathB, nSample) +
			MathGet(Settings.MathC, nSample);
	case CSettings::MathOperator::_AminusBplusC:
		return 
			MathGet(Settings.MathA, nSample) -
			MathGet(Settings.MathB, nSample) +
			MathGet(Settings.MathC, nSample);
	case CSettings::MathOperator::_BminusAplusC:
		return 
			MathGet(Settings.MathB, nSample) -
			MathGet(Settings.MathA, nSample) +
			MathGet(Settings.MathC, nSample);
	default:
		_ASSERT(0);
		return 0;
	}
}

#endif

