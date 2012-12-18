#ifndef __COREOSCILLOSCOPE_H__
#define __COREOSCILLOSCOPE_H__

#include <Source/HwLayer/Types.h>
#include <Source/Framework/Classes.h>

class CCoreOscilloscope
{
public:
	static void ConfigureAdc();
	static void ConfigureTrigger();
	static void UpdateConstants();
	// mask
	static void GetMaskAt(int nX, ui8** pLow, ui8** pHigh);
	static void GetMaskStats(int** nPass, int** nFail);
};

#endif