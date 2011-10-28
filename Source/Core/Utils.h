#ifndef __UTILS_H__
#define __UTILS_H__

#include <Source/HwLayer/Types.h>

class CUtils
{
public:
	static char* itoa(si16 n);
	static char tohex(ui8 n);
	static char* clrhex(ui16 c);
};

#endif