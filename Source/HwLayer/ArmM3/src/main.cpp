extern "C" {
#include "interrupt.h"
#include "BIOS.h"
}

#include <Source/HwLayer/Types.h>
#include <Source/HwLayer/Bios.h>
#include <Source/Main/Application.h>

extern u8 _vectors[];
//extern int __errno; // required by math.h

struct TIdentification {
	const char strProgName[65];
	u32 dwCode;	
	u32 dwProcAddr;
};

__attribute__ ((section(".identify")))
__attribute__((__used__))
static TIdentification Identification = {
	"DSO_APP: Gabonator's alternative UI;Alter UI;Gabriel Valky 2013;",
	ToDword('B', 'I', 'O', 'S'),
	(u32)(void*)&BIOS::SYS::GetProcAddress
};

//static const char strIdentification[] = "DSO_APP: Gabonator's alternative UI;Alter UI;Gabriel Valky 2013;";

int main(void)
{ 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, (u32)&_vectors);
	UsartInit();
	__USB_Init();

	CApplication app;
	app.Create();

	while ( app() );

	return 0;
}
