#ifndef __MAINMENU_H__
#define __MAINMENU_H__

#include <Source/Framework/Wnd.h>
#include <Source/Core/Controls.h>

class CWndModuleSelector : public CWnd
{
public:
	CWndMenuBlock	m_itmOscilloscope;
	CWndMenuBlock	m_itmSpectrum;
	CWndMenuBlock	m_itmGenerator;
	CWndMenuBlock	m_itmSettings;
	CWndMenuBlock	m_itmAbout;

	virtual void Create(CWnd *pParent, ui16 dwFlags);
	virtual void OnPaint();
	virtual void OnKey(ui16 nKey);
};

#endif