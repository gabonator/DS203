#ifndef __TOOLBAR_H_
#define __TOOLBAR_H_

#include <Source/Framework/Wnd.h>
#include <Source/Core/Design.h>
#include <Source/Core/Shapes.h>

class CWndToolBar : public CWnd
{
public:
	ui8	m_nFocus;
	ui8	m_nOldFocus;
	enum {
		clrSelected = RGB565(ffffff),
		clrNormal = RGB565(b0b0b0),
		
		clrSelectedFocus = RGB565(000000),
		bgrSelectedFocus = RGB565(ffffff),
	};

	enum {
		_Oscilloscope = 0,
		_Input = 1,
		_Cursor = 2,
		_Meas = 3,
		_Math = 4,
		_Disp = 5,
		_Spectrum = 6,
		_FFT = 7,
		_Marker = 8,
		_Generator = 9,
		_Wave = 10,
		_Edit = 11,
		_Modulation = 12,
		_Settings = 13,
		_Main = 14,
		_Keys = 15,
		_About = 16,
		_Device = 17,
		_Firmware = 18,
		_UserApp = 19,
		_Tuner = 20,
		_Demo = 21,
		_Calib = 22,
		_Meter =23 //,
//		_Dmm =24,
//		_DmmMeas =25
	};

	class CBarItem
	{
	public:
		enum EType {
			IMain,
			ISub,
			IEnd
		};

		EType m_eType;                                   
		PCSTR m_pName;
		CWnd* m_pWndMenu; 
		ui8   m_mainMenuIndex;
	};

public:
	
	virtual void Create(CWnd *pParent) 
	{
		m_nFocus = 0;
		CWnd::Create("CToolbarWnd", WsVisible, CRect(0, 0, BIOS::LCD::LcdWidth, 16), pParent);
	}

	virtual void	OnKey(ui16 nKey);
	virtual void	OnPaint();
	virtual void	OnMessage(CWnd* pSender, ui16 code, ui32 data);
	CWnd*			GetCurrentLayout();
	virtual CWnd*	GetLastActiveWindow();
	virtual void	ChangeFocus(ui8 oldFocus);

private:
	CBarItem* GetMenuItems();
};
	
#endif