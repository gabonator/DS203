#ifndef __MAINWND_H__
#define __MAINWND_H__

#define MainWnd (*CMainWnd::m_pInstance)

#include "Toolbar.h"
#include "Oscilloscope/Oscilloscope.h"
#include "Windows/WndConfirm.h"
#include "Windows/MessageBox.h"
#include "ToolBox/ToolBox.h"
#include "ToolBox/Manager.h"
#include "Generator/Generator.h"
#include "MainMenu.h"
#include "User/Screensaver.h"
#include "User/Snake.h"
#include "User/Balls.h"
#include "User/Tuner.h"
#include "About/About.h"
#include "Settings/Settings.h"
#include "Calibration/Calibration.h"
#include "User/Meter.h"
#include "Spectrum/Spectrum.h"

class CMainWnd : public CWnd
{
public:
	static CMainWnd	*m_pInstance;

public:
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
	CWndMenuDisplay		m_wndMenuDisplay;
	CWndMenuGenerator	m_wndMenuGenerator;
	CWndMenuGeneratorMod	m_wndMenuGeneratorMod;
	CWndMenuGeneratorEdit	m_wndMenuGeneratorEdit;
	CWndMenuMath		m_wndMenuMath;
	CWndZoomBar			m_wndZoomBar;
	CWndOscInfo			m_wndInfoBar;
	CWndLevelReferences	m_wndLReferences;
	CWndTimeReferences	m_wndTReferences;
	CWndScreenSaver		m_wndScreenSaver;
	CWndSnake			m_wndUserGame;
//	CWndBalls			m_wndUserBalls;
	CWndTuner			m_wndUserTuner;
	CWndAbout			m_wndAbout;
	//CWndCalibration		m_wndUserCalibration;
	CWndMenuCalibration	m_wndUserCalibration;
	CWndUserMeter		m_wndUserMeter;

	CWndMenuSpectMain	m_wndSpectrumMain;
	CWndMenuSpectMarker	m_wndSpectrumMarker;
	CWndSpecAnnotations m_wndSpectrumAnnot;

	CWndModuleSelector	m_wndModuleSel;
	CWndMessageBox		m_wndMessage;	
	CWndConfirm			m_wndConfirm;

	CWndToolbox			m_wndToolbox;
	CWndManager			m_wndManager;

	void Create();
	virtual void OnPaint();
	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);
	void OnTimer();
	virtual void WindowMessage(int nMsg, int nParam =0);

private:
	// auto trigger
	long				m_lLastAcquired;

private:
	void SdkProc();
};

#endif