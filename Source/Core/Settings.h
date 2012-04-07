#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#define Settings (*CSettings::m_pInstance)
//#include <Source/HwLayer/bios.h>
#include <Source/HwLayer/Types.h>

class CSettings
{
public:
	static CSettings* m_pInstance;

public:
	class LinearCalibration
	{
	public:
		si16 nOffset;
		ui16 nScale;

		si16 Apply( si16 nInput )
		{
			si32 nTemp = nInput + nOffset;
			nTemp *= nScale;
			nTemp >>= 10; // div 1024
			return (si16)nTemp;
		}

		LinearCalibration()
		{
			// set identity
			nOffset = 0;
			nScale = 1024;
		}

		LinearCalibration(si16 nOffset_, ui16 nScale_) :
			nOffset( nOffset_ ), nScale( nScale_ )
		{
		}

	};

	// input_storage_class, output_storage_class, input_variabe, output_variable, internal_calculations, length
	template <class Ttabin, class Ttabout, class Tvarin, class Tvarout, class Tcalc, int N_>
	class CalibrationCurve
	{
	public:
		Ttabin inp[N_];
		Ttabout out[N_];

		CalibrationCurve()
		{
			for ( int i=0; i<(int)COUNT(inp); i++ )
			{
				Tcalc t = 255*i/COUNT(inp);
				inp[i] = (Ttabin)t;
				out[i] = (Ttabout)t;
			}
		}

		CalibrationCurve(const Ttabin* tin, const Ttabout* tout)
		{
			for ( int i=0; i<(int)COUNT(inp); i++ )
			{
				inp[i] = tin[i];
				out[i] = tout[i];
			}
		}

		Tvarout Get( Tvarin i )
		{
			int b = 0, e = COUNT(inp)-1;
			while ( e-b > 1 )	
			{
				int m = (b+e) >> 1;
				if ( i >= inp[m] )
					b = m;
				else
					e = m;
			}
			if (sizeof(Tvarin) == sizeof(float))
			{
//			BIOS::DBG::Print("CC: b=%d, e=%d\n", b, e);
			}

			Tcalc nTmp = (Tcalc)(i - inp[b]);
			nTmp *= out[e] - out[b];
			nTmp /= inp[e] - inp[b];
			nTmp += out[b];
			return (Tvarout)nTmp;

//			return (Tout)( (i - inp[b]) * (out[e] - out[b]) / (inp[e] - inp[b]) + out[b] ); 
/*
			Tin nTmp = i - inp[b];
			nTmp *= out[e] - out[b];
			nTmp /= inp[e] - inp[b];
			nTmp += out[b];
			return (Tout)nTmp;
*/
		}		
	};

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
	AnalogChannel CH1;
	AnalogChannel CH2;
	DigitalChannel CH3;
	DigitalChannel CH4;
	TimeBase Time;
	Trigger Trig;
	Generator Gen;

	typedef CalibrationCurve<ui8, ui8, int, int, int, 6> ChannelCalibrationCurve;
	ChannelCalibrationCurve calCH1[AnalogChannel::_ResolutionMax];
	ChannelCalibrationCurve calCH2[AnalogChannel::_ResolutionMax];
	ChannelCalibrationCurve calPosCH1;
	CalibrationCurve<float, ui16, float, int, float, 2> calDAC;

#include "Calibrate.h"

	CSettings();
	void Save();
	void Load();
};
#endif