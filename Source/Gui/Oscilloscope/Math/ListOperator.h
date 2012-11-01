#ifndef __LISTOPERATOR_H__
#define __LISTOPERATOR_H__

class CWndListOperator : public CListBox
{
public:
	CSettings::MathOperator* m_pOper;

	CProviderColor	m_proColour;
	CProviderRgb	m_proRed, m_proGrn, m_proBlu;
	CLPItem			m_itmColour;
	CLPItem			m_itmRed, m_itmGrn, m_itmBlu;

	CProviderEnum	m_proType;
	CLPItem			m_itmType;

public:
	void Create( CSettings::MathOperator* pOper, CWnd* pParent )
	{
		m_pOper = pOper;
		CListBox::Create( "Operator", WsVisible | WsModal, CRect(100, 30, 300, 150), RGB565(8080B0), pParent );

		m_proType.Create( (const char**)CSettings::MathOperator::ppszTextType,
			(NATIVEENUM*)&pOper->Type, CSettings::MathOperator::_TypeMax );
		m_itmType.Create( "Type", CWnd::WsVisible, &m_proType, this );

		m_proColour.Create( &pOper->uiColor );
		m_proRed.Create( &pOper->uiColor, CProviderRgb::Red );
		m_proGrn.Create( &pOper->uiColor, CProviderRgb::Green );
		m_proBlu.Create( &pOper->uiColor, CProviderRgb::Blue );

		m_itmColour.Create( "Colour", CWnd::WsVisible | CWnd::WsNoActivate, &m_proColour, this );
		m_itmRed.Create(" - Red", CWnd::WsVisible, &m_proRed, this);
		m_itmGrn.Create(" - Green", CWnd::WsVisible, &m_proGrn, this);
		m_itmBlu.Create(" - Blue", CWnd::WsVisible, &m_proBlu, this);
	}
};

#endif