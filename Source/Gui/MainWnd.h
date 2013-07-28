#ifndef __MAINWND_H__
#define __MAINWND_H__

#define MainWnd (*CMainWnd::m_pInstance)

#include "Toolbar.h"
#include "Oscilloscope/Core/CoreOscilloscope.h"
#include "Oscilloscope/Oscilloscope.h"
#include "Windows/WndConfirm.h"
#include "Windows/MessageBox.h"
#include "ToolBox/ToolBox.h"
#include "ToolBox/Manager.h"
#include "Generator/Generator.h"
#include "MainMenu.h"
#include "About/About.h"
#include "Settings/Settings.h"
#include "Settings/KeySettings.h"
#include "Calibration/Calibration.h"
#include "Spectrum/Spectrum.h"
#include "Mouse.h"

bool HasOverlay();

#define ADD_MODULE( strName, type )
#include <Source/User/_Modules.h>
#undef ADD_MODULE

class CMainWnd : public CWnd
{
public:
	static CMainWnd	*m_pInstance;

public:
	CMouse					m_Mouse;
	CWndToolBar			m_wndToolBar;
	CWndOscGraph		m_wndGraph;
	CWndSigGraph		m_wndSignalGraph;

	CWndSpectrumGraph	m_wndSpectrumGraph;
	CWndSpectrumGraphMini	m_wndSpectrumMiniFD;
	CWndSpectrumGraphTime	m_wndSpectrumMiniTD;
	CWndSpectrumGraphSpec	m_wndSpectrumMiniSG;

	CWndMenuInput		m_wndMenuInput;
	CWndMenuCursor		m_wndMenuCursor;
	CWndMenuMeas		m_wndMenuMeas;
	CWndMenuSettings	m_wndMenuSettings;
	CWndMenuKeySettings	m_wndMenuKeySettings;
	CWndMenuDisplay		m_wndMenuDisplay;
	CWndMenuMask		m_wndMenuMask;
	CWndMenuGenerator	m_wndMenuGenerator;
//	CWndMenuGeneratorMod	m_wndMenuGeneratorMod;
	CWndMenuGeneratorEdit	m_wndMenuGeneratorEdit;
	CWndMenuMath		m_wndMenuMath;
	CWndZoomBar			m_wndZoomBar;
	CWndOscInfo			m_wndInfoBar;
	CWndLevelReferences	m_wndLReferences;
	CWndLevelReferencesMath	m_wndLReferencesM;
	CWndTimeReferences	m_wndTReferences;
	CWndAboutFirmware	m_wndAboutFirmware;
	CWndAboutDevice		m_wndAboutDevice;
	CWndAboutStatus		m_wndAboutStatus;
	CWndAboutModules	m_wndAboutModules;
	CWndMenuCalibration	m_wndCalibration;

	CWndMenuSpectMain	m_wndSpectrumMain;
	CWndMenuSpectMarker	m_wndSpectrumMarker;
	CWndSpecAnnotations m_wndSpectrumAnnot;

	CWndModuleSelector	m_wndModuleSel;
	CWndMessageBox		m_wndMessage;	
	CWndConfirm			m_wndConfirm;

	CWndToolbox			m_wndToolbox;
	CWndManager			m_wndManager;

#define ADD_MODULE( strName, type ) type m_wndUser##type;
#include <Source/User/_Modules.h>
#undef ADD_MODULE

	void				Create();
	virtual void		OnPaint();
	virtual void		OnMessage(CWnd* pSender, ui16 code, ui32 data);
	virtual void		OnTimer();
	virtual void		WindowMessage(int nMsg, int nParam =0);
	bool				HasOverlay();
	void				OnMouseClick();
	void				Resample();

private:
	// auto trigger
	long				m_lLastAcquired;
	int					m_nLastKey;
	bool				m_bSleeping;

private:
	void				SdkDiskProc();
	void				SdkUartProc();
	void				CallShortcut(int nShortcut);
};

#endif