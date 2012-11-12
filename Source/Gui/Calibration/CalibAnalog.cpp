#include "CalibAnalog.h"
#include <Source/Gui/MainWnd.h>
#include <Source/Core/Settings.h>

#define V_LEVEL_LO 20
#define V_LEVEL_HI 180
#define V_LEVEL 20

void CWndListCalSimple::LoadCalib()
{
	Settings.Trig.Sync = CSettings::Trigger::_None;

	if ( m_calChannel == CSettings::Trigger::_CH1 )
	{
		m_calCurve = Settings.CH1Calib.CalData[m_calRange];
		Settings.CH1.Resolution = (CSettings::AnalogChannel::eResolution)m_calRange;
		Settings.CH1.u16Position = V_LEVEL;
		Settings.CH1.Coupling = CSettings::AnalogChannel::_DC;
	}
	if ( m_calChannel == CSettings::Trigger::_CH2 )
	{
		m_calCurve = Settings.CH2Calib.CalData[m_calRange];
		Settings.CH2.Resolution = (CSettings::AnalogChannel::eResolution)m_calRange;
		Settings.CH2.u16Position = V_LEVEL;
		Settings.CH2.Coupling = CSettings::AnalogChannel::_DC;
	}
	Settings.Time.Resolution = CSettings::TimeBase::_1ms;
	MainWnd.m_wndMenuInput.ConfigureAdc();
	MainWnd.m_wndMenuInput.ConfigureTrigger();

	m_calCurve.m_arrCurveKin[0] = 0;
	m_calCurve.m_arrCurveKin[1] = 300;
	m_calCurve.m_arrCurveKin[2] = 300;
	m_calCurve.m_arrCurveKin[3] = 300;
	m_calCurve.m_arrCurveKin[4] = 300;
	m_calCurve.m_arrCurveKin[5] = 300;
	CSettings::Calibrator::Prepare(m_calRange, V_LEVEL, m_calCurve, m_calFast);
	m_nValue = m_calCurve.m_arrCurveKout[0];
	//m_nQ0 = (int)((float)m_calCurve.m_arrCurveQout[0] * 2048 / m_nValue); 
	//m_nQ1 = (int)((float)m_calCurve.m_arrCurveQout[1] * 2048 / m_nValue); 
	m_nQ0 = m_calCurve.m_arrCurveQout[0]*2048/m_nValue;
	m_nQ1 = m_calCurve.m_arrCurveQout[1]*2048/m_nValue;
/*
		void Prepare(int nRes, int nVert, LinCalibCurve& pCurCurve, FastCalc& fast)
		{
			fast.K = InterpolatorK::Get( pCurCurve.m_arrCurveKin, pCurCurve.m_arrCurveKout, nVert );
			fast.Q = InterpolatorQ::Get( pCurCurve.m_arrCurveQin, pCurCurve.m_arrCurveQout, nVert );
			fast.Zero = fast.Q / fast.K;
			fast.fMultiplier = arrMultipliers[nRes]; // V/div
		}
		float Voltage(FastCalc& fast, float fAdc)
		{	
			Y = q + X*k

			Y = (X+a)*b = a*b + X*b    
			return (fast.Q + fAdc * fast.K) * fast.fMultiplier / 65536.0f;
		}
		*/
}

void CWndListCalSimple::SaveCalib()
{
	if ( m_calChannel == CSettings::Trigger::_CH1 )
		Settings.CH1Calib.CalData[m_calRange] = m_calCurve;
	if ( m_calChannel == CSettings::Trigger::_CH2 )
		Settings.CH2Calib.CalData[m_calRange] = m_calCurve;
}

void CWndListCalSimple::_UpdateCalib( int nK, int nVert )
{
	m_calCurve.m_arrCurveKout[0] = nK;
	m_calCurve.m_arrCurveKout[1] = nK;
	m_calCurve.m_arrCurveKout[2] = nK;
	m_calCurve.m_arrCurveKout[3] = nK;
	m_calCurve.m_arrCurveKout[4] = nK;
	m_calCurve.m_arrCurveKout[5] = nK;

	//_ASSERT( (float) m_nQ0 * nK < (float)MAXINT );
	m_calCurve.m_arrCurveQout[0] = m_nQ0 * nK / 2048; //m_nQ0; //(int)((float)m_nQ0 * nK / 2048);
	m_calCurve.m_arrCurveQout[1] = m_nQ1 * nK / 2048; //m_nQ1; //(int)((float)m_nQ1 * nK / 2048);
	CSettings::Calibrator::Prepare(m_calRange, nVert, m_calCurve, m_calFast);
}

void CWndListCalSimple::UpdateZero()
{
	BIOS::DBG::sprintf(m_strQ0, " Vavg(%d)    ", 
		m_calCurve.m_arrCurveQin[0]);
	BIOS::DBG::sprintf(m_strQ0+10, " = %1f  ", 
		m_nQ0 / -2048.0f );

	m_itmStaticB.m_pszId = m_strQ0;
	m_itmStaticB.Invalidate();

	BIOS::DBG::sprintf(m_strQ1, " Vavg(%d)    ", 
		m_calCurve.m_arrCurveQin[1]);
	BIOS::DBG::sprintf(m_strQ1+10, " = %1f  ", 
		m_nQ1 / -2048.0f );

	m_itmStaticC.m_pszId = m_strQ1;
	m_itmStaticC.Invalidate();
	m_itmNumber.Invalidate();
}

void CWndListCalSimple::OnWave()
{
	int nMin = -1, nMax = -1, nSum = 0;
	for (int i=256; i<512; i++)
	{
		int nValue = 0;
		if ( m_calChannel == CSettings::Trigger::_CH1 )
			nValue = BIOS::ADC::GetAt(i) & 0xff;
		if ( m_calChannel == CSettings::Trigger::_CH2 )
			nValue = (BIOS::ADC::GetAt(i) >> 8) & 0xff;

		if ( nMax == -1 )
			nMax = nMin = nValue;
		else
		{
			nMax = max(nMax, nValue);
			nMin = min(nMin, nValue);
		}
		nSum += nValue;
	}
	float fAvg = nSum / 256.0f;
	float fValue = Settings.CH1Calib.Voltage( m_calFast, fAvg );

	if ( m_nResetPhase == -1 )
	{
		if ( nSum == 256*255 )
			BIOS::DBG::sprintf(m_strV, " Vin       = High   " );
		else if ( nSum == 0)
			BIOS::DBG::sprintf(m_strV, " Vin       = Low    " );
		else
			BIOS::DBG::sprintf(m_strV, " Vin       = %s ", CUtils::FormatVoltage( fValue ) );
		m_itmStaticH.m_pszId = m_strV;
		m_itmStaticH.Invalidate();
	}  

	if ( m_nResetPhase != -1 )
	{
		BIOS::DBG::sprintf(m_strV, " Vraw      = %1f", fAvg );
		m_itmStaticH.m_pszId = m_strV;
		m_itmStaticH.Invalidate();

		m_nResetPhase++;
		_ASSERT( m_nResetPhase < 32 );
		if ( m_nResetPhase == 1 )
		{
			Settings.CH1.u16Position = V_LEVEL_LO;
			Settings.CH2.u16Position = V_LEVEL_LO;
			MainWnd.m_wndMenuInput.ConfigureAdc();
		}
		if ( m_nResetPhase == 11 )
		{
			m_calCurve.m_arrCurveQin[0] = V_LEVEL_LO;
			m_nQ0 = (int)(fAvg*-2048.0f);
//			m_calCurve.m_arrCurveQout[0] = (int)(fAvg*-2048.0f);

			UpdateZero();
		}
		if ( m_nResetPhase == 21 )
		{
			Settings.CH1.u16Position = V_LEVEL_HI;
			Settings.CH2.u16Position = V_LEVEL_HI;
			MainWnd.m_wndMenuInput.ConfigureAdc();
		}
		if ( m_nResetPhase == 31 )
		{
			m_calCurve.m_arrCurveQin[1] = V_LEVEL_HI;
			m_nQ1 = (int)(fAvg*-2048.0f);
//			m_calCurve.m_arrCurveQout[1] = (int)(fAvg*-2048.0f);

			m_nResetPhase = -1;
			Settings.CH1.u16Position = V_LEVEL_LO;
			Settings.CH2.u16Position = V_LEVEL_LO;

			MainWnd.m_wndMenuInput.ConfigureAdc();

			_UpdateCalib( m_nValue, V_LEVEL_LO );
			UpdateZero();

			m_proReset.m_pName = "Ok.";
			m_itmReset.Invalidate();
		}
	}


}

/*virtual*/ void CWndListCalSimple::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{	
	if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
	{
		OnWave();
		return;
	}

	if ( pSender == &m_itmReset && code == ToWord('l', 'e') )
	{
		MainWnd.m_wndConfirm.Show( this, "Message", "Connect probe to ground\nReady?", RGB565(ffff00), "Yes", "No");
		return;
	}
	if ( pSender == &MainWnd.m_wndConfirm && code == ToWord('e', 'd') && data == (ui32)"Yes" )
	{
		m_proReset.m_pName = "Wait...";
		m_itmReset.Invalidate();

		m_nResetPhase = 0;
		MainWnd.m_wndConfirm.Hide();
		return;
	}
	if ( pSender == &MainWnd.m_wndConfirm && code == ToWord('e', 'd') && data == (ui32)"No" )
	{
		MainWnd.m_wndConfirm.Hide();
		return;
	}

	if ( pSender == &m_itmSave && code == ToWord('l', 'e') )
	{
		SaveCalib();
		StopModal();
		return;
	}

	if ( pSender == &m_itmNumber && code == ToWord('u', 'p') )
	{
		int nNewVal = m_nValue;
		UTILS.Clamp<int>(m_nValue, 256, 4096);
		if ( nNewVal != m_nValue )
			m_itmNumber.Invalidate();

		_UpdateCalib( m_nValue, V_LEVEL );
		UpdateZero();  // TODO: neni treba!
	}

	if ( ( pSender == &m_itmSource || pSender == &m_itmResolution ) && code == ToWord('u', 'p') )
	{
		LoadCalib();
		UpdateZero();
		return;
	}

	CListBox::OnMessage( pSender, code, data );
}

void CWndListCalAnalog::Finished()
{
	// calibration finished
	CSettings::LinCalibCurve& calCurve = Settings.CH1Calib.CalData[m_calRange];
	if ( m_calChannel == CSettings::Trigger::_CH2 )
		calCurve = Settings.CH2Calib.CalData[m_calRange];

	calCurve = m_calCurve;
	MainWnd.Invalidate();
	// parent doesn't exist anymore, window is destroyed!
	MainWnd.m_wndMessage.Show( &MainWnd, "Information", "Calibration finished!", RGB565(ffff00));
}

void CWndListCalAnalog::Aborted()
{
	MainWnd.Invalidate();
	MainWnd.m_wndMessage.Show( &MainWnd, "Information", "Error or calibration aborted", RGB565(ffff00));
}
