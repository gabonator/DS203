#include <Source/HwLayer/Types.h>
#include <Source/HwLayer/Bios.h>
//#include <string.h> // manual strstr function reduces size by 1760 bytes

void *__dso_handle = 0;
char *strstr ( const char * str1, const char * str2)
{        
	char *cp = (char *) str1;
	char *s1, *s2;
	if ( !*str2 )
		return((char *)str1);
	while (*cp)
	{
		s1 = cp;
		s2 = (char *) str2;
		while ( *s1 && *s2 && !(*s1-*s2) )
			s1++, s2++;                
		if (!*s2)
			return(cp);
		cp++;
	}
	return(NULL);
}

//// {{{
void Assert(const char*msg, int n)
{
	while (1);
}
//// }}}

__attribute__ ((section(".identify")))
__attribute__((__used__))
static const char Identification[] = "DSO_APP: User application with gBios++;";

char* GetSharedBuffer()
{
	return (char*)0x20000800;
}

int gethex(char c)
{
	if ( c >= '0' && c <= '9' )
		return c-'0';
	if ( c >= 'a' && c <= 'f' )
		return c-'a'+10;
	if ( c >= 'A' && c <= 'F' )
		return c-'A'+10;
	return -1;
}

ui32 htoi(char* a)
{
	int nDigit;
	ui32 nAux = 0;
	while ( (nDigit=gethex(*a++)) != -1 )
	{
		nAux <<= 4;
		nAux |= nDigit;
	}	
	return nAux;
}

typedef ui32 (*TGetProcAddress)(const char*);
TGetProcAddress gGetProcAddress = NULL;

extern "C" unsigned long _estack;

ui32 BIOS::SYS::GetProcAddress( const char* strFunction )
{
	return gGetProcAddress( strFunction );
}

void InterfaceReslove();

#define IMPORT(loc, f, ret, args) \
	typedef ret (*T ## loc)args; \
	T ## loc loc = ( T## loc) BIOS::SYS::GetProcAddress( #f );

class CGBios
{
public:
	CGBios()
	{
		char* strProc = strstr( GetSharedBuffer(), "biosproc=");
		if ( strProc )
			strProc += 11;

	  gGetProcAddress = (TGetProcAddress)_estack;
		gGetProcAddress = (TGetProcAddress)htoi(strProc);
		if (!gGetProcAddress )
			while(1) {}
		InterfaceReslove();
	}
	~CGBios()
	{
		IMPORT(Execute, BIOS::SYS::Execute, void, (int));	
		Execute(0);
	}
};

CGBios /*__attribute__((init_priority(101)))*/ GBios;

#undef IMPORT
#define IMPORT( ret, name, args, got ) \
	ui32 _GOT##got = NULL;	\
	ret name args	\
	{		\
		asm ("push {r0}");	\
		asm ("ldr r0, =" "_GOT" #got);	\
		asm ("ldr r0, [r0]");	\
		asm ("mov ip, r0");	\
		asm ("pop {r0}");	\
		asm ("bx ip");	\
	}

// PLT stubs
#include "Imports.h"

#undef IMPORT
#define IMPORT( ret, name, args, got ) \
	_GOT##got = (ui32)BIOS::SYS::GetProcAddress( #name );

// PLT resolver
void InterfaceReslove()
{
	#include "Imports.h"
}

#undef IMPORT

