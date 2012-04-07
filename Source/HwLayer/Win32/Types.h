#ifndef __TYPES_H__
#define __TYPES_H__

#include <windows.h>
#include <crtdbg.h>

typedef unsigned char ui8;
typedef unsigned char ui8;
typedef unsigned short u16;
typedef signed short si16;
typedef signed char si8;
typedef unsigned short ui16;
typedef unsigned long ui32;
typedef signed long si32;
typedef signed short si16;

#define RGB565RGB(r, g, b) (((r)>>3)|(((g)>>2)<<5)|(((b)>>3)<<11))
#define Get565R(rgb) (((rgb)&0x1f)<<3)
#define Get565G(rgb) ((((rgb)>>5)&0x3f)<<2)
#define Get565B(rgb) ((((rgb)>>11)&0x1f)<<3)
#define GetHtmlR(rgb) ((rgb) >> 16)
#define GetHtmlG(rgb) (((rgb) >> 8)&0xff)
#define GetHtmlB(rgb) ((rgb)&0xff)
#define GetColorR(rgb) ((rgb) & 0xff)
#define GetColorG(rgb) (((rgb) >> 8)&0xff)
#define GetColorB(rgb) (((rgb) >> 16)&0xff)

#define RGB565(rgb) RGB565RGB( GetHtmlR(0x##rgb), GetHtmlG(0x##rgb), GetHtmlB(0x##rgb))
#define COUNT(arr) (sizeof(arr)/sizeof(arr[0]))
#define RGBTRANS (RGB565(ff00ff)-1)

#define DecEnum(e) *((ui8*)&(e))-=1
#define IncEnum(e) *((ui8*)&(e))+=1

#define NATIVEPTR ULONGLONG
#define NATIVEENUM LONG

#define ToWord(a, b) (((a)<<8)|(b))

#define _ASSERT_VALID(a) if(!(a)) { _ASSERT(#a); }

struct FILEINFO {
	enum {
		SectorSize = 512
	};
	ui16 pCluster[3];
	ui32 pDirAddr[1];
	ui8 nMode;
	ui8 nSectors;
};


#endif