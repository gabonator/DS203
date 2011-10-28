//#pragma once
#ifdef _WIN32
#include <windows.h>
#endif
//#include <string.h>


class CApplication {
public:
	CApplication();
	~CApplication();
	void operator ()();
};
