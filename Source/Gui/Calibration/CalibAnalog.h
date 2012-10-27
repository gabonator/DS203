#ifndef __CALIBANALOG_H__
#define __CALIBANALOG_H__

#include <Source/Framework/Wnd.h>
#include <Source/Main/Application.h>
#include <Source/Core/Controls.h>
#include <Source/Core/ListItems.h>
#include <Source/Core/Settings.h>
#include "CalibCore.h"

class CWndListCalAnalog : public CListBox, public CCalibration
{
public:
	NATIVEENUM m_calChannel;
	NATIVEENUM m_calRange;
	CSettings::LinCalibCurve	m_calCurve;

	CLStaticItem	m_itmStaticA;
	CLStaticItem	m_itmStaticB;

	CProviderEnum	m_proSource;
	CLPItem			m_itmSource;

	CProviderEnum	m_proResolution;
	CLPItem			m_itmResolution;
 
	CProviderBtn	m_proInfo;
	CLPItem			m_itmInfo;

	CProviderBtn	m_proExecute;
	CLPItem			m_itmExecute;

public:
	void Create( CWnd* pParent )
	{
		m_calChannel = CSettings::Trigger::_CH1;
		m_calRange = CSettings::AnalogChannel::_200mV;

		CListBox::Create( "Analog Calibration", WsVisible | WsModal | CWnd::WsListener, CRect(20, 30, 280, 201), RGB565(00ff40), pParent );

		m_itmStaticA.Create( "Select source channel and range", CWnd::WsVisible | CWnd::WsNoActivate, this);
		m_itmStaticB.Create( "", CWnd::WsVisible | CWnd::WsNoActivate, this);

		m_proResolution.Create( (const char**)CSettings::AnalogChannel::ppszTextResolution,
			&m_calRange, CSettings::AnalogChannel::_ResolutionMax );
		m_itmResolution.Create( "Resolution", CWnd::WsVisible, &m_proResolution, this );
		
		m_proSource.Create( (const char**)CSettings::Trigger::ppszTextSource,
			&m_calChannel, CSettings::Trigger::_CH2 );
		m_itmSource.Create( "Source", CWnd::WsVisible, &m_proSource, this );

		m_proExecute.Create( "Start!" );
		m_itmExecute.Create( "Calibration", CWnd::WsVisible, &m_proExecute, this );

		m_proInfo.Create( "Ok!" );
		m_itmInfo.Create( "Get info", CWnd::WsVisible, &m_proInfo, this );
	}

	/*virtual*/ void OnKey(ui16 nKey)
	{
		if ( !IsRunning() )
		{
			CWnd::OnKey( nKey );
		} else
		{
			Stop();
		}
	}

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{	
		if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
		{
			OnWave();
			return;
		}

		if ( pSender == &m_itmExecute && code == ToWord('l', 'e') )
		{
			CCalibration::Setup( m_calChannel, m_calRange, &m_calCurve );
			SetFocus();	// become key listener
			CCalibration::Start();
			// following code is executed when the calibration finishes/stops
			StopModal(); // this recovers focus
			if ( CCalibration::GetResult() != CCalibration::ECalibFinished )
				Aborted();
			else
				Finished();
			return;
		}
		if ( pSender == &m_itmInfo && code == ToWord('l', 'e') )
		{
			const FLOAT fInv = 1.0f/2048.0f;
			StopModal();
			CSettings::LinCalibCurve& calCurve = Settings.CH1Calib.CalData[m_calRange];
			if ( m_calChannel == CSettings::Trigger::_CH2 )
				calCurve = Settings.CH2Calib.CalData[m_calRange];
			int y = 20;
			BIOS::LCD::Printf( 30, y += 16, RGB565(ffffff), RGB565(000000), "Calibration data %s: %s",
				CSettings::Trigger::ppszTextSource[ m_calChannel ],
				CSettings::AnalogChannel::ppszTextResolution[ m_calRange ]);
			y += 8;
			for (int i = 0; i < CSettings::LinCalibCurve::eQPoints; i++ )
				BIOS::LCD::Printf( 30, y += 16, RGB565(ffffff), RGB565(000000), "Q%d: %4d -> %d (%f)", 
					i, calCurve.m_arrCurveQin[i], calCurve.m_arrCurveQout[i], -calCurve.m_arrCurveQout[i] * fInv );
			y += 8;
			for (int i = 0; i < CSettings::LinCalibCurve::eKPoints; i++ )
				BIOS::LCD::Printf( 30, y += 16, RGB565(ffffff), RGB565(000000), "K%d: %4d -> %d (%f)", 
					i, calCurve.m_arrCurveKin[i], calCurve.m_arrCurveKout[i], calCurve.m_arrCurveKout[i] * fInv );
		}
	}

	void Finished();
	void Aborted();

	void OnWave()
	{
		if ( CCalibration::nDownload > 0 )
			if ( --CCalibration::nDownload == 0 )			
			{
#ifdef _WIN32
				float fAvg = CCalibration::nVertPos*256/200 + CCalibration::fVoltage*32/0.2f;
				if ( fAvg < 0 ) 
					fAvg = 0;
				if ( fAvg > 255.0f )
					fAvg = 255.0f;

				CCalibration::OnWave( fAvg, 0.5f );
				return;
#endif
				int nSum = 0;
				int nMax = -1, nMin = -1;
				for (int i=0; i</*BIOS::ADC::Length()*/ 4096; i++)
				{
					int nValue = 0;
					if ( CCalibration::nChannel == 1 )
						nValue = BIOS::ADC::GetAt(i) & 0xff;
					if ( CCalibration::nChannel == 2 )
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

				float fAverage = (float)nSum/4096.0f;
				float fVariance = (nMax-nMin)/4.0f;	// pseudo variance :)
				CCalibration::OnWave( fAverage, fVariance );
			}
	}

};

#endif