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

	Runtime.m_nMenuItem = -1;

	ResetCalibration();
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
	const static si32 defaultQout[] = {0, 0};
	const static si16 defaultKin[] = {0, 0, 0, 0, 0, 256};
	const static si32 defaultKout[] = {2048, 2048, 2048, 2048, 2048, 2048};

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

void CSettings::LoadCalibration()
{
	FILEINFO f;

	if ( !BIOS::DSK::Open(&f, "CALIB   DAT", BIOS::DSK::IoRead) )
	{
		return;
	}

	ui8* pSharedBuffer = (ui8*)BIOS::DSK::GetSharedBuffer();

	_ASSERT_VALID( BIOS::DSK::Read(&f, pSharedBuffer) );

	CStream bufStream( pSharedBuffer, FILEINFO::SectorSize );

	ui32 dwId = 0;
	bufStream >> dwId;
	if ( dwId != ToDword('C', 'A', 'L', '1') )
	{
		_ASSERT(0);
		return;
	}

	bufStream >> CStream(&DacCalib, sizeof(DacCalib));

	bufStream.Reset();
	_ASSERT_VALID( BIOS::DSK::Read(&f, pSharedBuffer) );
	bufStream >> CStream(&CH1Calib, sizeof(CH1Calib));

	bufStream.Reset();
	_ASSERT_VALID( BIOS::DSK::Read(&f, pSharedBuffer) );
	bufStream >> CStream(&CH2Calib, sizeof(CH2Calib));

	BIOS::DSK::Close(&f);
}
