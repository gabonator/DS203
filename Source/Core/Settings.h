#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#define Settings (*CSettings::m_pInstance)
#include <Source/HwLayer/Types.h>

class CSettings
{
public:
	static CSettings* m_pInstance;

public:
	struct AnalogChannel
	{
		static const char* const ppszTextEnabled[];
		static const char* const ppszTextCoupling[];
		static const char* const ppszTextResolution[];
		static const char* const ppszTextProbe[];

		enum eEnabled {_NO = 0, _YES = 1, _EnabledMax = _YES}
			Enabled;
		enum eCoupling {_AC = 0, _DC, _GND, _CouplingMax = _GND} 
			Coupling;
		enum eResolution {_50mV, _100mV, _200mV, _500mV, _1V, _2V, _5V, _10V, _ResolutionMax = _10V} 
			Resolution;
		enum eProbe { _1X = 0, _10X, _100X, _1000X, _ProbeMax = _1000X }
			Probe;
		ui16 u16Color;
		si16 u16Position;
		const char* pszName;
		const char* pszFullName;
	};
	struct DigitalChannel
	{
		static const char* const ppszTextPolarity[];

		enum {_NO = 0, _YES = 1, _EnabledMax = _YES}
			Enabled;
		ui16 u16Color;
		ui16 u16Position;
		enum {_POS=0, _NEG, _PolarityMax = _NEG}
			Polarity;

		const char* pszName;
		const char* pszFullName;
	};
	struct TimeBase
	{
		static const char* const ppszTextResolution[];
		enum { 
			_100ns, _200ns, _500ns, 
			_1us, _2us, _5us,
			_10us, _20us, _50us, _100us, _200us, _500us,
			_1ms, _2ms, _5ms,
			_10ms, _20ms, _50ms, _100ms, _200ms, _500ms, 
			_1s, _ResolutionMax = _1s }
			Resolution;
		enum {
			_1, _2, _4, _FULL}
			Range;
		si16 Shift;
	};

	struct Trigger
	{
		static const char* const ppszTextSync[];
		static const char* const ppszTextSource[];
		static const char* const ppszTextType[];
		enum { _Auto, _Norm, _Single, _None, _SyncMax = _None }
			Sync;
		enum { _EdgeLH, _EdgeHL, _Pulse, _TypeMax = _EdgeHL }
			Type;
		enum { _CH1, _CH2, _CH3, _CH4, _Math, _SourceMax = _Math }
			Source;
		enum { _Run, _Stop, _Wait, _Unsync, _StateMax = _Unsync }
			State;
		si16 nLevel;
		si16 nTime;
		ui32 nLastChange;

	};
	struct Generator
	{
		static const char* const ppszTextWave[];
		enum { _Sin = 0, _Tri, _Saw, _Sq, _Cardiac, _WaveMax = _Cardiac }
			Wave;
		int nPsc;
		int nArr;
	};

public:
	#include "Calibration.h"
	
public:
	AnalogChannel CH1;
	AnalogChannel CH2;
	DigitalChannel CH3;
	DigitalChannel CH4;
	TimeBase Time;
	Trigger Trig;
	Generator Gen;

	Calibrator	CH1Calib;
	Calibrator	CH2Calib;
	LinApprox	DacCalib;

	CSettings();
	void Save();
	void Load();
	void ResetCalibration();
};
#endif