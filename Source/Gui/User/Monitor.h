#define ENABLE_MONITOR

#if defined(ADD_MODULE) && defined(ENABLE_MONITOR)
	ADD_MODULE( "Monitor", CWndUserMonitor )
#endif

#ifndef __MONITOR_H__
#define __MONITOR_H__

#include <Source/Framework/Wnd.h>
#include <Source/Core/Utils.h>
#include <Source/Gui/Oscilloscope/Disp/ItemDisp.h>
#include <Source/Gui/Oscilloscope/Mask/MenuMask.h>

class CWndUserMonitor : public CWnd
{
	CProviderEnum	m_proBaud;
	CMPItem			m_itmBaud;
	CWndMenuItem	m_itmFormat;
	CMIButton		m_btnClear;
//	CMIButton		m_btnSend;

	int m_x;
	int m_y;	
	int m_nBaud;

	static const char* const m_ppszTextBaud[];
	static const int m_pnValueBaud[];

public:
	virtual void Create(CWnd *pParent, ui16 dwFlags);
	virtual void OnPaint();
	virtual void OnTimer();
	virtual void OnTick();
	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);
	virtual void OnKey(ui16 nKey);
};

#endif