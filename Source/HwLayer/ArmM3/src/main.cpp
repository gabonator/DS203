extern "C" {
#include "interrupt.h"
#include "BIOS.h"
}

#include <Source/Main/Application.h>

extern u8 _vectors[];
void Run();

int main(void)
{ 
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, (u32)&_vectors);
    __USB_Init();

	Run();

//	CApplication app;
//	while (1)
//		app();
	return 0;
}
