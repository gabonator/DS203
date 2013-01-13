#include <Source/Framework/Eval.h>
#include <Source/Gui/MainWnd.h>

	template <class T>
	class CEvalMappedInteger : public CEval::CEvalVariable
	{
		T* m_Variable;
	public:
	  CEvalMappedInteger( T* Variable ) :
			m_Variable( Variable )
		{
		}
	  CEvalMappedInteger() :
			m_Variable( NULL )
		{
		}

		void Reset( T* Variable )
		{
			m_Variable = Variable;
		}
		virtual void Set( CEval::CEvalOperand& pOperand )
		{
			*m_Variable = pOperand.GetInteger();
		}

		virtual CEval::CEvalOperand Get()
		{		
			return CEval::CEvalOperand( (INT) *m_Variable );
		}
	};

class CSdkEval : public CEval
{
public:
	#include "SdkV0.h"
	#include "SdkV1.h"
	
	static const CEvalToken* getOperators()
	{
		static const CEvalToken myTokens[] = 
		{
			// old interface
			{ "ADC::Count", CEvalToken::PrecedenceFunc, _ADC_Count },
			{ "ADC::Get", CEvalToken::PrecedenceFunc, _ADC_Get },

			{ "CH1::Coupling", CEvalToken::PrecedenceVar, _CH1_Coupling },
			{ "CH1::Offset", CEvalToken::PrecedenceVar, _CH1_Offset },
			{ "CH1::Resolution", CEvalToken::PrecedenceVar, _CH1_Resolution },
			{ "CH1::Color", CEvalToken::PrecedenceVar, _CH1_Color },

			{ "CH2::Coupling", CEvalToken::PrecedenceVar, _CH2_Coupling },
			{ "CH2::Offset", CEvalToken::PrecedenceVar, _CH2_Offset },
			{ "CH2::Resolution", CEvalToken::PrecedenceVar, _CH2_Resolution },
			{ "CH2::Color", CEvalToken::PrecedenceVar, _CH2_Color },

			{ "TIME::Offset", CEvalToken::PrecedenceVar, _TIME_Offset },
			{ "TIME::Resolution", CEvalToken::PrecedenceVar, _TIME_Resolution },
			{ "ALL::Setup", CEvalToken::PrecedenceFunc, _ALL_Setup },

			{ "ENUM::Time", CEvalToken::PrecedenceFunc, _ENUM_Time },
			{ "ENUM::Ampl", CEvalToken::PrecedenceFunc, _ENUM_Ampl },

			{ "GEN::Square", CEvalToken::PrecedenceFunc, _GEN_Square },
			{ "GEN::Update", CEvalToken::PrecedenceFunc, _GenUpdate },
			{ "GEN::Output", CEvalToken::PrecedenceFunc, _GenOutput },

			{ "OSC.GetViewData", CEvalToken::PrecedenceFunc, _OscGetViewData },

			{ "Print", CEvalToken::PrecedenceFunc, _Print },
			{ "Beep", CEvalToken::PrecedenceFunc, _Beep },
			{ "Sleep", CEvalToken::PrecedenceFunc, _Sleep },
			{ "Update", CEvalToken::PrecedenceFunc, _Update },

			{ "#", CEvalToken::PrecedenceFunc, _Transfer },

			// new interface
			
			// R/W variables
			{ "CH1.Enabled", CEvalToken::PrecedenceVar, _Ch1Enabled },
			{ "CH1.Coupling", CEvalToken::PrecedenceVar, _Ch1Coupling },
			{ "CH1.Resolution", CEvalToken::PrecedenceVar, _Ch1Resolution },
			{ "CH1.Probe", CEvalToken::PrecedenceVar, _Ch1Probe },
			{ "CH1.Position", CEvalToken::PrecedenceVar, _Ch1Position },
			{ "CH1.Color", CEvalToken::PrecedenceVar, _Ch1Color },
			
			{ "CH2.Enabled", CEvalToken::PrecedenceVar, _Ch1Enabled },
			{ "CH2.Coupling", CEvalToken::PrecedenceVar, _Ch1Coupling },
			{ "CH2.Resolution", CEvalToken::PrecedenceVar, _Ch1Resolution },
			{ "CH2.Probe", CEvalToken::PrecedenceVar, _Ch1Probe },
			{ "CH2.Position", CEvalToken::PrecedenceVar, _Ch1Position },
			{ "CH2.Color", CEvalToken::PrecedenceVar, _Ch1Color },

			{ "CH3.Enabled", CEvalToken::PrecedenceVar, _Ch3Enabled },
			{ "CH3.Polarity", CEvalToken::PrecedenceVar, _Ch3Polarity },
			{ "CH3.Position", CEvalToken::PrecedenceVar, _Ch3Position },
			{ "CH3.Color", CEvalToken::PrecedenceVar, _Ch3Color },

			{ "CH4.Enabled", CEvalToken::PrecedenceVar, _Ch4Enabled },
			{ "CH4.Polarity", CEvalToken::PrecedenceVar, _Ch4Polarity },
			{ "CH4.Position", CEvalToken::PrecedenceVar, _Ch4Position },
			{ "CH4.Color", CEvalToken::PrecedenceVar, _Ch4Color },

			{ "TIME.Resolution", CEvalToken::PrecedenceVar, _TimeResolution },

			{ "TRIG.Sync", CEvalToken::PrecedenceVar, _TrigSync },
			{ "TRIG.Type", CEvalToken::PrecedenceVar, _TrigType },
			{ "TRIG.Source", CEvalToken::PrecedenceVar, _TrigSource },
			{ "TRIG.State", CEvalToken::PrecedenceVar, _TrigState },
			{ "TRIG.Level", CEvalToken::PrecedenceVar, _TrigLevel },

			{ "DISP.Axes", CEvalToken::PrecedenceVar, _DispAxes },
			{ "DISP.Draw", CEvalToken::PrecedenceVar, _DispDraw },
			{ "DISP.Average", CEvalToken::PrecedenceVar, _DispAverage },
			{ "DISP.Persist", CEvalToken::PrecedenceVar, _DispPersist },
			{ "DISP.Grid", CEvalToken::PrecedenceVar, _DispGrid },
			{ "DISP.Axis", CEvalToken::PrecedenceVar, _DispAxis },

			{ "SPEC.Window", CEvalToken::PrecedenceVar, _SpecWindow },
			{ "SPEC.Display", CEvalToken::PrecedenceVar, _SpecDisplay },
			{ "RUN.Backlight", CEvalToken::PrecedenceVar, _RunBacklight },
			{ "RUN.Volume", CEvalToken::PrecedenceVar, _RunVolume },

			// functions
			{ "About", CEvalToken::PrecedenceFunc, _About },
			{ "Help", CEvalToken::PrecedenceFunc, _Help },
			{ "TTY", CEvalToken::PrecedenceConst, _Tty },	// for quick switching the output suitable for putty/other terminal

			{ "WND.Message", CEvalToken::PrecedenceFunc, _WndMessage },
			{ "WND.Dump", CEvalToken::PrecedenceFunc, _WndDump },			// output only through uart
			{ "WND.GetFocus", CEvalToken::PrecedenceFunc, _WndGetFocus },
			{ "WND.GetInfo", CEvalToken::PrecedenceFunc, _WndGetInfo },
			{ "ADC.Transfer", CEvalToken::PrecedenceFunc, _AdcTransfer },	// output only through uart

			// constants
			{ "WND::WmPaint", CEvalToken::PrecedenceConst, _WndWmPaint },
			{ "WND::WmKey", CEvalToken::PrecedenceConst, _WndWmKey },
			{ "WND::WmTick", CEvalToken::PrecedenceConst, _WndWmTick },
			{ "WND::WmBroadcast", CEvalToken::PrecedenceConst, _WndWmBroadcast },
			{ "WND::WsHidden", CEvalToken::PrecedenceConst, _WndWsHidden },
			{ "WND::WsVisible", CEvalToken::PrecedenceConst, _WndWsVisible },
			{ "WND::WsNoActivate", CEvalToken::PrecedenceConst, _WndWsNoActivate },
			{ "WND::WsModal", CEvalToken::PrecedenceConst, _WndWsModal },
			{ "WND::WsTick", CEvalToken::PrecedenceConst, _WndWsTick },
			{ "WND::WsListener", CEvalToken::PrecedenceConst, _WndWsListener },
			{ "KEY::Left", CEvalToken::PrecedenceConst, _KeyLeft },
			{ "KEY::Right", CEvalToken::PrecedenceConst, _KeyRight },
			{ "KEY::Up", CEvalToken::PrecedenceConst, _KeyUp },
			{ "KEY::Down", CEvalToken::PrecedenceConst, _KeyDown },
			{ "KEY::Enter", CEvalToken::PrecedenceConst, _KeyEnter },
			{ "KEY::Escape", CEvalToken::PrecedenceConst, _KeyEscape },
			{ "KEY::F1", CEvalToken::PrecedenceConst, _KeyF1 },
			{ "KEY::F2", CEvalToken::PrecedenceConst, _KeyF2 },
			{ "KEY::F3", CEvalToken::PrecedenceConst, _KeyF3 },
			{ "KEY::F4", CEvalToken::PrecedenceConst, _KeyF4 },

			// others
			{ "SET.Core.bUartTest", CEvalToken::PrecedenceVar, _SetCoreUartTest },
			{ "SET.Core.bUartEcho", CEvalToken::PrecedenceVar, _SetCoreUartEcho },
			{ "MENU.Item", CEvalToken::PrecedenceVar, _MenuItem },
			
			{ "SET.Save", CEvalToken::PrecedenceFunc, _SetSave },
			{ "SET.Load", CEvalToken::PrecedenceFunc, _SetLoad },
			{ "SET.Reset", CEvalToken::PrecedenceFunc, _SetReset },

			{ "ADC.Update", CEvalToken::PrecedenceFunc, _AdcUpdate },
			{ "TRIG.Update", CEvalToken::PrecedenceFunc, _TrigUpdate },
			{ "CORE.Update", CEvalToken::PrecedenceFunc, _CoreUpdate },
			{ "GEN.Update", CEvalToken::PrecedenceFunc, _GenUpdate2 },
			{ "LCD.Update", CEvalToken::PrecedenceFunc, _LcdUpdate },

			{ "ADC.ResetWritePtr", CEvalToken::PrecedenceFunc, _AdcResetWritePtr },
			{ "ADC.GetWritePtr", CEvalToken::PrecedenceFunc, _AdcGetWritePtr },
			{ "ADC.Enabled", CEvalToken::PrecedenceVar, _AdcEnabled },

			{ "BIOS.Get", CEvalToken::PrecedenceFunc, _BiosGet },
			{ "BIOS.Set", CEvalToken::PrecedenceFunc, _BiosSet },

			{ "MAIN.Mouse", CEvalToken::PrecedenceFunc, _Mouse },
			{ "LCD.GetBitmap", CEvalToken::PrecedenceFunc, _LcdGetBitmap },
			{ "LCD::Width", CEvalToken::PrecedenceConst, _LcdWidth },
			{ "LCD::Height", CEvalToken::PrecedenceConst, _LcdHeight },

			{ "MEM::Read", CEvalToken::PrecedenceFunc, _MemRead },
			{ "MEM::Write", CEvalToken::PrecedenceFunc, _MemWrite },

		
			{ NULL, -1, NULL }
		};

		return myTokens;
	}

	const CEvalToken* isOperator( char* pszExpression )
	{
		const CEvalToken* pToken = CEval::isOperator( pszExpression );
		if ( pToken )
			return pToken;

		const CEvalToken *pFind = getOperators();
		for (; pFind->m_pszToken; pFind++)
			if ( strncmp( pszExpression, pFind->m_pszToken, strlen(pFind->m_pszToken) ) == 0 )
				return (CEvalToken*)pFind;	// todo: ugly!

		return NULL;
	}
};

