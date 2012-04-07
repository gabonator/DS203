#include "Calibration.h"

#include <Source/Core/Settings.h>
#include <Source/Gui/MainWnd.h>

/*
int _F(float f)
{
	return f;
//	return *((int*)&f);
}*/
#define _F(a) a

class CStatistics 
{
public:
	struct Linear
	{
		FLOAT k, q;		// Y = k*X + q
		FLOAT r;			// correlation coefficient
	};

	template<class TYPE>
	static Linear LeastSquares( CArray<TYPE>& arrValues )
	{
		float xs = 0, x2s = 0;
		float ys = 0, y2s = 0;
		float xys = 0;
		int n = arrValues.GetSize();
		for ( int i = 0; i < n; i++ )
		{
			float x = (float)arrValues[i].getx();
			float y = (float)arrValues[i].gety();

			xs += x;
			x2s += x*x;
			ys += y;
			y2s += y*y;				
			xys += x*y;
		}			

		float intercept = (ys*x2s - xys*xs) / (float)(n*x2s - xs*xs);
		float slope = (n*xys - xs*ys)/(float)(n*x2s - xs*xs);
		float correlation = (n*xys - xs*ys) * (n*xys - xs*ys) / (float)((n*x2s-xs*xs)*(n*y2s-ys*ys));

		Linear aux;
		aux.k = (float)slope;
		aux.q = (float)intercept;
		aux.r = (float)correlation;

		return aux;
	}		

	class CurvePoint
	{
	public:
		typedef float x;
		typedef float y;
		typedef float u;

		CurvePoint()
		{
		}

		CurvePoint(float _x, float _y) :
		m_x( _x ), m_y( _y )
		{
		}

		float& getx()
		{
			return m_x;
		}	

		float& gety()
		{
			return m_y;
		}	

		float m_x, m_y;
	};	
	static float fabs(float f)
	{
		return ( f < 0 ) ? -f : f;
	}

	static void CurveFit( CArray<CurvePoint>& arrIn, CArray<CurvePoint>& arrOut, int nPoints )
	{
		arrOut.SetSize(2);
		arrOut[0] = arrIn[0];
		arrOut[1] = arrIn[arrIn.GetSize()-1];

		while ( arrOut.GetSize() < nPoints )
		{
			int nMaxDiff = 0;
			for ( int i = 1; i < arrIn.GetSize(); i++ )
			{
				if	(	fabs( arrIn[i].gety() - Interpolate( arrOut, arrIn[i].getx() ) ) > 
					fabs( arrIn[nMaxDiff].gety() - Interpolate( arrOut, arrIn[nMaxDiff].getx() ) )
					)
				{
					nMaxDiff = i;
				}
			}

			for ( int i = 0; i < arrOut.GetSize(); i++ )
			{
				if ( arrOut[i].getx() > arrIn[nMaxDiff].getx() )
				{
					arrOut.InsertAt( i, arrIn[nMaxDiff] ); 
					break;
				}
			}
		}
	}

	static float Interpolate( CArray<CurvePoint>& arrIn, float x )
	{
		int b = 0, e = arrIn.GetSize()-1;
		while ( e - b > 1 )
		{
			int m = (b+e) >> 1;
			if ( x > arrIn[m].getx() )
				b = m;
			else
				e = m;
		}
		return arrIn[b].gety() + (x - arrIn[b].getx()) / 
			(arrIn[e].getx() - arrIn[b].getx()) * (arrIn[e].gety() - arrIn[b].gety());
	}

	static void Filter( CArray<CurvePoint>& arrIn, CArray<CurvePoint>& arrOut, float f1, float f2, float f3 )
	{
		arrOut.SetSize( arrIn.GetSize() );
		for ( int i = 0; i < arrIn.GetSize(); i++ )
		{
			float v1 = arrIn[ (i>1) ? (i-1) : 0 ].gety();
			float v2 = arrIn[ i ].gety();
			float v3 = arrIn[ (i<arrIn.GetSize()-1) ? (i+1) : i ].gety();

			arrOut[i] = arrIn[i];
			arrOut[i].gety() = v1*f1 + v2*f2 + v3*f3;
		}
	}
};

class CCalibration
{
public:
	enum {
		EResolution = CSettings::AnalogChannel::_200mV,
		EMaxPPoints = (280+15)/5, // vertical positions
		EMaxVPoints = (2500-100)/100, // voltages 
		EVertMin = -15,
		EVertMax = 280,
		EVertStep = 5,		
		EVoltMinMv = 100, // 0.1V
		EVoltMaxMv = 2500, // 2.5V
		EVoltStepMv = 100 // 0.1V
	};
/*
	enum {
		EVertMin = -20,
		EVertMax = 200,
		EVertStep = 20,		
		EVoltMinMv = 0, // 0.1V
		EVoltMaxMv = 2000, // 2.5V
		EVoltStepMv = 500 // 0.1V
	};
*/
	bool bRunning;
	bool bResolutionSet;
	int nChannel;
	int nVertPos;
	int nDownload;
	int nRange;
	float fVoltage;

	float fMeasAverage;
	float fMeasVariance;

	struct MeasPair
	{ 
		// interface for interpolator {{{
		typedef float x;
		typedef float y;

		float& gety()
		{
			return fVolt;
		}

		float& getx()
		{
			return fAdc;
		}
		// }}}

		MeasPair()
		{
		}

		MeasPair(float _fVolt, float _fAdc) : fVolt( _fVolt ), fAdc( _fAdc )
		{
		}

		float fVolt;
		float fAdc;
	};

	struct KqPair
	{
		KqPair()
		{
		}

		KqPair(int nVert, float _k, float _q) : n(nVert), k(_k), q(_q)
		{
		}

		int n;
		float k;
		float q;
	};

	CCalibration()
	{
		OnInit();
	}

	void OnInit()
	{
		bRunning = false;
		bResolutionSet = false;
		nChannel = 1;
		nDownload = -1;
		nRange = EResolution;
	}

	void SetVertPos()
	{
		if ( nChannel == 1 )
			Settings.CH1.u16Position = nVertPos;
		if ( nChannel == 2 )
			Settings.CH2.u16Position = nVertPos;
	}

	void SetOutVoltage()
	{
		static ui16 nDacValue;

		ui16 nValue = Settings.calDAC.Get( fVoltage );

		nDacValue = nValue;
		BIOS::GEN::ConfigureWave( &nDacValue, 1 );
	}

	void OnStart()
	{
		bRunning = true;
		Calibration();
	}

	void OnStop()
	{
		bRunning = false;
	}

	bool Calibration()
	{
		BIOS::DBG::Print( "Setting voltage range CH%d to %s\n", nChannel,
			CSettings::AnalogChannel::ppszTextResolution[CSettings::AnalogChannel::_200mV]);

		if ( nChannel == 1 )
			Settings.CH1.Resolution = (CSettings::AnalogChannel::eResolution)nRange;
		if ( nChannel == 2 )
			Settings.CH2.Resolution = (CSettings::AnalogChannel::eResolution)nRange;

		MeasPair arrMeas_[EMaxVPoints];
		KqPair arrKq_[EMaxPPoints];
		CArray <KqPair> arrKq( arrKq_, COUNT(arrKq_) );

		for ( nVertPos = EVertMin; nVertPos < EVertMax; nVertPos += EVertStep )
		{
			SetVertPos();
			CWndMenuInput::ConfigureAdc();
			//MainWnd.WindowMessage( CWnd::WmPaint );

			CArray <MeasPair> arrMeas( arrMeas_, COUNT(arrMeas_));

			for ( fVoltage = EVoltMinMv/1000.0f; fVoltage <= (EVoltMaxMv+EVoltStepMv/4)/1000.0f; fVoltage += EVoltStepMv/1000.0f )
			{
				SetOutVoltage();		
				Wait(100);

				if ( !bRunning )
					return OnAbort();

				nDownload = 3;
				fMeasVariance = 1.0f;

				BIOS::DBG::Print("Meas Vpos=%d, U=%1fV:", nVertPos, _F(fVoltage) );	

				while (1) 
				{
					while ( nDownload > 0 )
					{		
						Wait(100);
						if ( !bRunning )
							return OnAbort();
					}

					if ( fMeasVariance < 1.0f )
						break;

					BIOS::DBG::Print("Variance too high (%3f), retrying... \n", _F(fMeasVariance));
					Wait(500);
					if ( !bRunning )
						return OnAbort();
					nDownload = 1;
				}

				BIOS::DBG::Print(" avg=%2f, var=%2f \n", _F(fMeasAverage), _F(fMeasVariance));

				if ( fMeasAverage >= 16.0f && fMeasAverage <= 240.0f )
				{
					arrMeas.Add( MeasPair( fVoltage, fMeasAverage ) );
				}

				if ( fMeasAverage >= 249.9f )
				{
					// we reached upper limit od ADC, no need
					// to increase the voltage
					break;
				}
			}	

			if ( arrMeas.GetSize() < 2 )
				continue;

 			// calculate linear approximation of measured set with least squares method
			CStatistics::Linear linApprox = CStatistics::LeastSquares<MeasPair>( arrMeas );

			if ( linApprox.r < 0.99 )
			{
				BIOS::DBG::Print("Calibration failed: part not linear (nVertPos=%d, r=%f) \n", nVertPos, _F(linApprox.r));
				return FALSE;
			}
			BIOS::DBG::Print("Result: [pos=%d, 1/k=%f, q=%f, r=%f] \n", nVertPos, 
				_F(linApprox.k), _F(linApprox.q), _F(linApprox.r));
			arrKq.Add( KqPair( nVertPos, linApprox.k, linApprox.q ) );
		}			

		// K aproximovat lomenou ciarou
		// Q aproximovat linearne
		CStatistics::CurvePoint arrPoints_[EMaxPPoints];
		CArray<CStatistics::CurvePoint> arrPoints(arrPoints_, COUNT(arrPoints_));
		CStatistics::CurvePoint arrTemp_[EMaxPPoints];
		CArray<CStatistics::CurvePoint> arrTemp(arrTemp_, COUNT(arrTemp_));
		CStatistics::CurvePoint arrFitQ_[2];
		CArray<CStatistics::CurvePoint> arrFitQ(arrFitQ_, COUNT(arrFitQ_));
		CStatistics::CurvePoint arrFitK_[6];
		CArray<CStatistics::CurvePoint> arrFitK(arrFitK_, COUNT(arrFitK_));

		// Q fitting
		arrPoints.SetSize( arrKq.GetSize() );
		for (int i=0; i<arrKq.GetSize(); i++)
			arrPoints[i] = CStatistics::CurvePoint( (float)arrKq[i].n, arrKq[i].q );

		CStatistics::CurveFit( arrPoints, arrFitQ, COUNT(arrFitQ_) );	// 2 points interpolation

		// K fitting
		arrPoints.SetSize( arrKq.GetSize() );
		for (int i=0; i<arrKq.GetSize(); i++)
			arrPoints[i] = CStatistics::CurvePoint( (float)arrKq[i].n, arrKq[i].k );

		CStatistics::Filter( arrPoints, arrTemp, 1.0f/4.0f, 2.0f/4.0f, 1.0f/4.0f );  // general average filter
		CStatistics::Filter( arrTemp, arrPoints, 1.0f/2.0f, 0.0f/2.0f, 1.0f/2.0f );	// zig zag filter

		CStatistics::CurveFit( arrPoints, arrFitK, COUNT(arrFitK_) );  // 6 points interpolation


		BIOS::DBG::Print("\nCalibration curves:\n");
		BIOS::DBG::Print("Q = [");
		DumpCurve(arrFitQ);
		BIOS::DBG::Print("]\n");
		BIOS::DBG::Print("K = [");
		DumpCurve(arrFitK);
		BIOS::DBG::Print("]\n");
		// result : arrFitQ, arrFitK
		WriteReport( arrFitK, arrFitQ );
		bRunning = false;
		return true;
	}

	void WriteReport( CArray<CStatistics::CurvePoint>& arrK, CArray<CStatistics::CurvePoint>& arrQ )
	{
		FILEINFO f;
		BIOS::DSK::Open( &f, "CALIBRATTXT", BIOS::DSK::IoWrite );
		ui8* pSector = (ui8*)BIOS::DSK::GetSharedBuffer();

		char *pBuffer = (char*)pSector;
		pBuffer += BIOS::DBG::sprintf( pBuffer, "Calibration report:\n" );
		pBuffer += BIOS::DBG::sprintf( pBuffer, "K = [" );

		for (int i=0; i<arrK.GetSize(); i++)
		{
			if ( i > 0 )
				pBuffer += BIOS::DBG::sprintf( pBuffer, ", " );
			pBuffer += BIOS::DBG::sprintf( pBuffer, "{x:%1f, y:%8f}", _F(arrK[i].getx()), _F(arrK[i].gety()));
		}

		pBuffer += BIOS::DBG::sprintf( pBuffer, "]\n" );
		pBuffer += BIOS::DBG::sprintf( pBuffer, "Q = [" );

		for (int i=0; i<arrQ.GetSize(); i++)
		{
			if ( i > 0 )
				pBuffer += BIOS::DBG::sprintf( pBuffer, ", " );
			pBuffer += BIOS::DBG::sprintf( pBuffer, "{x:%1f, y:%8f}", _F(arrQ[i].getx()), _F(arrQ[i].gety()));
		}

		pBuffer += BIOS::DBG::sprintf( pBuffer, "]\n" );
		int nLength = pBuffer-(char*)pSector;
		_ASSERT( nLength < 512 ); // single sector

		BIOS::DSK::Write( &f, pSector );
		BIOS::DSK::Close( &f, nLength );
	}

	void DumpCurve( CArray<CStatistics::CurvePoint> arrPoints )
	{
		for (int i=0; i<arrPoints.GetSize(); i++)
		{
			if ( i > 0 )
				BIOS::DBG::Print(", ");
			BIOS::DBG::Print("{x:%1f, y:%7f}", _F(arrPoints[i].getx()), _F(arrPoints[i].gety()));
		}
	}

	void OnWave( float fAverage, float fVariance )
	{
		fMeasAverage = fAverage;
		fMeasVariance = fVariance;
	}

	bool OnAbort()
	{
		BIOS::DBG::Print("Calibration aborted\n");	
		return false;
	}

	void Wait( int nTime )
	{
		CWndCalibration::Wait( nTime );
	}
};

CCalibration g_Calibration;

void CWndCalibration::OnWave()
{
	if ( g_Calibration.nDownload > 0 )
		if ( --g_Calibration.nDownload == 0 )			
		{
#ifdef _WIN32
			float fAvg = g_Calibration.nVertPos*256/200 + g_Calibration.fVoltage*32/0.2f;
			if ( fAvg < 0 ) 
				fAvg = 0;
			if ( fAvg > 255.0f )
				fAvg = 255.0f;

			g_Calibration.OnWave( fAvg, 0.5f );
			return;
#endif
			int nSum = 0;
			int nMax = -1, nMin = -1;
			for (int i=0; i</*BIOS::ADC::Length()*/ 4096; i++)
			{
				int nValue = BIOS::ADC::Get()&0xff;
				if ( nMax == -1 )
					nMax = nMin = nValue;
				else
				{
					nMax = max(nMax, nValue);
					nMin = min(nMin, nValue);
				}
				nSum += nValue;
			}

			float fAverage = (float)nSum/4096.0f;
			float fVariance = (nMax-nMin)/4.0f;	// pseudo variance :)
			g_Calibration.OnWave( fAverage, fVariance );
		}
}

void CWndCalibration::OnInit()
{
	nDownload = -1;
	g_Calibration.OnInit();
}

void CWndCalibration::OnStart()
{
	g_Calibration.OnStart();
}

void CWndCalibration::OnStop()
{
	g_Calibration.OnStop();
}

bool CWndCalibration::IsRunning()
{
	return g_Calibration.bRunning;
}
