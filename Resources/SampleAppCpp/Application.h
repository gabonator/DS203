//#pragma once
#ifndef __APPLICATION__
#define __APPLICATION__

#ifdef _WIN32
#include <windows.h>
#endif

bool HasOverlay();

class CApplication 
{
public:
	CApplication();
	~CApplication();
	void Create();
	void Destroy();
	bool operator ()();
};

#endif