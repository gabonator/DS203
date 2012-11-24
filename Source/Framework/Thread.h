#ifndef __THREAD_H__
#define __THREAD_H__

#include <Source/Main/Application.h>

class CThread 
{
public:
	virtual void Start() 
	{ 
		_ASSERT(0);
	}
	virtual void Stop() 
	{ 
		_ASSERT(0);
	}
	virtual void Run() 
	{ 
		_ASSERT(0);
	}
	virtual BOOL IsRunning() 
	{ 
		_ASSERT(0);
		return FALSE;
	}
	virtual int GetResult()
	{
		_ASSERT(0);
		return 0;
	}
	void Sleep(int nTime)
	{
		ui32 nLastTime = BIOS::SYS::GetTick() + nTime;
		do 
		{
			Application.operator()();
		} while ( BIOS::SYS::GetTick() < nLastTime);

	}
};

#endif