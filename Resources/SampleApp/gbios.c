#include "gbios.h"
#include <string.h>
extern u8 _vectors[];

__attribute__ ((section(".identify")))
__attribute__((__used__))
static const char Identification[] = "DSO_APP: User application with gBios;";

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

#define NVIC_VectTab_FLASH ((ui32)0x08000000)

typedef void (*THandler)(void);
THandler gUSB_Istr = NULL;
THandler gCTR_HP = NULL;
typedef ui32 (*TGetProcAddress)(const char*);
TGetProcAddress gGetProcAddress = NULL;

void gBiosInit()
{
	char* strProc = strstr( GetSharedBuffer(), "biosproc=");
	if ( strProc )
		strProc += 11;

	gGetProcAddress = (TGetProcAddress)htoi(strProc);
	if (!gGetProcAddress )
		while(1) {}

	IMPORT(NVIC_SetVectorTable, NVIC_SetVectorTable, void, (ui32, ui32));
	IMPORT(USB_Init, __USB_Init, void, (void));
	IMPORT(USB_Istr, __USB_Istr, void, (void));
	IMPORT(CTR_HP, __CTR_HP, void, (void));
	gUSB_Istr = USB_Istr;
	gCTR_HP = CTR_HP;
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, (u32)&_vectors);
  USB_Init();
}

void gBiosExit()
{
	IMPORT(Execute, BIOS::SYS::Execute, void, (int));	
	Execute(0);
}

ui32 GetProcAddress(const char* strName)
{
	return gGetProcAddress( strName );
}



//
// interrupt.c
//

void NMIException(void) {}
void HardFaultException(void) { while (1) {} }
void MemManageException(void) { while (1) {} }
void BusFaultException(void) { while (1) {} }
void UsageFaultException(void) { while (1) {} }
void DebugMonitor(void) {}
void SVCHandler(void) {}
void PendSVC(void) {}
void SysTickHandler(void) {}
void WWDG_IRQHandler(void) {}
void PVD_IRQHandler(void) {}
void TAMPER_IRQHandler(void) {}
void RTC_IRQHandler(void) {}
void FLASH_IRQHandler(void) {}
void RCC_IRQHandler(void) {}
void EXTI0_IRQHandler(void) {}
void EXTI1_IRQHandler(void) {}
void EXTI2_IRQHandler(void) {}
void EXTI3_IRQHandler(void) {}
void EXTI4_IRQHandler(void) {}
void DMA1_Channel1_IRQHandler(void) {}
void DMA1_Channel2_IRQHandler(void) {}
void DMA1_Channel3_IRQHandler(void) {}
void DMA1_Channel4_IRQHandler(void) {}
void DMA1_Channel5_IRQHandler(void) {}
void DMA1_Channel6_IRQHandler(void) {}
void DMA1_Channel7_IRQHandler(void) {}
void ADC1_2_IRQHandler(void) {}
void USB_HP_CAN_TX_IRQHandler(void) { gCTR_HP(); }
void USB_LP_CAN_RX0_IRQHandler(void) { gUSB_Istr(); }
void CAN_RX1_IRQHandler(void) {}
void CAN_SCE_IRQHandler(void) {}
void EXTI9_5_IRQHandler(void) {}
void TIM1_BRK_IRQHandler(void) {}
void TIM1_UP_IRQHandler(void) {}
void TIM1_TRG_COM_IRQHandler(void) {}
void TIM1_CC_IRQHandler(void) {}
void TIM2_IRQHandler(void) {}
void TIM3_IRQHandler(void) 
{
	#define TIM3_SR     (*((volatile unsigned int *)(0x40000400+0x10)))
	TIM3_SR = 0; 
}
void TIM4_IRQHandler(void) {}
void I2C1_EV_IRQHandler(void) {}
void I2C1_ER_IRQHandler(void) {}
void I2C2_EV_IRQHandler(void) {}
void I2C2_ER_IRQHandler(void) {}
void SPI1_IRQHandler(void) {}
void SPI2_IRQHandler(void) {}
void USART1_IRQHandler(void) {}
void USART2_IRQHandler(void) {}
void USART3_IRQHandler(void) {}
void EXTI15_10_IRQHandler(void) {}
void RTCAlarm_IRQHandler(void) {}
void USBWakeUp_IRQHandler(void) {}
void TIM8_BRK_IRQHandler(void) {}
void TIM8_UP_IRQHandler(void) {}
void TIM8_TRG_COM_IRQHandler(void) {}
void TIM8_CC_IRQHandler(void) {}
void ADC3_IRQHandler(void) {}
void FSMC_IRQHandler(void) {}
void SDIO_IRQHandler(void) {}
void TIM5_IRQHandler(void) {}
void SPI3_IRQHandler(void) {}
void UART4_IRQHandler(void) {}
void UART5_IRQHandler(void) {}
void TIM6_IRQHandler(void) {}
void TIM7_IRQHandler(void) {}
void DMA2_Channel1_IRQHandler(void) {}
void DMA2_Channel2_IRQHandler(void) {}
void DMA2_Channel3_IRQHandler(void) {}
void DMA2_Channel4_5_IRQHandler(void) {}

//
// startup.c
//

typedef void( *const intfunc )( void );

#define WEAK __attribute__ ((weak))

extern unsigned long _etext;
extern unsigned long _sidata;
extern unsigned long _sdata;
extern unsigned long _edata;
extern unsigned long _sbss;
extern unsigned long _ebss;
extern unsigned long _estack;

void Reset_Handler(void) __attribute__((__interrupt__));
void __Init_Data(void);
void Default_Handler(void);

extern int main(void);
extern void __libc_init_array(void);

__attribute__ ((section(".isr_vectors")))
void (* const g_pfnVectors[])(void) = {
    (intfunc)((unsigned long)&_estack), /* The stack pointer after relocation */
Reset_Handler, NMIException, HardFaultException,
MemManageException, BusFaultException, UsageFaultException, 0, 0,
0, 0, SVCHandler, DebugMonitor, 0, PendSVC, SysTickHandler,
WWDG_IRQHandler, PVD_IRQHandler, TAMPER_IRQHandler, RTC_IRQHandler,
FLASH_IRQHandler, RCC_IRQHandler, EXTI0_IRQHandler,
EXTI1_IRQHandler, EXTI2_IRQHandler, EXTI3_IRQHandler,
EXTI4_IRQHandler, DMA1_Channel1_IRQHandler,
DMA1_Channel2_IRQHandler, DMA1_Channel3_IRQHandler,
DMA1_Channel4_IRQHandler, DMA1_Channel5_IRQHandler,
DMA1_Channel6_IRQHandler, DMA1_Channel7_IRQHandler,
ADC1_2_IRQHandler, USB_HP_CAN_TX_IRQHandler,
USB_LP_CAN_RX0_IRQHandler, CAN_RX1_IRQHandler, CAN_SCE_IRQHandler,
EXTI9_5_IRQHandler, TIM1_BRK_IRQHandler, TIM1_UP_IRQHandler,
TIM1_TRG_COM_IRQHandler, TIM1_CC_IRQHandler, TIM2_IRQHandler,
TIM3_IRQHandler, TIM4_IRQHandler, I2C1_EV_IRQHandler,
I2C1_ER_IRQHandler, I2C2_EV_IRQHandler, I2C2_ER_IRQHandler,
SPI1_IRQHandler, SPI2_IRQHandler, USART1_IRQHandler,
USART2_IRQHandler, USART3_IRQHandler, EXTI15_10_IRQHandler,
RTCAlarm_IRQHandler, USBWakeUp_IRQHandler, TIM8_BRK_IRQHandler,
TIM8_UP_IRQHandler, TIM8_TRG_COM_IRQHandler, TIM8_CC_IRQHandler,
ADC3_IRQHandler, FSMC_IRQHandler, SDIO_IRQHandler, TIM5_IRQHandler,
SPI3_IRQHandler, UART4_IRQHandler, UART5_IRQHandler,
TIM6_IRQHandler, TIM7_IRQHandler, DMA2_Channel1_IRQHandler,
DMA2_Channel2_IRQHandler, DMA2_Channel3_IRQHandler,
DMA2_Channel4_5_IRQHandler
};

void __Init_Data(void) {
	unsigned long *src, *dst;
	/* copy the data segment into ram */
	src = &_sidata;
	dst = &_sdata;
	if (src != dst)
		while(dst < &_edata)
			*(dst++) = *(src++);

	/* zero the bss segment */
	dst = &_sbss;
	while(dst < &_ebss)
		*(dst++) = 0;
}

void Reset_Handler(void) {
	/* Initialize data and bss */
	__Init_Data();
	main();
	while(1) {}
}

void Default_Handler(void) {
	while (1) {}
}

void _init(void)
{
}
