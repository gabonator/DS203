#include "CoreOscilloscope.h"
#include <Source/Core/Settings.h>

/*static*/ void CCoreOscilloscope::ConfigureAdc()
{
	const static ui16 arrPsc[] = 
	{ 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 10-1, 10-1, 10-1, 100-1, 100-1, 
	100-1, 1000-1, 1000-1, 1000-1, 1000-1, 1000-1 };
	const static ui16 arrArr[] = 
	{ 1-1, 1-1, 2-1, 3-1, 5-1, 12-1, 24-1, 48-1, 120-1, 240-1, 480-1, 120-1, 240-1, 480-1,
	120-1, 240-1, 480-1, 120-1, 240-1, 480-1, 1200-1, 1000-1};
//	const static int arrLen[] = 
//	{ 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 2048, 2048, 2048, 2048, 
//	1024, 1024, 1024, 512, 512, 512 };

	_ASSERT( COUNT(arrPsc) == COUNT(arrArr) && COUNT(arrPsc) == CSettings::TimeBase::_ResolutionMax+1 );

	ui8 nACouple = Settings.CH1.Coupling == CSettings::AnalogChannel::_DC ? 0 : 1;
	ui8 nARange = (ui8)(NATIVEENUM)Settings.CH1.Resolution;
	ui16 nAOffset = Settings.CH1.u16Position;
	ui8 nBCouple = Settings.CH2.Coupling == CSettings::AnalogChannel::_DC ? 0 : 1;
	ui8 nBRange = (ui8)(NATIVEENUM)Settings.CH2.Resolution;
	ui16 nBPosition = Settings.CH2.u16Position;
	ui16 nTimePsc = arrPsc[ (NATIVEENUM)Settings.Time.Resolution ];
	ui16 nTimeArr = arrArr[ (NATIVEENUM)Settings.Time.Resolution ];
	
	BIOS::ADC::Enable(true);
	BIOS::ADC::Configure( nACouple, nARange, nAOffset, nBCouple, nBRange, nBPosition, nTimePsc, nTimeArr );
	//BIOS::ADC::ConfigureBuffer( arrLen[ (NATIVEENUM)Settings.Time.Resolution ] );
	BIOS::ADC::Restart();

	UpdateConstants();
}

/*static*/ void CCoreOscilloscope::ConfigureTrigger()
{
	if ( Settings.Trig.Sync == CSettings::Trigger::_None )
	{
		BIOS::ADC::ConfigureTrigger(0, 0, 0, -1 );
	} else {
		BIOS::ADC::ConfigureTrigger(Settings.Trig.nTime, Settings.Trig.nLevel, 
			(ui8)(NATIVEENUM)Settings.Trig.Source, 
			1-(ui8)(NATIVEENUM)Settings.Trig.Type );
	}
}

/*static*/ void CCoreOscilloscope::UpdateConstants()
{
	Settings.Runtime.m_fTimeRes = CSettings::TimeBase::pfValueResolution[ Settings.Time.Resolution ];
	Settings.Runtime.m_fCH1Res  = 
		CSettings::AnalogChannel::pfValueResolution[ Settings.CH1.Resolution ] *
		CSettings::AnalogChannel::pfValueProbe[ Settings.CH1.Probe ];
	Settings.Runtime.m_fCH2Res  = 
		CSettings::AnalogChannel::pfValueResolution[ Settings.CH2.Resolution ] *
		CSettings::AnalogChannel::pfValueProbe[ Settings.CH2.Probe ];
}