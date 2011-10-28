#ifndef __MAINWND_H__
#define __MAINWND_H__

#define MainWnd (*CMainWnd::m_pInstance)

#include "Toolbar.h"
#include "Input.h"
#include "Others.h"
#include "Generator.h"

class CMainWnd : public CWnd
{
public:
	static CMainWnd	*m_pInstance;

public:
	CWndToolBar			m_wndToolBar;
	CWndOscGraph		m_wndGraph;
	CWndSigGraph		m_wndSignalGraph;
	CWndMenuInput		m_wndMenuInput;
	CWndMenuCursor		m_wndMenuCursor;
	CWndMenuMeas		m_wndMenuMeas;
	CWndMenuSettings	m_wndMenuSettings;
	CWndMenuDisplay		m_wndMenuDisplay;
	CWndMenuGenerator	m_wndMenuGenerator;
	CWndMenuMath		m_wndMenuMath;
	CWndMenuTools		m_wndMenuTools;
	CWndZoomBar			m_wndZoomBar;
	CWndLevelReferences	m_wndLReferences;
	CWndTimeReferences	m_wndTReferences;
	
	void Create();
	virtual void OnPaint();
	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);
};

#endif