#define ENABLE_PIDREGULATOR

#if defined(ADD_MODULE) && defined(ENABLE_PIDREGULATOR)
	ADD_MODULE( "PID Regulator", CWndPidRegulator)
#endif

#ifndef __PIDREGULATOR_H__
#define __PIDREGULATOR_H__

#include <Source/Framework/Wnd.h>
#include <Source/Core/Utils.h>

class CPid
{
public:
	// user defined variables
	float m_Kp;
	float m_Kd;
	float m_Ki;
	float m_dt;

	// levels
	float m_Target;		// set point
	float m_Current;	// CH1 ADC value
	float m_Output;		// DAC output value

	//
	const float m_fEpsilon;
	const float m_fMinimum;
	const float m_fMaximum;
	const float m_fInvalid;
	float m_preError;
	float m_integral;
	float m_derivative;
	float m_error;

	CPid() : 
		m_fEpsilon( 0.0005f ),
		m_fMinimum( 0.1f ),
		m_fMaximum( 2.4f ),
		m_fInvalid( -123.45f )
	{
		m_dt = 0.01f;
		m_Kp = 0.2f;
		m_Kd = 0.05f;
		m_Ki = 30.0f;
		m_Target = 1.0f;

		m_Current = 0.0f;
		m_Output = 0.0f;

		Reset();
	}

	void Reset()
	{
		m_preError = m_fInvalid;
		m_integral = 0;
		m_derivative = 0;
		m_error = 0;
	}

	void operator()()
	{
		//Caculate P,I,D
		m_error = m_Target - m_Current;

		//In case of error too small then stop intergration
		if (abs(m_error) > m_fEpsilon)
			m_integral += m_error*m_dt;

		if ( m_preError != m_fInvalid )
			m_derivative = (m_error - m_preError)/m_dt;

		m_Output = m_Kp*m_error + m_Ki*m_integral + m_Kd*m_derivative;

		//Saturation Filter
		UTILS.Clamp<float>( m_Output, m_fMinimum, m_fMaximum );
		m_preError = m_error;
	}
	
	float Process(float fCurrent)
	{
		m_Current = fCurrent;
		(*this)();
		return m_Output;
	}
};

class CWndPidRegulator : public CWnd
{
	static const unsigned char bitmapRegulator[];
	CPid m_Pid;

public:
	virtual void Create(CWnd *pParent, ui16 dwFlags);
	virtual void OnPaint();
	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);
	virtual void OnKey(ui16 nKey);
	virtual void OnTimer();

private:
	void ShowLocalMenu(bool bFocus);
	void ShowDiagram(CPoint& cpBase);
	void ShowDiagramValues(CPoint cpBase);
	void OnTimerGraph();
	void OnTimerDiagram();
	void ShowValues(bool bValues);
	void Process();
	void ShowGraph();

private:
	void _Highlight( CRect rcRect, int nR, int nG, int nB );
	void _Highlight( CPoint cpPoint, int nRadius, int nR, int nG, int nB );
};

#endif