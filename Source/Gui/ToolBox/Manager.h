#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <Source\Core\Controls.h>
#include <Source\Framework\Thread.h>
#include "Export.h"
#include "Import.h"
#include "LineTabs.h"


class CWndManager : public CListBox, public CThread, public CExport, public CImport
{
public:
	static const char* const tabs[];
	static const char* const strTemplateDisplay[];	
	static const char* const strTemplateFile[];

	enum {
		Width = 200,
		Height = 120
	};

public:
	bool			m_bRunning;
	int				m_nValue;
	char			m_strCurrent[20];

	CProviderNumAny<int>	m_proValue;
	CLPItem			m_itmValue;
	CLStaticItem	m_itmFile;
	CLStaticItem	m_itmSpace1;
	CLStaticItem	m_itmSpace2;
	CProviderBtn	m_proLoad;
	CLPItem			m_itmLoad;
	CProviderBtn	m_proSave;
	CLPItem			m_itmSave;
	CLTabs			m_itmTabs;
	
	CWndManager();

	void			Create( CWnd* pParent );
	virtual BOOL	IsRunning();
	virtual void	OnMessage(CWnd* pSender, ui16 code, ui32 data);
	virtual int		GetResult();
	void			DoModal();
	void			Cancel();

private:
	bool			Exists(char *strName);
};

#endif