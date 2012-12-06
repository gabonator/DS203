#include "CoreOscilloscope.h"
#include <Source/Core/Settings.h>

/*static*/ void CCoreOscilloscope::ConfigureAdc()
{
	/*
	const static ui16 arrPsc[] = 
	{ 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 10-1, 10-1, 10-1, 100-1, 100-1, 
	100-1, 1000-1, 1000-1, 1000-1, 1000-1, 1000-1 };
	const static ui16 arrArr[] = 
	{ 1-1, 1-1, 2-1, 3-1, 5-1, 12-1, 24-1, 48-1, 120-1, 240-1, 480-1, 120-1, 240-1, 480-1,
	120-1, 240-1, 480-1, 120-1, 240-1, 480-1, 1200-1, 1000-1};
	const static int arrLen[] = 
	{ 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 2048, 2048, 2048, 2048, 
	1024, 1024, 1024, 512, 512, 512 };
	*/
//	_ASSERT( COUNT(arrPsc) == COUNT(arrArr) && COUNT(arrPsc) == CSettings::TimeBase::_ResolutionMax+1 );

	ui8 nACouple = Settings.CH1.Coupling == CSettings::AnalogChannel::_DC ? 0 : 1;
	ui8 nARange = (ui8)(NATIVEENUM)Settings.CH1.Resolution;
	ui16 nAOffset = Settings.CH1.u16Position;
	ui8 nBCouple = Settings.CH2.Coupling == CSettings::AnalogChannel::_DC ? 0 : 1;
	ui8 nBRange = (ui8)(NATIVEENUM)Settings.CH2.Resolution;
	ui16 nBPosition = Settings.CH2.u16Position;

	float fTimePerDiv = CSettings::TimeBase::pfValueResolution[ (NATIVEENUM)Settings.Time.Resolution ];
	//ui16 nTimePsc = arrPsc[ (NATIVEENUM)Settings.Time.Resolution ];
	//ui16 nTimeArr = arrArr[ (NATIVEENUM)Settings.Time.Resolution ];
	
	BIOS::ADC::Enable(true);
	BIOS::ADC::Configure( nACouple, nARange, nAOffset, nBCouple, nBRange, nBPosition, fTimePerDiv );
//	BIOS::ADC::ConfigureBuffer( arrLen[ (NATIVEENUM)Settings.Time.Resolution ] );
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

/*
nova:
const X_attr X_ATTR[27] ={
// STR PSC ARR CCR KP SCALE
.  {"! 1S !", 1000-1, 2400-1, 1200, 1024, 33333333},
.  {"500mS", 1000-1, 1200-1, 600, 1024, 16666667},
.  {"200mS", 1000-1, 480-1, 240, 1024, 6666667},
.  {"100mS", 1000-1, 240-1, 120, 1024, 3333333},
.  {"!50mS!", 1000-1, 120-1, 60, 1024, 1666667},
  {"!20mS!", 100-1, 480-1, 240, 1024, 666667},
  {"!10mS!", 100-1, 240-1, 120, 1024, 333333},
  {" 5mS ", 100-1, 120-1, 60, 1024, 166667},
  {" 2mS ", 10-1, 480-1, 240, 1024, 66667},
  {" 1mS ", 10-1, 240-1, 120, 1024, 33333},
  {"500uS", 10-1, 120-1, 60, 1024, 16667},
  {"200uS", 1-1, 480-1, 240, 1024, 6667},
  {"100uS", 1-1, 240-1, 120, 1024, 3333},
  {"!50uS!", 1-1, 120-1, 60, 1024, 1667},
  {"!20uS!", 1-1, 48-1, 24, 1024, 667},
  {"!10uS!", 1-1, 24-1, 12, 1024, 333},
  {" 5uS ", 1-1, 12-1, 6, 1024, 167},
  {" 2uS ", 1-1, 5-1, 3, 983, 67}, //1024*4.8/5 =983
  {" 1uS ", 1-1, 3-1, 2, 819, 33}, //1024*2.4/3 =819
  {"0.5uS", 1-1, 2-1, 1, 614, 17}, //1024*1.2/2 =614
  {"0.2uS", 1-1, 1-1, 1, 492, 7}, //1024*0.48 =492
  {"0.1uS", 1-1, 1-1, 1, 246, 3}, //1024*0.24 =246
  {" 2uS+", 1-1, 10-1, 3, 983, 67}, //1024*9.6/10=983
  {" 1uS+", 1-1, 5-1, 2, 983, 33}, //1024*4.8/5 =983
  {".5uS+", 1-1, 3-1, 1, 819, 17}, //1024*2.4/3 =819
  {".2uS+", 1-1, 2-1, 1, 983, 7}, //1024*0.96 =983
  {".1uS+", 1-1, 1-1, 1, 491, 3}};//1024*0.48 =492


*/