#ifndef __TUNER_H__
#define __TUNER_H__

#include <Source/Core/Settings.h>
#include <Source/Core/Utils.h>
#include <Source/Framework/Wnd.h>
#include <Source/Core/Bitmap.h>
#include <Source/Gui/Spectrum/Core/FFT.h>
#include <Source/Gui/Oscilloscope/Core/CoreOscilloscope.h>
#include <Source/Gui/Oscilloscope/Controls/GraphBase.h>
// not very nice placing complex library into .h, put depedent code to .cpp file!
#include <math.h>


class CWndTuner : public CWnd
{
	static const unsigned char bitmapTuner[];
	bool m_bWave;

public:
	virtual void Create(CWnd *pParent, ui16 dwFlags);
	virtual void OnPaint();
	virtual void OnMessage(CWnd* pSender, ui16 code, ui32 data);

protected:
	float GetFundamental();
	void DrawPiano();
	void DrawKey(int, bool);
	void DrawScale();
	void DrawCents(int nCents, bool);
};



#endif