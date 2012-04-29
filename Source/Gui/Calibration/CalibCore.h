#ifndef __CALIBCORE_H__
#define __CALIBCORE_H__

#include <Source/Framework/Thread.h>
#include <Source/Gui/Oscilloscope/Input/MenuInput.h>
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

class CCalibration : public CThread
{
public:
	enum {
		EResolution = CSettings::AnalogChannel::_200mV,
//		EMaxPPoints = (280+20)/5, // vertical positions
//		EMaxVPoints = (2500-100)/100, // voltages 

		EVertMin = -20,
		EVertMax = 280,
		EVertStep = 5,		
		EVoltMinMv = 100, // 0.1V
		EVoltMaxMv = 2500, // 2.5V
		EVoltStepMv = 100, // 0.1V
		EMaxPPoints = (EVertMax-EVertMin)/EVertStep+1, // vertical positions
		EMaxVPoints = (EVoltMaxMv-EVoltMinMv)/EVoltStepMv+1, // voltages 

	};
	enum ECalibResult {
		ECalibNone = 0, // not started
		ECalibError = 1,
		ECalibFinished = 2,
		ECalibAborted = 3
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
	CSettings::LinCalibCurve* pCurve;
	ECalibResult eResult;

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
		eResult = ECalibNone;
		bRunning = false;
		pCurve = NULL;
//		OnInit();
	}

	virtual int GetResult()
	{
		return eResult;
	}

	void Setup(NATIVEENUM calCh, NATIVEENUM calRang, CSettings::LinCalibCurve* pCurve_ )
	{
		bRunning = false;
		bResolutionSet = false;
		/*switch (calRang)
		{
		}*/
		switch (calCh)
		{
			case CSettings::Trigger::_CH1: nChannel = 1; break;
			case CSettings::Trigger::_CH2: nChannel = 2; break;
			default:
				_ASSERT(0);
		}
		nDownload = -1;
		nRange = calRang;
		pCurve = pCurve_;
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

		ui16 nValue = Settings.DacCalib.Get( fVoltage );

		nDacValue = nValue;
		BIOS::GEN::ConfigureWave( &nDacValue, 1 );
	}

	virtual void Start()
	{
		eResult = ECalibNone;
		bRunning = true;
		Run();
	}

	virtual void Stop()
	{
		bRunning = false;
	}

	virtual BOOL IsRunning()
	{
		return bRunning;
	}

	void Run()
	{
		BIOS::DBG::Print( "Setting voltage range CH%d to %s\n", nChannel,
			CSettings::AnalogChannel::ppszTextResolution[nRange]);

		if ( nChannel == 1 )
			Settings.CH1.Resolution = (CSettings::AnalogChannel::eResolution)nRange;
		if ( nChannel == 2 )
			Settings.CH2.Resolution = (CSettings::AnalogChannel::eResolution)nRange;

		MeasPair arrMeas_[EMaxVPoints];
		KqPair arrKq_[EMaxPPoints];
		CArray <KqPair> arrKq( arrKq_, COUNT(arrKq_) );

		for ( nVertPos = EVertMin; nVertPos <= EVertMax; nVertPos += EVertStep )
		{
			SetVertPos();
			CWndMenuInput::ConfigureAdc();

			CArray <MeasPair> arrMeas( arrMeas_, COUNT(arrMeas_));

			for ( fVoltage = EVoltMinMv/1000.0f; fVoltage <= (EVoltMaxMv+EVoltStepMv/4)/1000.0f; fVoltage += EVoltStepMv/1000.0f )
			{
				SetOutVoltage();		
				Sleep(100);

				if ( !bRunning )
					return OnAbort();

				nDownload = 3;
				fMeasVariance = 1.0f;

				BIOS::DBG::Print("Meas Vpos=%d, U=%1fV:", nVertPos, _F(fVoltage) );	

				int nPass = 0;
				while (1)
				{
					while ( nDownload > 0 )
					{		
						Sleep(100);
						if ( !bRunning )
							return OnAbort();
					}

					if ( fMeasVariance <= 1.0f )
						break;

					BIOS::DBG::Print("Variance too high (%3f), retrying... \n", _F(fMeasVariance));
					Sleep(500);
					if ( !bRunning )
						return OnAbort();
					nDownload = 1;
					if ( ++nPass >= 10 )
					{
						BIOS::Beep(500);
						BIOS::DBG::Print("Calibration failed: Signal too noisy, exiting...\n");
						BIOS::DelayMs(500);
						OnError();
						return;
					}
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
				BIOS::Beep(500);
				BIOS::DBG::Print("Calibration failed: part not linear (nVertPos=%d, r=%f) \n", nVertPos, _F(linApprox.r));
				BIOS::DelayMs(500);
				OnError();
				return;
				//return FALSE;
			}
			BIOS::DBG::Print("Result: [pos=%d, 1/k=%f, q=%f, r=%f] \n", nVertPos, 
				_F(1.0f/linApprox.k), _F(linApprox.q), _F(linApprox.r));
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
		//WriteReport( arrFitK, arrFitQ );
		if ( pCurve )
		{
			const static float arrMultipliers[] = 
				{50e-3f, 100e-3f, 200e-3f, 500e-3f, 1.0f, 2.0f, 5.0f, 10.0f};
			FLOAT fCorrect = 65536.0f / arrMultipliers[nRange];

			_ASSERT( arrFitQ.GetSize() == CSettings::LinCalibCurve::eQPoints );
			for ( int i = 0; i < CSettings::LinCalibCurve::eQPoints; i++ )
			{
				pCurve->m_arrCurveQin[i] = (int)arrFitQ[i].m_x;
				pCurve->m_arrCurveQout[i] = (si32)(arrFitQ[i].m_y * fCorrect);
			}
			_ASSERT( arrFitK.GetSize() == CSettings::LinCalibCurve::eKPoints );
			for ( int i = 0; i < CSettings::LinCalibCurve::eKPoints; i++ )
			{
				pCurve->m_arrCurveKin[i] = (int)arrFitK[i].m_x;
				pCurve->m_arrCurveKout[i] = (si32)(arrFitK[i].m_y * fCorrect);
			}
		}
		OnFinish();
		bRunning = false;
		//return true;
		return;
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
			pBuffer += BIOS::DBG::sprintf( pBuffer, "{x:%d, y:%d}", (int)arrK[i].getx(), (int)(arrK[i].gety()*65536.0f));
		}

		pBuffer += BIOS::DBG::sprintf( pBuffer, "]\n" );
		pBuffer += BIOS::DBG::sprintf( pBuffer, "Q = [" );

		for (int i=0; i<arrQ.GetSize(); i++)
		{
			if ( i > 0 )
				pBuffer += BIOS::DBG::sprintf( pBuffer, ", " );
			pBuffer += BIOS::DBG::sprintf( pBuffer, "{x:%d, y:%d}", (int)arrQ[i].getx(), (int)(arrQ[i].gety()*65536.0f));
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

	void OnAbort()
	{
		eResult = ECalibAborted;
		BIOS::DBG::Print("Calibration aborted\n");	
	}

	void OnError()
	{
		eResult = ECalibError;
	}

	void OnFinish()
	{
		eResult = ECalibFinished;
	}
/*
	void Wait( int nTime )
	{
		CWndCalibration::Wait( nTime );
	}
	*/
};

#endif