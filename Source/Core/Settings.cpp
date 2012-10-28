#include "Settings.h"    
#include <Source/HwLayer/bios.h>
#include <string.h>

CSettings* CSettings::m_pInstance = NULL;

/*static*/ const char* const CSettings::AnalogChannel::ppszTextEnabled[] = 
		{"No", "Yes"};
/*static*/ const char* const CSettings::AnalogChannel::ppszTextCoupling[] = 
		{"AC", "DC", "GND"};
/*static*/ const char* const CSettings::AnalogChannel::ppszTextResolution[] =
		{"50mV", "100mV", "200mV", "500mV", "1V", "2V", "5V", "10V"};
/*static*/ const float CSettings::AnalogChannel::pfValueResolution[] =
		{0.050f, 0.100f, 0.200f, 0.500f, 1.0f, 2.0f, 5.0f, 10.0f};
/*static*/ const char* const CSettings::AnalogChannel::ppszTextProbe[] =
		{"1 x", "10 x", "100 x", "1000 x"};
/*static*/ const float CSettings::AnalogChannel::pfValueProbe[] =
		{1.0f, 10.0f, 100.0f, 1000.0f};
/*static*/ const char* const CSettings::DigitalChannel::ppszTextPolarity[] =
		{"Positive", "Negative"};
/*static*/ const char* const CSettings::TimeBase::ppszTextResolution[] =
		{"100ns", "200ns", "500ns", 
		"1us", "2us", "5us",
		"10us", "20us", "50us", "100us", "200us", "500us",
		"1ms", "2ms", "5ms",
		"10ms", "20ms", "50ms", "100ms", "200ms", "500ms", 
		"1s" };
/*static*/ const float CSettings::TimeBase::pfValueResolution[] =
		{100e-9f, 200e-9f, 500e-9f,
		1e-6f, 2e-6f, 5e-6f,
		10e-6f, 20e-6f, 50e-6f, 100e-6f, 200e-6f, 500e-6f,
		1e-3f, 2e-3f, 5e-3f,
		10e-3f, 20e-3f, 50e-3f, 100e-3f, 200e-3f, 500e-3f,
		1.0f};
/*static*/ const char* const CSettings::Generator::ppszTextWave[] =
		{"Sin", "Triangle", "Sawtooth", "Square", "Cardiac"};
/*static*/ const char* const CSettings::Trigger::ppszTextSync[] =
		{"Auto", "Norm", "Single", "None"};
/*static*/ const char* const CSettings::Trigger::ppszTextSource[] =
		{"CH1", "CH2", "CH3", "CH4", "Math"};
/*static*/ const char* const CSettings::Trigger::ppszTextType[] =
		{"high", "low"};

/*static*/ const char* const CSettings::Marker::ppszTextMode[] =
		{"Off", "On", "Auto"};
/*static*/ const char* const CSettings::Marker::ppszTextSource[] =
		{ "CH1", "CH2" };
/*static*/ const char* const CSettings::Marker::ppszTextDisplay[] =
		{ "Raw", "Units" };
/*static*/ const char* const CSettings::Marker::ppszTextFind[] =
		{ "Minimum", "Average", "Maximum" };

/*static*/ const char* const CSettings::Measure::ppszTextEnabled[] =
		{ "Off", "On" };
/*static*/ const char* const CSettings::Measure::ppszTextSource[] =
		{ "CH1", "CH2" };
/*static*/ const char* const CSettings::Measure::ppszTextType[] =
		{ "Minimum", "Maximum", "Average", "RMS", "RectAvg", "Vpp", "Freq", "Period", "FormFact", "Sigma", "Variance" };
/*static*/ const char* const CSettings::Measure::ppszTextRange[] =
		{ "View", "Selection", "All" };

CSettings::CSettings()
{
	m_pInstance = this;
	Reset();
	ResetCalibration();
}

void CSettings::Reset()
{
	CH1.pszName = "CH1";
	CH1.pszFullName = "Analog input: CH1";
	CH1.Coupling = AnalogChannel::_DC;
	CH1.Resolution = AnalogChannel::_200mV;
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

	MarkT1.Mode = Marker::_On;
	MarkT1.Source = Marker::_CH1;
	MarkT1.Display = Marker::_Physical;
	MarkT1.nValue = 100;
	MarkT1.u16Color = RGB565(707070);
	MarkT1.Type = Marker::_Time;
	MarkT1.strName = "X1";
	MarkT1.strFullName = "Cursor X1";

	MarkT2.Mode = Marker::_On;
	MarkT2.Source = Marker::_CH1;
	MarkT2.Display = Marker::_Physical;
	MarkT2.nValue = 200;
	MarkT2.u16Color = RGB565(d0d0d0);
	MarkT2.Type = Marker::_Time;
	MarkT2.strName = "X2";
	MarkT2.strFullName = "Cursor X2";

	MarkY1.Mode = Marker::_Auto;
	MarkY1.Source = Marker::_CH1;
	MarkY1.Display = Marker::_Physical;
	MarkY1.nValue = 200;
	MarkY1.u16Color = RGB565(707070);
	MarkY1.Type = Marker::_Voltage;
	MarkY1.strName = "Y1";
	MarkY1.strFullName = "Cursor Y1";

	MarkY2.Mode = Marker::_Auto;
	MarkY2.Source = Marker::_CH1;
	MarkY2.Display = Marker::_Physical;
	MarkY2.nValue = 200;
	MarkY2.u16Color = RGB565(d0d0d0);
	MarkY2.Type = Marker::_Voltage;
	MarkY2.strName = "Y2";
	MarkY2.strFullName = "Cursor Y2";

	Meas[0].Enabled = Measure::_On;
	Meas[0].Source = Measure::_CH1;
	Meas[0].Type = Measure::_Rms;
	Meas[0].Range = Measure::_View;
	Meas[0].fValue = 0;

	Meas[1].Enabled = Measure::_On;
	Meas[1].Source = Measure::_CH1;
	Meas[1].Type = Measure::_Vpp;
	Meas[1].Range = Measure::_View;
	Meas[1].fValue = 0;

	Meas[2].Enabled = Measure::_Off;
	Meas[2].Source = Measure::_CH1;
	Meas[2].Type = Measure::_Vpp;
	Meas[2].Range = Measure::_View;
	Meas[2].fValue = 0;

	Meas[3].Enabled = Measure::_On;
	Meas[3].Source = Measure::_CH2;
	Meas[3].Type = Measure::_Rms;
	Meas[3].Range = Measure::_View;
	Meas[3].fValue = 0;

	Meas[4].Enabled = Measure::_On;
	Meas[4].Source = Measure::_CH2;
	Meas[4].Type = Measure::_Vpp;
	Meas[4].Range = Measure::_View;
	Meas[4].fValue = 0;

	Meas[5].Enabled = Measure::_Off;
	Meas[5].Source = Measure::_CH2;
	Meas[5].Type = Measure::_Vpp;
	Meas[5].Range = Measure::_View;
	Meas[5].fValue = 0;

	Runtime.m_nMenuItem = -1;
	Runtime.m_nUptime = 0;
}

ui32 CSettings::GetChecksum()
{
	ui8* pSharedBuffer = (ui8*)BIOS::DSK::GetSharedBuffer();
	memset( pSharedBuffer, 0, FILEINFO::SectorSize );

	CStream bufStream( pSharedBuffer, FILEINFO::SectorSize );
	bufStream << *this;
	return bufStream.GetChecksum();
}

void CSettings::Save()
{
	FILEINFO f;
	if ( !BIOS::DSK::Open(&f, "CONFIG  DAT", BIOS::DSK::IoWrite) )
	{
		_ASSERT( 0 );
		return;
	}
	ui8* pSharedBuffer = (ui8*)BIOS::DSK::GetSharedBuffer();
	memset( pSharedBuffer, 0, FILEINFO::SectorSize );

	CStream bufStream( pSharedBuffer, FILEINFO::SectorSize );
	bufStream << *this;

	_ASSERT_VALID( BIOS::DSK::Write(&f, pSharedBuffer) );
	BIOS::DSK::Close(&f, bufStream.GetLength());
}

void CSettings::Load()
{
	FILEINFO f;

	if ( !BIOS::DSK::Open(&f, "CONFIG  DAT", BIOS::DSK::IoRead) )
	{
		return;
	}

	ui8* pSharedBuffer = (ui8*)BIOS::DSK::GetSharedBuffer();

	_ASSERT_VALID( BIOS::DSK::Read(&f, pSharedBuffer) );

	CStream bufStream( pSharedBuffer, FILEINFO::SectorSize );
	bufStream >> *this;

	BIOS::DSK::Close(&f);
}

void CSettings::ResetCalibration()
{
	// valid on range 128..4096
	//DacCalib.m_arrCurveIn[0] = 0.1765f;	DacCalib.m_arrCurveOut[0] = 256;
	//DacCalib.m_arrCurveIn[1] = 1.412f;	DacCalib.m_arrCurveOut[1] = 2048;
	DacCalib.m_arrCurveIn[0] = 0.5f;	DacCalib.m_arrCurveOut[0] = 725;
	DacCalib.m_arrCurveIn[1] = 1.5f;	DacCalib.m_arrCurveOut[1] = 2176;

	const static si16 defaultQin[] = {0, 256};
	const static si32 defaultQout[] = {0, -(256<<11)};
	const static si16 defaultKin[] = {0, 0, 0, 0, 0, 256};
	const static si32 defaultKout[] = {1<<11, 1<<11, 1<<11, 1<<11, 1<<11, 1<<11};

	for ( int i = 0; i < AnalogChannel::_ResolutionMax; i++ )
	{
		memcpy( CH1Calib.CalData[i].m_arrCurveQin, defaultQin, sizeof(defaultQin) );
		memcpy( CH1Calib.CalData[i].m_arrCurveQout, defaultQout, sizeof(defaultQout) );
		memcpy( CH1Calib.CalData[i].m_arrCurveKin, defaultKin, sizeof(defaultKin) );
		memcpy( CH1Calib.CalData[i].m_arrCurveKout, defaultKout, sizeof(defaultKout) );

		memcpy( CH2Calib.CalData[i].m_arrCurveQin, defaultQin, sizeof(defaultQin) );
		memcpy( CH2Calib.CalData[i].m_arrCurveQout, defaultQout, sizeof(defaultQout) );
		memcpy( CH2Calib.CalData[i].m_arrCurveKin, defaultKin, sizeof(defaultKin) );
		memcpy( CH2Calib.CalData[i].m_arrCurveKout, defaultKout, sizeof(defaultKout) );
	}

	#define CONCAT2(x, y) x ## y
	#define CONCAT(x, y) CONCAT2(x, y)
	#define _COPY(type, target, ...) \
		const static type CONCAT(tmp,__LINE__)[] = __VA_ARGS__; \
		memcpy( target, CONCAT(tmp,__LINE__), sizeof(CONCAT(tmp,__LINE__)) );

	_COPY( si16, CH1Calib.CalData[AnalogChannel::_200mV].m_arrCurveQin, {-20, 280} );
	_COPY( si32, CH1Calib.CalData[AnalogChannel::_200mV].m_arrCurveQout, {9380*5, -154816*5} );
	_COPY( si16, CH1Calib.CalData[AnalogChannel::_200mV].m_arrCurveKin, {-20, 15, 75, 90, 245, 280} );
	_COPY( si32, CH1Calib.CalData[AnalogChannel::_200mV].m_arrCurveKout, {581*5, 580*5, 581*5, 582*5, 584*5, 580*5} );
}

void CSettings::SaveCalibration()
{
	FILEINFO f;
	if ( !BIOS::DSK::Open(&f, "CALIB   DAT", BIOS::DSK::IoWrite) )
	{
		_ASSERT( 0 );
		return;
	}
	ui8* pSharedBuffer = (ui8*)BIOS::DSK::GetSharedBuffer();
	memset( pSharedBuffer, 0, FILEINFO::SectorSize );

	ui32 dwId = ToDword('C', 'A', 'L', '1');

	CStream bufStream( pSharedBuffer, FILEINFO::SectorSize );
	bufStream
			<< dwId
			<< CStream(&DacCalib, sizeof(DacCalib));

	_ASSERT_VALID( BIOS::DSK::Write(&f, pSharedBuffer) );

	memset( pSharedBuffer, 0, FILEINFO::SectorSize );
	bufStream.Reset();
	bufStream << CStream(&CH1Calib, sizeof(CH1Calib));
	_ASSERT_VALID( BIOS::DSK::Write(&f, pSharedBuffer) );

	memset( pSharedBuffer, 0, FILEINFO::SectorSize );
	bufStream.Reset();
	bufStream << CStream(&CH2Calib, sizeof(CH1Calib));

	_ASSERT_VALID( BIOS::DSK::Write(&f, pSharedBuffer) );
	BIOS::DSK::Close(&f);
}

bool CSettings::LoadCalibration()
{
	FILEINFO f;

	if ( !BIOS::DSK::Open(&f, "CALIB   DAT", BIOS::DSK::IoRead) )
	{
		return false;
	}

	ui8* pSharedBuffer = (ui8*)BIOS::DSK::GetSharedBuffer();

	_ASSERT_VALID( BIOS::DSK::Read(&f, pSharedBuffer) );

	CStream bufStream( pSharedBuffer, FILEINFO::SectorSize );

	ui32 dwId = 0;
	bufStream >> dwId;
	if ( dwId != ToDword('C', 'A', 'L', '1') )
	{
		_ASSERT(0);
		return false;
	}

	bufStream >> CStream(&DacCalib, sizeof(DacCalib));

	bufStream.Reset();
	_ASSERT_VALID( BIOS::DSK::Read(&f, pSharedBuffer) );
	bufStream >> CStream(&CH1Calib, sizeof(CH1Calib));

	bufStream.Reset();
	_ASSERT_VALID( BIOS::DSK::Read(&f, pSharedBuffer) );
	bufStream >> CStream(&CH2Calib, sizeof(CH2Calib));

	BIOS::DSK::Close(&f);
	return true;
}
