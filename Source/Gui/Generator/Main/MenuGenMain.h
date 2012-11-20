#ifndef __GENERATOR_H__
#define __GENERATOR_H__

//#include <Source/HwLayer/bios.h>
#include <Source/Gui/Generator/Core/CoreGenerator.h>
#include <Source/Core/Settings.h>
#include <Source/Framework/Wnd.h>
#include <Source/Core/ListItems.h>

#include "ItemGenWaveform.h"
#include "ItemGenFrequency.h"
#include "ItemGenAmplitude.h"
#include "ItemGenOffset.h"

class CWndMenuGenerator : public CCoreGenerator, public CWnd
{
public:
	CItemGenWaveform	m_itmWave;
	CItemFrequency	m_itmFreq;
	CItemAmplitude	m_itmAmpl;
	CItemOffset		m_itmOffset;

	virtual void Create(CWnd *pParent, ui16 dwFlags);
	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);
};

#endif