#define ENABLE_EMETER

#if defined(ADD_MODULE) && defined(ENABLE_EMETER)
	ADD_MODULE( "EMeter", CWndEMeter )
#endif

#ifndef __EMETER_H__
#define __EMETER_H__

#include <Source/Core/Settings.h>
#include <Source/Core/Utils.h>
#include <Source/Framework/Wnd.h>
#include <Source/Core/Bitmap.h>
#include <Source/Gui/Spectrum/Core/FFT.h>
#include <Source/Gui/Oscilloscope/Core/CoreOscilloscope.h>
#include <Source/Gui/Oscilloscope/Controls/GraphBase.h>


class CWndEMeter : public CWnd
{
	int m_nTickStart;
	int m_nLastSlot;
	int m_nCounter;
	int nOldResolution;
	int nOldSync;
	int nOldStandby;

public:
	virtual void Create(CWnd *pParent, ui16 dwFlags)
	{
		CWnd::Create("CWndEnergyMeter", dwFlags | CWnd::WsListener | CWnd::WsNoActivate, CRect(0, 16, 400, 240), pParent);
	}

	virtual void OnPaint();

	void OnWave( int nValue );

	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data)
	{
		if (code == ToWord('L', 'D') )
		{
			Settings.Time.Resolution = (CSettings::TimeBase::EResolution)nOldResolution;
			Settings.Trig.Sync = (CSettings::Trigger::ESync)nOldSync;
			Settings.Runtime.m_nStandby = nOldStandby;
			CCoreOscilloscope::ConfigureAdc();
			return;
		}

		if (code == ToWord('L', 'E') )
		{
			nOldResolution = Settings.Time.Resolution;
			nOldSync = Settings.Trig.Sync;
//			Settings.CH1.Resolution = CSettings::AnalogChannel::_200mV;
			Settings.Time.Resolution = CSettings::TimeBase::_200us;
			Settings.Trig.Sync = CSettings::Trigger::_Scan;
			nOldStandby = Settings.Runtime.m_nStandby;
			Settings.Runtime.m_nStandby = 0; // disable standby
			CCoreOscilloscope::ConfigureAdc();
			return;
		}

		if ( pSender == NULL && code == WmBroadcast && data == ToWord('d', 'g') )
		{
			// get average to reduce spurious errors, probably useless
			int nSum = 0;
			for (int i=256; i<512; i++)
			{
				BIOS::ADC::SSample Sample;
				Sample.nValue = BIOS::ADC::GetAt(i);
				nSum += Sample.CH1;
			}
			nSum /= 256;
			
			OnWave( nSum );
			return;
		}
	}
};

#endif