#ifndef __TOOLBOX_H__
#define __TOOLBOX_H__

#include <Source/Framework/Wnd.h>
#include <Source/Framework/Thread.h>
#include "Export.h"

class CWndToolbox : public CWnd, public CThread, public CExport
{
public:
	enum {
		Width = 200,
		Height = 88
	};

	enum {
		MenuPauseResume = 1,
		MenuManager = 0,
		MenuReset = 2,
		MenuSave = 3,
		MenuMax = MenuSave
	};

public:
	bool			m_bRunning;
	bool			m_bFirst;
	bool			m_bAdcEnabled;
	int				m_nFocus;
	
	CWndToolbox();

	void			Create( CWnd* pParent );
	virtual void	OnPaint();
	void			PrintBold( int x, int y, ui16 clrFront, ui16 clrBorder, PCSTR szLabel );
	virtual BOOL	IsRunning();
	virtual void	OnKey(ui16 nKey);
	virtual int		GetResult();
	void			DoModal();
	virtual void	ToggleAdc();
	virtual void	UpdateAdc();
};

#endif
