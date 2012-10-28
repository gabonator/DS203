#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <Source\Core\Controls.h>
#include <Source\Framework\Thread.h>
#include "Export.h"

class CWndManager : public CListBox, public CThread
{
	enum {
		Width = 200,
		Height = 120
	};

public:
	bool			m_bRunning;
	int				m_nValue;
	char			m_strCurrent[16];

	CProviderNumAny<int>	m_proValue;
	CLPItem			m_itmValue;
	CLStaticItem	m_itmFile;
	CLStaticItem	m_itmSpace1;
	CLStaticItem	m_itmSpace2;
	CProviderBtn	m_proLoad;
	CLPItem			m_itmLoad;
	CProviderBtn	m_proSave;
	CLPItem			m_itmSave;


	CWndManager();

	void			Create( CWnd* pParent );
	virtual BOOL	IsRunning();
	virtual void	OnMessage(CWnd* pSender, ui16 code, ui32 data);

	virtual int		GetResult();
	void			DoModal();

private:
	bool			Exists(char *strName);
};

#endif