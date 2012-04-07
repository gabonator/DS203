#include "Settings.h"    
//#include <Source/HwLayer/bios.h>
#include <string.h>

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

//	calCH1[AnalogChannel::_500mV] = LinearCalibration(-7, 1024*256/182);

	const ui8 _inp[] = {0, 40, 80, 120, 160, 200};
	const ui8 _out[] = {7*200/256, 40*200/256, 77*200/256, 114*200/256, 152*200/256, 190*200/256};

	memcpy(calPosCH1.inp, _inp, sizeof(_inp));
	memcpy(calPosCH1.out, _out, sizeof(_out));

/*
pos=-9 500mV 1x
0V: 0.015   
1V: 40.017
2V: 86.978
3V: 133.073
4V: 180.997 
5V: 228.008
6V: 255
*/
	const ui8 _inp1[] = {0, 40, 87, 133, 181, 228};
	const ui8 _out1[] = {0, 64, 128, 192, 255, 255};
	memcpy(calCH1[AnalogChannel::_500mV].inp, _inp1, sizeof(_inp1));
	memcpy(calCH1[AnalogChannel::_500mV].out, _out1, sizeof(_out1));
	CH1.u16Position = -9;

/*
	const float _out_dac[] = {0.0679f, 0.0795f, 0.1733f, 0.4297f, 1.5747f, 2.7179f};
	const ui16 _inp_dac[] = {0,        128, 256, 640, 2304, 3968};
	memcpy(calDAC.inp, _out_dac, sizeof(_out_dac));
	memcpy(calDAC.out, _inp_dac, sizeof(_inp_dac));
*/


	// valid on range 128..4096
	calDAC.inp[0] = 0.1765;		calDAC.out[0] = 256;
	calDAC.inp[1] = 1.412;		calDAC.out[1] = 2048;


/*
generator
GEN::Output(0)	2.859*64/40/32*0.5 = 0.071475 V
500							13.250*64/40/32*0.5	= 0.33125 V
1000						27.266 							= 0.68165
*/
}

void CSettings::Save()
{
/*
	FILEINFO f;

	if ( !BIOS::DSK::Open(&f, (si8*)"CONFIG  DAT", BIOS::DSK::IoWrite) )
	{
		_ASSERT( 0 );
		return;
	}

	ui8* pBuffer = (ui8*)this;
	int nLength = sizeof(CSettings);
	_ASSERT( nLength < 512 );
	_ASSERT_VALID( BIOS::DSK::Write(&f, pBuffer ) );

	BIOS::DSK::Close(&f, nLength);
*/
}

void CSettings::Load()
{
/*
	FILEINFO f;

	if ( !BIOS::DSK::Open(&f, (si8*)"CONFIG  DAT", BIOS::DSK::IoRead) )
	{
		return;
	}

	ui8* pBuffer = (ui8*)this;
	int nLength = sizeof(CSettings);
	_ASSERT( nLength < 512 );

	_ASSERT_VALID( BIOS::DSK::Read(&f, pBuffer ) );

	BIOS::DSK::Close(&f, nLength);
*/
}
