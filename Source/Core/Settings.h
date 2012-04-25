#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#define Settings (*CSettings::m_pInstance)
#include <Source/HwLayer/Types.h>
#include "Serialize.h"

#define _VERSION ToDword('D', 'S', 'C', '2')

class CSettings : public CSerialize
{
public:
	static CSettings* m_pInstance;

public:
	class AnalogChannel : public CSerialize
	{
	public:
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

		virtual CSerialize& operator <<( CStream& stream )
		{
			stream << _E(Enabled) << _E(Coupling) << _E(Resolution) << _E(Probe) << u16Color << u16Position;
			return *this;
		}
		virtual CSerialize& operator >>( CStream& stream )
		{
			stream >> _E(Enabled) >> _E(Coupling) >> _E(Resolution) >> _E(Probe) >> u16Color >> u16Position;
			return *this;
		}
	};
	class DigitalChannel : public CSerialize
	{
	public:
		static const char* const ppszTextPolarity[];

		enum {_NO = 0, _YES = 1, _EnabledMax = _YES}
			Enabled;
		ui16 u16Color;
		ui16 u16Position;
		enum {_POS=0, _NEG, _PolarityMax = _NEG}
			Polarity;

		const char* pszName;
		const char* pszFullName;

		virtual CSerialize& operator <<( CStream& stream )
		{
			stream << _E(Enabled) << _E(Polarity) << u16Color << u16Position;
			return *this;
		}
		virtual CSerialize& operator >>( CStream& stream )
		{
			stream >> _E(Enabled) >> _E(Polarity) >> u16Color >> u16Position;
			return *this;
		}
	};
	class TimeBase : public CSerialize
	{
	public:
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


		virtual CSerialize& operator <<( CStream& stream )
		{
			stream << _E(Resolution) << _E(Range) << Shift; 
			return *this;
		}
		virtual CSerialize& operator >>( CStream& stream )
		{
			stream >> _E(Resolution) >> _E(Range) >> Shift; 
			return *this;
		}
	};

	class Trigger : public CSerialize
	{
	public:
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

		virtual CSerialize& operator <<( CStream& stream )
		{
			stream << _E(Sync) << _E(Type) << _E(Source) << _E(State) << nLevel << nTime;
			return *this;
		}
		virtual CSerialize& operator >>( CStream& stream )
		{
			stream >> _E(Sync) >> _E(Type) >> _E(Source) >> _E(State) >> nLevel >> nTime;
			return *this;
		}
	};
	class Generator : public CSerialize
	{
	public:
		static const char* const ppszTextWave[];
		enum { _Sin = 0, _Tri, _Saw, _Sq, _Cardiac, _WaveMax = _Cardiac }
			Wave;
		int nPsc;
		int nArr;

		virtual CSerialize& operator <<( CStream& stream )
		{
			stream << _E(Wave) << nPsc << nArr;
			return *this;
		}
		virtual CSerialize& operator >>( CStream& stream )
		{
			stream >> _E(Wave) >> nPsc >> nArr;
			return *this;
		}
	};
	class CRuntime : public CSerialize
	{
	public:
		int m_nMenuItem;

		virtual CSerialize& operator <<( CStream& stream )
		{
			stream << m_nMenuItem;
			return *this;
		}
		virtual CSerialize& operator >>( CStream& stream )
		{
			stream >> m_nMenuItem;
			return *this;
		}
	};

public:
	#include "Calibration.h"
	
public:
	CRuntime Runtime;
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

	virtual CSerialize& operator <<( CStream& stream )
	{
		ui32 dwId = _VERSION;
		ui32 dwEnd = ToDword('E', 'N', 'D', 27);
		stream << dwId << Runtime << CH1 << CH2 << CH3 << CH4 << Time << Trig << Gen << dwEnd;

		return *this;
	}

	virtual CSerialize& operator >>( CStream& stream )
	{
		ui32 dwId = 0, dwEnd = 0;
		
		stream >> dwId;
		_ASSERT( dwId == _VERSION );
		if ( dwId == _VERSION )
		{
			stream >> Runtime >> CH1 >> CH2 >> CH3 >> CH4 >> Time >> Trig >> Gen >> dwEnd;
			_ASSERT( dwEnd == ToDword('E', 'N', 'D', 27) );
		}
		return *this;
	}

	CSettings();
	ui32 GetChecksum();
	void Save();
	void Load();

	void ResetCalibration();
	void SaveCalibration();
	void LoadCalibration();
};
#endif
