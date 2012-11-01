#ifndef __CWNDOSCGRAPH_H__
#define __CWNDOSCGRAPH_H__

#include "GraphBase.h"
#include <Source/Core/Settings.h>
#include <Source/Gui/Oscilloscope/Math/ChannelMath.h>

class CWndOscGraph : public CWndGraph, public CMathChannel
{
	void			_PrepareColumn( ui16 *column, ui16 n, ui16 clr );

public:
	virtual void	Create(CWnd *pParent, ui16 dwFlags);
	virtual void	OnMessage(CWnd* pSender, ui16 code, ui32 data);
	virtual void	OnPaint();
	void			SetupMarkers( CSettings::Calibrator::FastCalc& Ch1fast, CSettings::Calibrator::FastCalc& Ch2fast,
		int& nMarkerT1, int& nMarkerT2, int& nMarkerY1, int& nMarkerY2 );
	void			SetupSelection( bool& bSelection, int& nMarkerT1, int& nMarkerT2 );
	void			GetCurrentRange(int& nBegin, int& nEnd);
	void			UpdateResolutions();
};

#endif