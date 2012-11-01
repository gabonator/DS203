#include "MenuMath.h"

#include <Source/Gui/MainWnd.h>

/*virtual*/ void CWndMenuMath::Create(CWnd *pParent, ui16 dwFlags) 
{
	CWnd::Create("CWndMenuMath", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
	m_itmOperand[0].Create( &Settings.MathA, this );
	m_itmOperand[1].Create( &Settings.MathB, this );
	m_itmOperand[2].Create( &Settings.MathC, this );
	m_itmOperator.Create( &Settings.Math, this );
}

/*virtual*/ void CWndMenuMath::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	// LAYOUT ENABLE/DISABLE FROM TOP MENU BAR
	if (code == ToWord('L', 'D') )
	{
		MainWnd.m_wndGraph.ShowWindow( SwHide );
		MainWnd.m_wndInfoBar.ShowWindow( SwHide );
		MainWnd.m_wndLReferences.ShowWindow( SwHide );
		MainWnd.m_wndTReferences.ShowWindow( SwHide );
		return;
	}

	if (code == ToWord('L', 'E') )
	{
		MainWnd.m_wndGraph.ShowWindow( SwShow );
		MainWnd.m_wndInfoBar.ShowWindow( SwShow );
		MainWnd.m_wndLReferences.ShowWindow( SwShow );
		MainWnd.m_wndTReferences.ShowWindow( SwShow );
		return;
	}
	
	// Show listbox - cursor details
	
	// calling from menu items
	bool bSenderOperand = ( pSender == &m_itmOperand[A] ||
		pSender == &m_itmOperand[B] || pSender == &m_itmOperand[C] );
	bool bSenderOperator = ( pSender == &m_itmOperator );	// value changes

	// calling from listbox
	bSenderOperand |= (pSender == &m_wndListOperand);		// exiting dialog
	bSenderOperand |= (pSender->GetParent() == &m_wndListOperand); // changing attribute in listbox
	bSenderOperator |= (pSender == &m_wndListOperator);		// exiting dialog
	bSenderOperator |= (pSender->GetParent() == &m_wndListOperator); // changing color

	if (code == ToWord('m', 'c') && bSenderOperand )	
	{
		m_wndListOperand.Create( (CSettings::MathOperand*)(NATIVEPTR)data, this );
		m_wndListOperand.StartModal( &m_wndListOperand.m_itmType );
		return;
	}
	if ( code == ToWord('o', 'k') && bSenderOperand )
	{
		// done
		m_wndListOperand.StopModal();
		return;
	}
	if ( code == ToWord('e', 'x') && bSenderOperand )
	{
		// done
		m_wndListOperand.StopModal();
		return;
	}
	if ( code == ToWord('u', 'p') && bSenderOperand )
	{
		// done
		if ( m_wndListOperand.m_pOper == &Settings.MathA )
			m_itmOperand[0].Invalidate();
		if ( m_wndListOperand.m_pOper == &Settings.MathB )
			m_itmOperand[1].Invalidate();
		if ( m_wndListOperand.m_pOper == &Settings.MathC )
			m_itmOperand[2].Invalidate();
		return;
	}

	if (code == ToWord('m', 'c') && bSenderOperator )	
	{
		m_wndListOperator.Create( (CSettings::MathOperator*)(NATIVEPTR)data, this );
		m_wndListOperator.StartModal( &m_wndListOperator.m_itmType );
		return;
	}
	if ( code == ToWord('o', 'k') && bSenderOperator )
	{
		// done
		m_wndListOperator.StopModal();
		return;
	}
	if ( code == ToWord('e', 'x') && bSenderOperator )
	{
		// done
		m_wndListOperator.StopModal();
		return;
	}
	if ( code == ToWord('u', 'p') && bSenderOperator )
	{
		// done
		m_wndListOperator.m_itmColour.Invalidate();
		m_itmOperator.Invalidate();
		return;
	}

}
