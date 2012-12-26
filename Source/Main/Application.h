//#pragma once
#ifndef __APPLICATION__
#define __APPLICATION__

#ifdef _WIN32
#include <windows.h>
#endif

#define Application (*CApplication::getInstance())

class CApplication 
{
	static CApplication* m_pInstance;
 
public:
	static CApplication* getInstance();

	CApplication();
	~CApplication();
	void Create();
	bool operator ()();
};

#endif