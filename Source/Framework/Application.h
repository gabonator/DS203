//#pragma once
#ifndef __APPLICATIONBASE__
#define __APPLICATIONBASE__

#include <Source/HwLayer/Types.h>

class CApplicationProto 
{
public:
	//CApplicationProto();
	//virtual ~CApplicationProto();
	virtual void Create() = 0;
	virtual void Destroy() = 0;
	virtual bool operator ()() = 0;
};
/*
class CApplication
{
public:
	CApplication();
	~CApplication();
	void Create();
	void Destroy();
	bool operator ()();
};
*/
#endif