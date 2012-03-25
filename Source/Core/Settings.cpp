#include "Settings.h"

CSettings* CSettings::m_pInstance = NULL;

/*static*/ const char* const CSettings::AnalogChannel::ppszTextEnabled[] = 
		{"No", "Yes"};
/*static*/ const char* const CSettings::AnalogChannel::ppszTextCoupling[] = 
		{"AC", "DC", "GND"};
/*static*/ const char* const CSettings::AnalogChannel::ppszTextResolution[] =
		{"50mV", "100mV", "200mV", "500mV", "1V", "2V", "5V", "10V"};
/*static*/ const char* const CSettings::AnalogChannel::ppszTextProbe[] =
		{"1 x", "10 x", "100 x", "1000 x"};
/*static*/ const char* const CSettings::DigitalChannel::ppszTextPolarity[] =
		{"Positive", "Negative"};
/*static*/ const char* const CSettings::TimeBase::ppszTextResolution[] =
		{"100ns", "200ns", "500ns", 
		"1us", "2us", "5us",
		"10us", "20us", "50us", "100us", "200us", "500us",
		"1ms", "2ms", "5ms",
		"10ms", "20ms", "50ms", "100ms", "200us", "500ms", 
		"1s" };
/*static*/ const char* const CSettings::Generator::ppszTextWave[] =
		{"Sin", "Triangle", "Sawtooth", "Square", "Cardiac"};
/*static*/ const char* const CSettings::Trigger::ppszTextSync[] =
		{"Auto", "Norm", "Single", "None"};
/*static*/ const char* const CSettings::Trigger::ppszTextSource[] =
		{"CH1", "CH2", "CH3", "CH4", "Math"};
/*static*/ const char* const CSettings::Trigger::ppszTextType[] =
		{"high", "low"};

CSettings::CSettings()
{
	m_pInstance = this;

	CH1.pszName = "CH1";
	CH1.pszFullName = "Analog input: CH1";
	CH1.Coupling = AnalogChannel::_DC;
	CH1.Resolution = AnalogChannel::_500mV;
	CH1.Probe = AnalogChannel::_1X;
	CH1.u16Color = RGB565(ffff00);
	CH1.u16Position = 55;
	CH1.Enabled = AnalogChannel::_YES;

	CH2.pszName = "CH2";
	CH2.pszFullName = "Analog input: CH2";
	CH2.Coupling = AnalogChannel::_AC;
	CH2.Resolution = AnalogChannel::_50mV;
	CH2.Probe = AnalogChannel::_10X;
	CH2.u16Color = RGB565(00ffff);
	CH2.u16Position = 100;
	CH2.Enabled = AnalogChannel::_YES;

	CH3.pszName = "CH3";
	CH3.pszFullName = "Digital input: CH3";
	CH3.u16Color = RGB565(ff00ff);
	CH3.u16Position = 100;
	CH3.Polarity = DigitalChannel::_POS;
	CH3.Enabled = DigitalChannel::_NO;

	CH4.pszName = "CH4";
	CH4.pszFullName = "Digital input: CH4";
	CH4.u16Color = RGB565(00ff00);
	CH4.u16Position = 200;
	CH4.Polarity = DigitalChannel::_NEG;
	CH4.Enabled = DigitalChannel::_NO;
	
	Time.Resolution = TimeBase::_500us;
	Time.Range = TimeBase::_4;
	Time.Shift = 0;

	Trig.Sync = Trigger::_None;
	Trig.Type = Trigger::_EdgeLH;
	Trig.Source = Trigger::_CH1;
	Trig.State = Trigger::_Run;
	Trig.nLevel = 128;
	Trig.nTime = 30*4;
	Trig.nLastChange = 0;

	Gen.Wave = Generator::_Cardiac;
	//Gen.nPsc = 180-1;
	//Gen.nArr = 592; //2000-1;
	Gen.nPsc = 180-1;
	Gen.nArr = 5100;

	calCH1[AnalogChannel::_500mV] = LinearCalibration(-7, 1024*256/182);
}