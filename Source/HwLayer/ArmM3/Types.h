#ifndef __TYPES_H__
#define __TYPES_H__

//typedef const unsigned short uc16;
typedef unsigned char ui8;
//typedef unsigned char ui8;
typedef unsigned short u16;
typedef signed short si16;
typedef signed char si8;
typedef unsigned short ui16;
typedef unsigned long ui32;
typedef signed long si32;
typedef signed short si16;
typedef void* PVOID;
typedef bool BOOL;
typedef float FLOAT;
typedef int INT;
typedef const char * PCSTR;
typedef char * PSTR;
typedef char CHAR;

#define TRUE 1
#define FALSE 0

#define RGB565RGB(r, g, b) (((r)>>3)|(((g)>>2)<<5)|(((b)>>3)<<11))
#define GetHtmlR(rgb) ((rgb) >> 16)
#define GetHtmlG(rgb) (((rgb) >> 8)&0xff)
#define GetHtmlB(rgb) ((rgb)&0xff)
#define RGB565(rgb) RGB565RGB( GetHtmlR(0x##rgb), GetHtmlG(0x##rgb), GetHtmlB(0x##rgb))
#define COUNT(arr) (sizeof(arr)/sizeof(arr[0]))
#define RGBTRANS (RGB565(ff00ff)-1)

#define DecEnum(e) *((ui8*)&(e))-=1
#define IncEnum(e) *((ui8*)&(e))+=1

#ifndef min 
#	define min(a, b) ((a)<(b)?(a):(b))
#endif
#ifndef max
#	define max(a, b) ((a)>(b)?(a):(b))
#endif

#define NATIVEPTR ui32
#define NATIVEENUM unsigned char


#ifndef NULL
#	define NULL 0
#endif

extern void Assert(const char* msg, int n);
#ifdef _ASSERT
#	undef _ASSERT
#endif
#define _ASSERT(a) if(!(a)) { Assert(__FILE__, __LINE__); }

#define _ASSERT_VALID(a) if(!(a)) { Assert(__FILE__, __LINE__); }

#define ToWord(a, b) (((a)<<8)|(b))

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