	static CEvalOperand _ADC_Get( CArray<CEvalOperand>& arrOperands )
	{
		class CAdcStream : public CStream {
		public:
			int m_nPos;
			union {
				ui32 m_ui32;
				ui8 m_arr8[4];
			} m_LastSample;

		public:		
			static CAdcStream* getInstance()
			{
				static CAdcStream staticStream;
				staticStream.m_nPos = 0;
				return &staticStream;
			}

		public:
			virtual INT GetSize()
			{
				return 4096*4;
			};

			virtual CHAR Get()
			{
				//if ( m_nPos == 0 )
				//	BIOS::ADC::Restart();
				if ( (m_nPos & 3) == 0 )
					m_LastSample.m_ui32 = BIOS::ADC::GetAt(m_nPos >> 2);

				return m_LastSample.m_arr8[ m_nPos++ & 3 ];
			};

		};

		return CEvalOperand(CAdcStream::getInstance());
	}

	static CEvalOperand _OscGetViewData( CArray<CEvalOperand>& arrOperands )
	{
		class CAdcStream : public CStream {
		public:
			int m_nPos;

		public:		
			static CAdcStream* getInstance()
			{
				static CAdcStream staticStream;
				staticStream.m_nPos = 0;
				return &staticStream;
			}

		public:
			virtual INT GetSize()
			{
				return 300*2;
			};

			virtual CHAR Get()
			{
				const BIOS::ADC::TSample& sample = BIOS::ADC::GetAt( 10 + m_nPos/2 );
				CHAR chOut = (m_nPos & 1) ? (CHAR) (sample >> 8) : (CHAR)sample;
				m_nPos++;
				return chOut;
			};

		};

		return CEvalOperand(CAdcStream::getInstance());
	}

	static CEvalOperand _CH1_Coupling( CArray<CEvalOperand>& arrOperands )
	{
		static CEvalMappedInteger<NATIVEENUM> coupling( (NATIVEENUM*)&Settings.CH1.Coupling );
		return CEvalOperand( &coupling );
	}

	static CEvalOperand _CH1_Offset( CArray<CEvalOperand>& arrOperands )
	{
		static CEvalMappedInteger<si16> offset( &Settings.CH1.u16Position );
		return CEvalOperand( &offset );
	}

	static CEvalOperand _CH1_Resolution( CArray<CEvalOperand>& arrOperands )
	{
		static CEvalMappedInteger<NATIVEENUM> resolution( (NATIVEENUM*)&Settings.CH1.Resolution );
		return CEvalOperand( &resolution );
	}

	static CEvalOperand _CH1_Color( CArray<CEvalOperand>& arrOperands )
	{
		static CEvalMappedInteger<ui16> color( &Settings.CH1.u16Color );
		return CEvalOperand( &color );
	}

	static CEvalOperand _CH2_Coupling( CArray<CEvalOperand>& arrOperands )
	{
		static CEvalMappedInteger<NATIVEENUM> coupling( (NATIVEENUM*)&Settings.CH2.Coupling );
		return CEvalOperand( &coupling );
	}

	static CEvalOperand _CH2_Offset( CArray<CEvalOperand>& arrOperands )
	{
		static CEvalMappedInteger<si16> offset( &Settings.CH2.u16Position );
		return CEvalOperand( &offset );
	}

	static CEvalOperand _CH2_Resolution( CArray<CEvalOperand>& arrOperands )
	{
		static CEvalMappedInteger<NATIVEENUM> resolution( (NATIVEENUM*)&Settings.CH2.Resolution );
		return CEvalOperand( &resolution );
	}

	static CEvalOperand _CH2_Color( CArray<CEvalOperand>& arrOperands )
	{
		static CEvalMappedInteger<ui16> color( &Settings.CH2.u16Color );
		return CEvalOperand( &color );
	}

	static CEvalOperand _ALL_Setup( CArray<CEvalOperand>& arrOperands )
	{
		CCoreOscilloscope::ConfigureAdc();
		return CEvalOperand( CEvalOperand::eoNone );
	}

	static CEvalOperand _TIME_Offset( CArray<CEvalOperand>& arrOperands )
	{
		static CEvalMappedInteger<si16> resolution( &Settings.Time.Shift );
		return CEvalOperand( &resolution );
	}

	static CEvalOperand _TIME_Resolution( CArray<CEvalOperand>& arrOperands )
	{
		static CEvalMappedInteger<NATIVEENUM> resolution( (NATIVEENUM*)&Settings.Time.Resolution );
		return CEvalOperand( &resolution );
	}
	static CEvalOperand _ENUM_Ampl( CArray<CEvalOperand>& arrOperands )
	{
		int nValue = arrOperands.RemoveLast().GetInteger();
		if ( nValue < 0 || nValue > CSettings::AnalogChannel::_ResolutionMax )
			return CEvalOperand( CEvalOperand::eoError );

		PCSTR strEnum = CSettings::AnalogChannel::ppszTextResolution[ nValue ];
		return CEvalOperand( strEnum, strlen(strEnum) );
	}

	static CEvalOperand _ENUM_Time( CArray<CEvalOperand>& arrOperands )
	{
		int nValue = arrOperands.RemoveLast().GetInteger();
		if ( nValue < 0 || nValue > CSettings::TimeBase::_ResolutionMax )
			return CEvalOperand( CEvalOperand::eoError );

		PCSTR strEnum = CSettings::TimeBase::ppszTextResolution[ nValue ];
		return CEvalOperand( strEnum, strlen(strEnum) );
	}

	static CEvalOperand _ADC_Count( CArray<CEvalOperand>& arrOperands )
	{
		return (INT)4096;
	}

	static CEvalOperand _Print( CArray<CEvalOperand>& arrOperands )
	{
		const CEvalToken* pTokDelim = &(CEval::getOperators()[2]);		
		if ( arrOperands.GetSize() == 1 )
		{
			CEvalOperand& op = arrOperands[-1];
			op.m_Data.m_pString[ op.m_Data.m_pData32[1] ] = 0;
			PSTR strPrint = op.m_Data.m_pString;			
			BIOS::DBG::Print(strPrint);

			arrOperands.Resize(-1);
		} else
		{	
			_SAFE( arrOperands.GetSize() == 5 );
			_ASSERT( arrOperands[-5].Is( CEvalOperand::eoInteger ) );
			_ASSERT( arrOperands[-4].Is( pTokDelim ) );
			_ASSERT( arrOperands[-3].Is( CEvalOperand::eoInteger ) );
			_ASSERT( arrOperands[-2].Is( pTokDelim ) );
			_ASSERT( arrOperands[-1].Is( CEvalOperand::eoString ) );

			CEvalOperand& op = arrOperands[-1];
			op.m_Data.m_pString[ op.m_Data.m_pData32[1] ] = 0;
			PSTR strPrint = op.m_Data.m_pString;			

			BIOS::LCD::Print( arrOperands[-5].GetInteger(), arrOperands[-3].GetInteger(),
				0xffff, 0x0000, strPrint );

			arrOperands.Resize(-5);
		}
		return CEvalOperand(CEvalOperand::eoNone);
	}

	static CEvalOperand _Beep( CArray<CEvalOperand>& arrOperands )
	{
		_SAFE( arrOperands.GetSize() == 1 );
		_SAFE( arrOperands[-1].Is( CEvalOperand::eoInteger ) );

		BIOS::SYS::Beep( arrOperands.RemoveLast().GetInteger() );

		return CEvalOperand(CEvalOperand::eoNone);
	}

	static CEvalOperand _Sleep( CArray<CEvalOperand>& arrOperands )
	{
		_SAFE( arrOperands.GetSize() == 1 );
		_SAFE( arrOperands[-1].Is( CEvalOperand::eoInteger ) );

		BIOS::SYS::DelayMs( arrOperands.RemoveLast().GetInteger() );

		return CEvalOperand(CEvalOperand::eoNone);
	}

	static CEvalOperand _Update( CArray<CEvalOperand>& arrOperands )
	{
		MainWnd.WindowMessage( CWnd::WmPaint );
		return CEvalOperand(CEvalOperand::eoNone);
	}

	static CEvalOperand _GEN_Square( CArray<CEvalOperand>& arrOperands )
	{
		const CEvalToken* pTokDelim = &(CEval::getOperators()[2]);		
		_SAFE( arrOperands.GetSize() == 3 );
		_ASSERT( arrOperands[-3].Is( CEvalOperand::eoInteger ) );
		_ASSERT( arrOperands[-2].Is( pTokDelim ) );
		_ASSERT( arrOperands[-1].Is( CEvalOperand::eoInteger ) );

		int nPsc = arrOperands[-3].GetInteger();
		int nArr = arrOperands[-1].GetInteger();
		arrOperands.Resize(-3);

		BIOS::GEN::ConfigureSq( nPsc, nArr, (nArr+1)>>1 );
		return CEvalOperand(CEvalOperand::eoNone);
	}
/*

*/
	static CEvalOperand _GenUpdate( CArray<CEvalOperand>& arrOperands )
	{
		_SAFE( arrOperands.GetSize() == 1 );
		_SAFE( arrOperands.GetLast().Is( CEval::CEvalOperand::eoInteger ) );
		
		int nLength = arrOperands.RemoveLast().GetInteger();
		BIOS::GEN::ConfigureWave( CWndMenuGenerator::GetRamDac(), nLength );

		return CEvalOperand::eoNone;
	}
	static CEvalOperand _GenOutput( CArray<CEvalOperand>& arrOperands )
	{
		static ui16 nDacValue;

		if ( arrOperands.GetLast().Is( CEvalOperand::eoInteger ) )
		{
			_SAFE( arrOperands.GetLast().Is( CEvalOperand::eoInteger ) );	
			int nValue = arrOperands.RemoveLast().GetInteger();
			if ( nValue < 0 || nValue > 65536 )
				return CEvalOperand::eoError;

			nDacValue = nValue;
			BIOS::GEN::ConfigureWave( &nDacValue, 1 );
		
			return CEvalOperand(CEvalOperand::eoNone);
		} else
		if ( arrOperands.GetLast().Is( CEvalOperand::eoFloat ) )
		{
			float fVoltage = arrOperands.RemoveLast().GetFloat();

			ui16 nValue = Settings.DacCalib.Get( fVoltage );
		
			nDacValue = nValue;
			BIOS::GEN::ConfigureWave( &nDacValue, 1 );
		
			return CEvalOperand(CEvalOperand::eoNone);
		} else
		{
			return CEvalOperand( CEvalOperand::eoError );
		}
	}   

	static CEvalOperand _Transfer( CArray<CEvalOperand>& arrOperands )
	{
		static char strAnswer[32];
		_SAFE( arrOperands.GetSize() == 2 );

		CEvalOperand opResult = arrOperands.RemoveLast();
		CEvalOperand opPrefix = arrOperands.RemoveLast();

		_SAFE( opPrefix.m_eType == CEval::CEvalOperand::eoAttribute );

		memcpy( strAnswer, opPrefix.m_Data.m_pString, opPrefix.m_Data.m_pData32[1] );
		strAnswer[ opPrefix.m_Data.m_pData32[1] ] = 0;

		char* pParam = strAnswer + strlen(strAnswer); // go to end

 		if ( opResult.m_eType == CEval::CEvalOperand::eoVariable )
 		{
 			opResult = opResult.m_Data.m_pVariable->Get();
 		}

 		switch ( opResult.m_eType )               
 		{
 			case CEval::CEvalOperand::eoError: 
 				BIOS::DBG::sprintf(pParam, "()"); 
 			break;

 			case CEval::CEvalOperand::eoFloat: 
 				BIOS::DBG::sprintf(pParam, "(%d)", (int)opResult.m_Data.m_fData); 
 			break;

 			case CEval::CEvalOperand::eoInteger: 	
 				BIOS::DBG::sprintf(pParam, "(%d)", (int)opResult.m_Data.m_iData); 
 			break;

 			case CEval::CEvalOperand::eoString: 
 				pParam[0] = '(';
 				pParam[1] = '\'';
 				memcpy( pParam+2, opResult.m_Data.m_pString, opResult.m_Data.m_pData32[1] );
 				pParam[opResult.m_Data.m_pData32[1]+2] = '\'';
 				pParam[opResult.m_Data.m_pData32[1]+3] = ')';
 				pParam[opResult.m_Data.m_pData32[1]+4] = 0;
 			break;

 			case CEval::CEvalOperand::eoCString: 
 				pParam[0] = '(';
 				pParam[1] = '\'';
 				memcpy( pParam+2, opResult.m_Data.m_pcString, opResult.m_Data.m_pData32[1] );
 				pParam[opResult.m_Data.m_pData32[1]+2] = '\'';
 				pParam[opResult.m_Data.m_pData32[1]+3] = ')';
 				pParam[opResult.m_Data.m_pData32[1]+4] = 0;
 			break;

 			case CEval::CEvalOperand::eoNone: 
 				BIOS::DBG::sprintf(pParam, "()"); 
 			break;

 			default:
 				_ASSERT( 0 );
 			break;

 			case CEval::CEvalOperand::eoStream:  
 				_ASSERT( 0 ); // not implemented
 			break;				
 		}

		return CEvalOperand(strAnswer, strlen(strAnswer));
	}
