#ifndef __MAINMENU_H__
#define __MAINMENU_H__

#include <Source/Framework/Wnd.h>
#include <Source/Core/Controls.h>

class CWndModuleSelector : public CWnd
{
public:
	struct TMenuBlockStruct
	{
		CWndMenuBlockIcon* m_pWnd;
		PCSTR m_strLabel;
		ui16 m_clr;
		PCSTR m_strTarget;
		PVOID m_icon;
	};
private:
	static const unsigned char iconOscilloscope[];
	static const unsigned char iconSpectrum[];
	static const unsigned char iconGenerator[];
	static const unsigned char iconSettings[];
	static const unsigned char iconUser[];
	static const unsigned char iconAbout[];
	
public:
	CWndMenuBlockIcon	m_itmOscilloscope;
	CWndMenuBlockIcon	m_itmSpectrum;
	CWndMenuBlockIcon	m_itmLogic;
	CWndMenuBlockIcon	m_itmResponse;
	CWndMenuBlockIcon	m_itmGenerator;
	CWndMenuBlockIcon	m_itmSettings;
	CWndMenuBlockIcon	m_itmUser;
	CWndMenuBlockIcon	m_itmAbout;
	CWndMenuBlockIcon	m_itmDmm;

	virtual void Create(CWnd *pParent, ui16 dwFlags);
	virtual void OnPaint();
	virtual void OnKey(ui16 nKey);

private:
	const TMenuBlockStruct* GetLayout();
	int _GetItemId(CWnd* pWnd);
	CWnd* _GetWindowById(int nId);
};

#endif