#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <Source/Framework/Wnd.h>
#include <Source/Core/ListItems.h>

class CWndMenuGenerator : public CWnd
{
public:
	CItemProvider	m_itmWave;
	CItemProvider	m_itmPsc;
	CItemProvider	m_itmArr;
	CItemProvider	m_itmFreq;
	CItemProvider	m_itmBpm;
	//CWndMenuItem	m_itmFrequency;
	//CWndMenuItem	m_itmTransfer;
	//CWndMenuItem	m_itmFile;
	//CWndMenuItem	m_itmSingle;

	CProviderEnum	m_proWave;
	CProviderNum	m_proPsc;
	CProviderNum	m_proArr;
	CProviderNum	m_proFreq;
	CProviderNum	m_proBpm;

	si16			m_nFreq;
	si16			m_nBpm;

	virtual void Create(CWnd *pParent, ui16 dwFlags);
	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);
};

#endif