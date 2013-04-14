#pragma once

#include <Source/HwLayer/Types.h>
#include <Source/Framework/Application.h>

bool HasOverlay();

class CApplication : public CApplicationProto
{
public:
	CApplication();
	virtual ~CApplication();
	virtual void Create();
	virtual void Destroy();
	virtual bool operator ()();
};

