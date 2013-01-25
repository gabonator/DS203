#define DECLARE_COMMON( type ) \
	static CEvalMappedInteger<type>* GetCommon##type () \
	{	\
		static CEvalMappedInteger<type> m_common;	\
		return &m_common;	\
	}
#define DECLARE_DYNAVAR( type, name, target ) \
	static CEvalOperand name( CArray<CEvalOperand>& arrOperands )	\
	{	\
		GetCommon##type()->Reset( (type*)& target );	\
		return CEvalOperand( GetCommon##type() );	\
	}
#define DECLARE_CONST( name, value ) \
	static CEvalOperand name( CArray<CEvalOperand>& arrOperands )	\
	{	\
		return CEvalOperand( (int)value );	\
	}
#define DECLARE_FUNCTION(name)	\
	static CEvalOperand name( CArray<CEvalOperand>& arrOperands )

DECLARE_FUNCTION( _SetLoad )
{
	Settings.Load();
	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _SetSave )
{
	Settings.Save();
	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _SetReset )
{
	Settings.Reset();
	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _About )
{
	BIOS::SERIAL::Send( 
		"DS203 terminal service v2.0\n"
		"Gabriel Valky 2013, Slovakia\n"
		"\n"
		"Try 'Help();' to get list of available commands\n"
		"\n");

	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _Help )
{
	const CEvalToken* pTokens = getOperators();

	BIOS::SERIAL::Send( "List of available commands:\n\n" );

	for (; pTokens->m_pszToken; pTokens++)
	{
		BIOS::SERIAL::Send( "  " );
		switch ( pTokens->m_ePrecedence )
		{
		case CEvalToken::PrecedenceLow:
			BIOS::SERIAL::Send( "Low-Func" ); break;
		case CEvalToken::PrecedenceFunc:
			BIOS::SERIAL::Send( "Function" ); break;
		case CEvalToken::PrecedenceConst:
			BIOS::SERIAL::Send( "Constant" ); break;
		case CEvalToken::PrecedenceVar:
			BIOS::SERIAL::Send( "Variable" ); break;
		default: 
			BIOS::SERIAL::Send( "Unknown " ); break;
		}
		BIOS::SERIAL::Send( "  " );
		BIOS::SERIAL::Send( pTokens->m_pszToken );
		BIOS::SERIAL::Send( "\n" );
	}
	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _WndMessage )
{
	const CEvalToken* pTokDelim = &(CEval::getOperators()[2]);		

	CWnd* pTarget = &MainWnd;
	int nMessage = 0, nData = 0;

	if ( arrOperands.GetSize() == 3 )
	{
		_ASSERT( arrOperands[-3].Is( CEvalOperand::eoInteger ) );
		_ASSERT( arrOperands[-2].Is( pTokDelim ) );
		_ASSERT( arrOperands[-1].Is( CEvalOperand::eoInteger ) );

		nMessage = arrOperands[-3].GetInteger();
		nData = arrOperands[-1].GetInteger();
		arrOperands.Resize(-3);
	} else
	if ( arrOperands.GetSize() == 5 )
	{
		_ASSERT( arrOperands[-5].Is( CEvalOperand::eoInteger ) );
		_ASSERT( arrOperands[-4].Is( pTokDelim ) );
		_ASSERT( arrOperands[-3].Is( CEvalOperand::eoInteger ) );
		_ASSERT( arrOperands[-2].Is( pTokDelim ) );
		_ASSERT( arrOperands[-1].Is( CEvalOperand::eoInteger ) );

		pTarget = (CWnd*)arrOperands[-5].GetInteger();
		nMessage = arrOperands[-3].GetInteger();
		nData = arrOperands[-1].GetInteger();
		arrOperands.Resize(-5);
	} else
		_SAFE( 0 );

	pTarget->WindowMessage( nMessage, nData );
	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _WndDump )
{
	int nWndStyleMask = -1;

	if ( arrOperands.GetSize() == 1 )
		nWndStyleMask = arrOperands.RemoveLast().GetInteger();		

	_SAFE( arrOperands.GetSize() == 0 );

	char msg[32];
	BIOS::DBG::sprintf( msg, "QAN=(text, json)" );
	BIOS::SERIAL::Send( msg );

	_ScanWindowRecursive( &MainWnd, nWndStyleMask, msg );

	BIOS::SERIAL::Putch( 0x1b ); // escape at end of text
	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _WndGetInfo )
{
	_SAFE( arrOperands.GetSize() == 1 );
	ui32 dwPtr = arrOperands.RemoveLast().GetInteger();		

	char msg[32];
	BIOS::DBG::sprintf( msg, "QAN=(text, json)" );
	BIOS::SERIAL::Send( msg );

	CWnd* pWnd = (CWnd*)dwPtr;
	_ScanWindowRecursive( pWnd, 0, msg );

	BIOS::SERIAL::Putch( 0x1b ); // escape at end of text
	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _WndGetFocus )
{
	ui32 dwFocus = (ui32)MainWnd.GetFocus();
	return (UINT)dwFocus;
}

DECLARE_FUNCTION( _Tty )
{
	Settings.Runtime.m_bUartTest = false;
	Settings.Runtime.m_bUartEcho = true;
	return "Tty ready\r\n";
}

static void _ScanWindowRecursive( CWnd* pWnd, int nMask, char* msg )
{
	// {ptr:0x01234567, name:"MainWnd", style:8, rect:{left:20,top:10,right:30,bottom:40}, children:[...] }
	BIOS::SERIAL::Send( "{" );
		BIOS::DBG::sprintf( msg, "ptr:0x%x, ", (ui32)pWnd );
		BIOS::SERIAL::Send( msg );
		BIOS::DBG::sprintf( msg, "name:\"%s\", ", pWnd->m_pszId );
		BIOS::SERIAL::Send( msg );
		BIOS::DBG::sprintf( msg, "style:%d, ", pWnd->m_dwFlags );
		BIOS::SERIAL::Send( msg );
		BIOS::SERIAL::Send( "rect:{" );
			BIOS::DBG::sprintf( msg, "left:%d,", pWnd->m_rcClient.left );
			BIOS::SERIAL::Send( msg );
			BIOS::DBG::sprintf( msg, "top:%d,", pWnd->m_rcClient.top );
			BIOS::SERIAL::Send( msg );
			BIOS::DBG::sprintf( msg, "right:%d,", pWnd->m_rcClient.right );
			BIOS::SERIAL::Send( msg );
			BIOS::DBG::sprintf( msg, "bottom:%d", pWnd->m_rcClient.bottom );
			BIOS::SERIAL::Send( msg );
		BIOS::SERIAL::Send( "}" );

		if ( pWnd->m_pFirst && nMask != 0 )
		{
			BIOS::SERIAL::Send( ", children:[" );
			CWnd *pChild = pWnd->m_pFirst;
			bool bFirst = true;
			while (pChild)
			{
				if ( nMask == -1 || pChild->m_dwFlags & nMask )
				{
					if ( bFirst )
						bFirst = false;
					else
						BIOS::SERIAL::Send(", ");
					_ScanWindowRecursive( pChild, nMask, msg );
				}
				pChild = pChild->m_pNext;
			}
			BIOS::SERIAL::Send( "]" );
		}
	BIOS::SERIAL::Send( "}" );
}

DECLARE_FUNCTION( _AdcTransfer )
{
	const CEvalToken* pTokDelim = &(CEval::getOperators()[2]);		

	int nBegin = 0, nEnd = BIOS::ADC::GetCount();

	if ( arrOperands.GetSize() == 0 )
	{
	} else
	{	
		_SAFE( arrOperands.GetSize() == 3 );
		_ASSERT( arrOperands[-3].Is( CEvalOperand::eoInteger ) );
		_ASSERT( arrOperands[-2].Is( pTokDelim ) );
		_ASSERT( arrOperands[-1].Is( CEvalOperand::eoInteger ) );

		nBegin = arrOperands[-3].GetInteger();
		nEnd = arrOperands[-1].GetInteger();

		arrOperands.Resize(-3);
	}

	char msg[32];
	BIOS::DBG::sprintf(msg, "QAN=(binary, len=%d)", (nEnd-nBegin)*4 );
	BIOS::SERIAL::Send( msg );

	for (int i = nBegin; i < nEnd; i++ )
	{
		BIOS::ADC::TSample Sample = BIOS::ADC::GetAt( i );
		ui8* buf = (ui8*)&Sample;

		BIOS::SERIAL::Putch( buf[0] );
		BIOS::SERIAL::Putch( buf[1] );
		BIOS::SERIAL::Putch( buf[2] );
		BIOS::SERIAL::Putch( 0 );	// shared with other applications
	}

	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _AdcUpdate )
{
	CCoreOscilloscope::ConfigureAdc();
	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _TrigUpdate )
{
	CCoreOscilloscope::ConfigureTrigger();
	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _CoreUpdate )
{
	CCoreSettings::Update();
	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _GenUpdate2 )
{
	CCoreGenerator::Update();
	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _LcdUpdate )
{
	MainWnd.Invalidate();
	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _AdcResetWritePtr )
{
	BIOS::ADC::Restart();
	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _AdcGetWritePtr )
{
	return CEvalOperand( BIOS::ADC::GetPointer() );
}

DECLARE_FUNCTION( _BiosSet )
{
	_SAFE( arrOperands.GetSize() == 3 );
	const CEvalToken* pTokDelim = &(CEval::getOperators()[2]);		

	_ASSERT( arrOperands[-3].Is( CEvalOperand::eoInteger ) );
	_ASSERT( arrOperands[-2].Is( pTokDelim ) );
	_ASSERT( arrOperands[-1].Is( CEvalOperand::eoInteger ) );

		
	int nKey = arrOperands[-3].GetInteger();
	int nValue = arrOperands[-1].GetInteger();
	arrOperands.Resize(-3);

	BIOS::SYS::Set( nKey, nValue );

	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _BiosGet )
{
	int nKey = 0, nSub = 0;

	if ( arrOperands.GetSize() == 3 )
	{
		const CEvalToken* pTokDelim = &(CEval::getOperators()[2]);		

		_ASSERT( arrOperands[-3].Is( CEvalOperand::eoInteger ) );
		_ASSERT( arrOperands[-2].Is( pTokDelim ) );
		_ASSERT( arrOperands[-1].Is( CEvalOperand::eoInteger ) );
		nKey = arrOperands[-3].GetInteger();
		nSub = arrOperands[-1].GetInteger();
		arrOperands.Resize( -3 );
	} else
	if ( arrOperands.GetSize() == 1 )
	{
		_ASSERT( arrOperands[-1].Is( CEvalOperand::eoInteger ) );
		nKey = arrOperands[-1].GetInteger();
		arrOperands.Resize( -1 );
	} else
		_SAFE( 0 );

	int nRet = BIOS::SYS::Get( nKey, nSub );
	return CEvalOperand( nRet );
}

	
// new interface implementation
DECLARE_COMMON( NATIVEENUM )
DECLARE_COMMON( ui32 )
DECLARE_COMMON( ui16 )
DECLARE_COMMON( si16 )
DECLARE_COMMON( int )

DECLARE_DYNAVAR( NATIVEENUM, _Ch1Enabled, Settings.CH1.Enabled )
DECLARE_DYNAVAR( NATIVEENUM, _Ch1Coupling, Settings.CH1.Coupling )
DECLARE_DYNAVAR( NATIVEENUM, _Ch1Resolution, Settings.CH1.Resolution )
DECLARE_DYNAVAR( NATIVEENUM, _Ch1Probe, Settings.CH1.Probe )
DECLARE_DYNAVAR( ui16, _Ch1Position, Settings.CH1.u16Position )
DECLARE_DYNAVAR( ui16, _Ch1Color, Settings.CH1.u16Color )

DECLARE_DYNAVAR( NATIVEENUM, _Ch2Enabled, Settings.CH2.Enabled )
DECLARE_DYNAVAR( NATIVEENUM, _Ch2Coupling, Settings.CH2.Coupling )
DECLARE_DYNAVAR( NATIVEENUM, _Ch2Resolution, Settings.CH2.Resolution )
DECLARE_DYNAVAR( NATIVEENUM, _Ch2Probe, Settings.CH2.Probe )
DECLARE_DYNAVAR( ui16, _Ch2Position, Settings.CH2.u16Position )
DECLARE_DYNAVAR( ui16, _Ch2Color, Settings.CH2.u16Color )

DECLARE_DYNAVAR( NATIVEENUM, _Ch3Enabled, Settings.CH3.Enabled )
DECLARE_DYNAVAR( NATIVEENUM, _Ch3Polarity, Settings.CH3.Polarity )
DECLARE_DYNAVAR( ui16, _Ch3Position, Settings.CH3.u16Position )
DECLARE_DYNAVAR( ui16, _Ch3Color, Settings.CH3.u16Color )

DECLARE_DYNAVAR( NATIVEENUM, _Ch4Enabled, Settings.CH4.Enabled )
DECLARE_DYNAVAR( NATIVEENUM, _Ch4Polarity, Settings.CH4.Polarity )
DECLARE_DYNAVAR( ui16, _Ch4Position, Settings.CH4.u16Position )
DECLARE_DYNAVAR( ui16, _Ch4Color, Settings.CH4.u16Color )

DECLARE_DYNAVAR( NATIVEENUM, _TimeResolution, Settings.Time.Resolution )

DECLARE_DYNAVAR( NATIVEENUM, _TrigSync, Settings.Trig.Sync )
DECLARE_DYNAVAR( NATIVEENUM, _TrigType, Settings.Trig.Type )
DECLARE_DYNAVAR( NATIVEENUM, _TrigSource, Settings.Trig.Source )
DECLARE_DYNAVAR( si16, _TrigLevel, Settings.Trig.nLevel )
DECLARE_DYNAVAR( NATIVEENUM, _TrigState, Settings.Trig.State )

DECLARE_DYNAVAR( NATIVEENUM, _DispAxes, Settings.Disp.Axes )
DECLARE_DYNAVAR( NATIVEENUM, _DispDraw, Settings.Disp.Draw )
DECLARE_DYNAVAR( NATIVEENUM, _DispAverage, Settings.Disp.Average )
DECLARE_DYNAVAR( NATIVEENUM, _DispPersist, Settings.Disp.Persist )
DECLARE_DYNAVAR( NATIVEENUM, _DispGrid, Settings.Disp.Grid )
DECLARE_DYNAVAR( NATIVEENUM, _DispAxis, Settings.Disp.Axis )

DECLARE_DYNAVAR( NATIVEENUM, _SpecWindow, Settings.Spec.Window )
DECLARE_DYNAVAR( NATIVEENUM, _SpecDisplay, Settings.Spec.Display )

DECLARE_DYNAVAR( NATIVEENUM, _RunBacklight, Settings.Runtime.m_nBacklight )
DECLARE_DYNAVAR( NATIVEENUM, _RunVolume, Settings.Runtime.m_nVolume )

DECLARE_CONST( _WndWmPaint, CWnd::WmPaint )
DECLARE_CONST( _WndWmKey, CWnd::WmKey )
DECLARE_CONST( _WndWmTick, CWnd::WmTick )
DECLARE_CONST( _WndWmBroadcast, CWnd::WmBroadcast )
DECLARE_CONST( _WndWsHidden, CWnd::WsHidden )
DECLARE_CONST( _WndWsVisible, CWnd::WsVisible )
DECLARE_CONST( _WndWsNoActivate, CWnd::WsNoActivate )
DECLARE_CONST( _WndWsModal, CWnd::WsModal )
DECLARE_CONST( _WndWsTick, CWnd::WsTick )
DECLARE_CONST( _WndWsListener, CWnd::WsListener )
DECLARE_CONST( _KeyLeft, BIOS::KEY::KeyLeft )
DECLARE_CONST( _KeyRight, BIOS::KEY::KeyRight )
DECLARE_CONST( _KeyUp, BIOS::KEY::KeyUp )
DECLARE_CONST( _KeyDown, BIOS::KEY::KeyDown )
DECLARE_CONST( _KeyEnter, BIOS::KEY::KeyEnter )
DECLARE_CONST( _KeyEscape, BIOS::KEY::KeyEscape )
DECLARE_CONST( _KeyF1, BIOS::KEY::KeyFunction )
DECLARE_CONST( _KeyF2, BIOS::KEY::KeyFunction2 )
DECLARE_CONST( _KeyF3, BIOS::KEY::KeyS1 )
DECLARE_CONST( _KeyF4, BIOS::KEY::KeyS2 )

DECLARE_DYNAVAR( int, _SetCoreUartTest, Settings.Runtime.m_bUartTest )
DECLARE_DYNAVAR( int, _SetCoreUartEcho, Settings.Runtime.m_bUartEcho )
	
DECLARE_FUNCTION( _AdcEnabled )
{
	class CVariable : public CEval::CEvalVariable
	{
		virtual void Set( CEval::CEvalOperand& pOperand )
		{
			BIOS::ADC::Enable( pOperand.GetInteger() ? true : false );
		}
		virtual CEval::CEvalOperand Get()
		{		
			return CEval::CEvalOperand( (INT) BIOS::ADC::Enabled() );
		}
	} static s_Instance;
	return CEvalOperand(&s_Instance);
}

DECLARE_FUNCTION( _MenuItem )
{
	class CVariable : public CEval::CEvalVariable
	{
		virtual void Set( CEval::CEvalOperand& pOperand )
		{
			MainWnd.SendMessage( &MainWnd.m_wndToolBar, ToWord('g', '2'), pOperand.GetInteger() );
		}
		virtual CEval::CEvalOperand Get()
		{		
			return CEval::CEvalOperand( (INT) Settings.Runtime.m_nMenuItem );
		}
	} static s_Instance;
	return CEvalOperand(&s_Instance);
}

DECLARE_FUNCTION( _Mouse )
{
	const CEvalToken* pTokDelim = &(CEval::getOperators()[2]);		

	_SAFE( arrOperands.GetSize() == 5 );

	_ASSERT( arrOperands[-5].Is( CEvalOperand::eoInteger ) );
	_ASSERT( arrOperands[-4].Is( pTokDelim ) );
	_ASSERT( arrOperands[-3].Is( CEvalOperand::eoInteger ) );
	_ASSERT( arrOperands[-2].Is( pTokDelim ) );
	_ASSERT( arrOperands[-1].Is( CEvalOperand::eoInteger ) );

	int nX = arrOperands[-5].GetInteger();
	int nY = arrOperands[-3].GetInteger();
	int nB = arrOperands[-1].GetInteger();

	arrOperands.Resize(-5);

	MainWnd.m_Mouse.Override( nX, nY, nB );

	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_CONST( _LcdWidth, BIOS::LCD::LcdWidth )
DECLARE_CONST( _LcdHeight, BIOS::LCD::LcdHeight )

DECLARE_FUNCTION( _LcdGetBitmap )
{
	const CEvalToken* pTokDelim = &(CEval::getOperators()[2]);		

	_SAFE( arrOperands.GetSize() == 7 );
	_ASSERT( arrOperands[-7].Is( CEvalOperand::eoInteger ) );
	_ASSERT( arrOperands[-6].Is( pTokDelim ) );
	_ASSERT( arrOperands[-5].Is( CEvalOperand::eoInteger ) );
	_ASSERT( arrOperands[-4].Is( pTokDelim ) );
	_ASSERT( arrOperands[-3].Is( CEvalOperand::eoInteger ) );
	_ASSERT( arrOperands[-2].Is( pTokDelim ) );
	_ASSERT( arrOperands[-1].Is( CEvalOperand::eoInteger ) );

	CRect rcTransfer( 
		arrOperands[-7].GetInteger(),
		arrOperands[-5].GetInteger(),
		arrOperands[-3].GetInteger(),
		arrOperands[-1].GetInteger() );

	if ( rcTransfer.left < 0 || rcTransfer.top < 0 ||
		rcTransfer.right >= BIOS::LCD::LcdWidth ||
		rcTransfer.bottom >= BIOS::LCD::LcdHeight )
	{
		return CEvalOperand(CEvalOperand::eoError);
	}

	int nPixels = ( rcTransfer.Width()+1 ) * ( rcTransfer.Height()+1 ) * 2;

	char msg[32];
	BIOS::DBG::sprintf(msg, "QAN=(binary, len=%d)", nPixels );
	BIOS::SERIAL::Send( msg );

	for ( int y = rcTransfer.top; y <= rcTransfer.bottom; y++ )
		for ( int x = rcTransfer.left; x <= rcTransfer.right; x++ )
		{
			ui16 clr = BIOS::LCD::GetPixel( x, y );
			BIOS::SERIAL::Putch( clr >> 8 );
			BIOS::SERIAL::Putch( clr & 0xff );
		}

	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_FUNCTION( _MemRead )
{
	_SAFE( arrOperands.GetSize() == 1 );

	_ASSERT( arrOperands[-1].Is( CEvalOperand::eoInteger ) );

	int nAddress = arrOperands[-1].GetInteger();
	arrOperands.Resize(-1);
	ui32* pData = (ui32*)nAddress;
	ui32 nData = *pData;

	return (UINT)nData;
}

DECLARE_FUNCTION( _MemWrite )
{
	const CEvalToken* pTokDelim = &(CEval::getOperators()[2]);		

	_SAFE( arrOperands.GetSize() == 3 );

	_ASSERT( arrOperands[-3].Is( CEvalOperand::eoInteger ) );
	_ASSERT( arrOperands[-2].Is( pTokDelim ) );
	_ASSERT( arrOperands[-1].Is( CEvalOperand::eoInteger ) );

	int nAddress = arrOperands[-3].GetInteger();
	int nValue = arrOperands[-1].GetInteger();

	arrOperands.Resize(-3);

	ui32* pData = (ui32*)nAddress;
	*pData = nValue;

	return CEvalOperand(CEvalOperand::eoNone);
}

DECLARE_DYNAVAR( ui32, _IoGpioACrl, *BIOS::GPIO::GetRegister(BIOS::GPIO::PortA, BIOS::GPIO::RegCrl) )
DECLARE_DYNAVAR( ui32, _IoGpioACrh, *BIOS::GPIO::GetRegister(BIOS::GPIO::PortA, BIOS::GPIO::RegCrh) )
DECLARE_DYNAVAR( ui32, _IoGpioAIdr, *BIOS::GPIO::GetRegister(BIOS::GPIO::PortA, BIOS::GPIO::RegIdr) )
DECLARE_DYNAVAR( ui32, _IoGpioAOdr, *BIOS::GPIO::GetRegister(BIOS::GPIO::PortA, BIOS::GPIO::RegOdr) )
DECLARE_DYNAVAR( ui32, _IoGpioABsrr, *BIOS::GPIO::GetRegister(BIOS::GPIO::PortA, BIOS::GPIO::RegBsrr) )
DECLARE_DYNAVAR( ui32, _IoGpioALckr, *BIOS::GPIO::GetRegister(BIOS::GPIO::PortA, BIOS::GPIO::RegLckr) )
