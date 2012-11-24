#ifndef __CORESETINGS_H__
#define __CORESETTINGS_H__

#include <Source/HwLayer/Types.h>
#include <Source/Framework/Classes.h>

class CCoreSettings
{
public:
	static void Update()
	{
		BIOS::SYS::SetBacklight( Settings.Runtime.m_nBacklight );
		BIOS::SYS::SetVolume( Settings.Runtime.m_nVolume );
	}
};

#endif