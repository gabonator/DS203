#ifndef __OTHERS_H__
#define __OTHERS_H__

class CWndMenuCursor : public CWnd
{
public:
	CWndMenuItem	m_itmX1;
	CWndMenuItem	m_itmX2;
	CWndMenuItem	m_itmY1;
	CWndMenuItem	m_itmY2;
	CWndMenuItem	m_itmMode;
	CWndMenuItem	m_itmDeltaX;
	CWndMenuItem	m_itmDeltaY;

	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndMenuCursor", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
		m_itmX1.Create("X1", RGB565(808080), 1, this);
		m_itmX2.Create("X2", RGB565(808080), 1, this);
		m_itmY1.Create("Y1", RGB565(808080), 1, this);
		m_itmY2.Create("Y2", RGB565(808080), 1, this);
		m_itmDeltaX.Create("dX", RGB565(ffffff), 2, this);
		m_itmDeltaY.Create("dY", RGB565(ffffff), 2, this);
		m_itmMode.Create("Mode\nManual", RGB565(808080), 1, this);
	}
};

class CWndMenuMeas : public CWnd
{
public:
	CWndMenuItem	m_itm1;
	CWndMenuItem	m_itm2;
	CWndMenuItem	m_itm3;
	CWndMenuItem	m_itm4;
	CWndMenuItem	m_itm5;
	CWndMenuItem	m_itm6;

	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndMenuMeas", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
		m_itm1.Create("RMS:\n5.323V", RGB565(ffff00), 2, this);
		m_itm2.Create("Vpp:\n12.000V", RGB565(ffff00), 2, this);
		m_itm3.Create("f:\n128.3 Hz", RGB565(ffff00), 2, this);
		m_itm4.Create("Lo:\n5.323V", RGB565(00ffff), 2, this);
		m_itm5.Create("Hi:\n12.000V", RGB565(00ffff), 2, this);
		m_itm6.Create("Avg:\n0.233V", RGB565(00ff00), 2, this);
	}
};

class CWndMenuSettings : public CWnd
{
public:
	CWndMenuItem	m_itmSave;
	CWndMenuItem	m_itmBacklight;
	CWndMenuItem	m_itmVolume;
	CWndMenuItem	m_itmLanguage;
	CWndMenuItem	m_itmAutoOff;
	CWndMenuItem	m_itmAbout;

	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndMenuSettings", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
		m_itmSave.Create("Save", RGB565(ffffff), 1, this);
		m_itmBacklight.Create("Backl.", RGB565(808080), 1, this);
		m_itmVolume.Create("Volume", RGB565(808080), 1, this);
		m_itmLanguage.Create("Language\nEnglish", RGB565(808080), 2, this);
		m_itmAutoOff.Create("Auto off\n5 min", RGB565(808080), 2, this);
		m_itmAbout.Create("About", RGB565(808080), 1, this);
	}
};

class CWndMenuDisplay : public CWnd
{
public:
	CWndMenuItem	m_itmMode;
	CWndMenuItem	m_itmDisplay;
	CWndMenuItem	m_itmDomain;
	CWndMenuItem	m_itmPersist;
	CWndMenuItem	m_itmAutoOff;
	CWndMenuItem	m_itmAbout;

	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndMenuDisplay", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
		m_itmMode.Create("Mode\nX-T", RGB565(808080), 2, this);
		m_itmDisplay.Create("Display\nPoints", RGB565(808080), 2, this);
		m_itmDomain.Create("Domain\nTime", RGB565(808080), 2, this);
		m_itmPersist.Create("Persist\nOff", RGB565(808080), 2, this);
	}
};

class CWndMenuMath : public CWnd
{
public:
	CWndMenuItem	m_itmChannel;
	CWndMenuItem	m_itmSource1;
	CWndMenuItem	m_itmSource2;
	CWndMenuItem	m_itmOperation;

	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndMenuMath", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
		m_itmChannel.Create("Off", RGB565(0000ff), 2, this);
		m_itmSource1.Create("Operand1\nChannel\nCh1", RGB565(ffff00), 3, this);
		m_itmSource2.Create("Operand2\nConstant\n4.0", RGB565(808080), 3, this);
		m_itmOperation.Create("Operator\nMultiply", RGB565(808080), 2, this);
	}
};

class CWndMenuTools : public CWnd
{
public:
	CWndMenuItem	m_itmBaudDet;
	CWndMenuItem	m_itmUartDecoder;
	CWndMenuItem	m_itmArea;

	virtual void Create(CWnd *pParent, ui16 dwFlags) 
	{
		CWnd::Create("CWndMenuTools", dwFlags, CRect(320-CWndMenuItem::MarginLeft, 20, 400, 240), pParent);
		m_itmBaudDet.Create("BaudDet", RGB565(808080), 1, this);
		m_itmUartDecoder.Create("Uart", RGB565(808080), 1, this);
		m_itmArea.Create("Area", RGB565(808080), 1, this);
	}
};

#endif