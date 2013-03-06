typedef unsigned char u8;
typedef unsigned int ui32;
typedef unsigned int u32;
typedef unsigned short ui16;
typedef unsigned short uc16;

#define IMPORT(loc, f, ret, args) \
	typedef ret (*T ## loc)args; \
	T ## loc loc = ( T## loc) GetProcAddress( #f );

void gBiosInit();
void gBiosExit();
ui32 GetProcAddress(const char* strName);
