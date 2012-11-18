#include "MenuGenMain.h"
#include <Source/Gui/MainWnd.h>

/*virtual*/ void CWndMenuGenerator::Create(CWnd *pParent, ui16 dwFlags) 
{
	CCoreGenerator::Update();

	CWnd::Create("CWndMenuGenerator", dwFlags, CRect(280-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);

	m_itmWave.Create( this );
	m_itmFreq.Create("Frequency", RGB565(b00040), 2, this );
	m_itmAmpl.Create("Aplitude", RGB565(b00040), 2, this );
	
	OnMessage( &m_itmWave, ToWord('i', 'u'), 0 ); // force update
}

/*virtual*/ void CWndMenuGenerator::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	#define CPUCLOCK (72 MHz)
	#define MHz *1000000

	// LAYOUT ENABLE/DISABLE FROM TOP MENU BAR
	if (code == ToWord('L', 'D') )
	{
		MainWnd.m_wndSignalGraph.ShowWindow( SwHide );
		return;
	}

	if (code == ToWord('L', 'E') )
	{
		MainWnd.m_wndSignalGraph.ShowWindow( SwShow );
		if ( Settings.Gen.Wave == CSettings::Generator::_Dc )
			MainWnd.m_wndSignalGraph.Setup( NULL, 0 );
		else
			MainWnd.m_wndSignalGraph.Setup( CCoreGenerator::GetRamDac(), CCoreGenerator::GetRamLen() );
		return;
	}

	if ( code == ToWord('i', 'u') && pSender == &m_itmWave )
	{
		CCoreGenerator::Update();
		m_itmFreq.Invalidate();
		if ( Settings.Gen.Wave == CSettings::Generator::_Dc )
			MainWnd.m_wndSignalGraph.Setup( NULL, 0 );
		else
			MainWnd.m_wndSignalGraph.Setup( CCoreGenerator::GetRamDac(), CCoreGenerator::GetRamLen() );
	}

	if ( code == ToWord('i', 'u') && pSender == &m_itmFreq )
	{
		CCoreGenerator::Update();
	}

	if ( code == ToWord('i', 'u') && pSender == &m_itmAmpl )
	{
		Settings.Gen.nScale = (int)(m_itmAmpl.GetAmplitude() / 2.5f * 0x10000); // Settings.DacCalib.Get( m_itmAmpl.GetAmplitude() );
		CCoreGenerator::Update();
		if ( Settings.Gen.Wave == CSettings::Generator::_Dc )
			MainWnd.m_wndSignalGraph.Setup( NULL, 0 );
		else
			MainWnd.m_wndSignalGraph.Setup( CCoreGenerator::GetRamDac(), CCoreGenerator::GetRamLen() );
	}
}

