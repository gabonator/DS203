#ifndef __MAINMENU_H__
#define __MAINMENU_H__

#include <Source/Framework/Wnd.h>
#include <Source/Core/Controls.h>

class CWndModuleSelector : public CWnd
{
public:
	struct TMenuBlockStruct
	{
		CWndMenuBlock* m_pWnd;
		PCSTR m_strLabel;
		ui16 m_clr;
		PCSTR m_strTarget;
	};

public:
	CWndMenuBlock	m_itmOscilloscope;
	CWndMenuBlock	m_itmSpectrum;
	CWndMenuBlock	m_itmLogic;
	CWndMenuBlock	m_itmResponse;
	CWndMenuBlock	m_itmGenerator;
	CWndMenuBlock	m_itmSettings;
	CWndMenuBlock	m_itmUser;
	CWndMenuBlock	m_itmAbout;
	CWndMenuBlock	m_itmDmm;

	virtual void Create(CWnd *pParent, ui16 dwFlags);
	virtual void OnPaint();
	virtual void OnKey(ui16 nKey);

private:
	const TMenuBlockStruct* GetLayout();
	int _GetItemId(CWnd* pWnd);
	CWnd* _GetWindowById(int nId);
};

#endif