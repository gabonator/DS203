/********************* (C) COPYRIGHT 2010 e-Design Co.,Ltd. ********************
 File Name : Interrupt.c  
 Version   : DS203_APP Ver 2.3x                                  Author : bure
*******************************************************************************/
#include "Interrupt.h"
#include "BIOS.h"

vu32 vu32Tick;
vu8  Cursor_Cnt, Key_Wait_Cnt, Key_Repeat_Cnt, Key_Buffer, Cnt_mS, Cnt_20mS;
vu8  Twink, Blink;
u8   Volume=20, Light;
vu16 Delay_Cnt, Beep_mS, Key_Status_Last, Sec_Cnt, PD_Cnt;
vu32 Wait_Cnt; 

//void BIOS__USART__HANDLER();
#include "Source\HwLayer\ArmM3\stm32f10x\inc\stm32f10x_usart.h"
#define USART1              ((USART_TypeDef *) USART1_BASE)

int usartch = -1;
/*void BIOS__USART__HANDLER()
{
}*/

void NMIException(void)
{}

void HardFaultException(void)
{
  while (1) {}
}

void MemManageException(void)
{
  while (1) {}
}

void BusFaultException(void)
{
  while (1) {}
}
void UsageFaultException(void)
{
  while (1) {}
}

void DebugMonitor(void)
{}

void SVCHandler(void)
{}

void PendSVC(void)
{}

void SysTickHandler(void)
{}

void WWDG_IRQHandler(void)
{}

void PVD_IRQHandler(void)
{}

void TAMPER_IRQHandler(void)
{}

void RTC_IRQHandler(void)
{}

void FLASH_IRQHandler(void)
{}

void RCC_IRQHandler(void)
{}

void EXTI0_IRQHandler(void)
{}

void EXTI1_IRQHandler(void)
{}

void EXTI2_IRQHandler(void)
{}

void EXTI3_IRQHandler(void)
{}

void EXTI4_IRQHandler(void)
{}

void DMA1_Channel1_IRQHandler(void)
{}

void DMA1_Channel2_IRQHandler(void)
{}

void DMA1_Channel3_IRQHandler(void)
{}

void DMA1_Channel4_IRQHandler(void)
{}

void DMA1_Channel5_IRQHandler(void)
{}

void DMA1_Channel6_IRQHandler(void)
{}

void DMA1_Channel7_IRQHandler(void)
{}

void ADC1_2_IRQHandler(void)
{}

void USB_HP_CAN_TX_IRQHandler(void)
{
  __CTR_HP();
}

void USB_LP_CAN_RX0_IRQHandler(void)
{
  __USB_Istr();
}

void CAN_RX1_IRQHandler(void)
{}

void CAN_SCE_IRQHandler(void)
{}

void EXTI9_5_IRQHandler(void)
{}

void TIM1_BRK_IRQHandler(void)
{}

void TIM1_UP_IRQHandler(void)
{}

void TIM1_TRG_COM_IRQHandler(void)
{}

void TIM1_CC_IRQHandler(void)
{}

void TIM2_IRQHandler(void)
{}

void TIM3_IRQHandler(void)
{ 
//  static char tmp[2] = {'A', 0};
  u8 KeyCode;
  vu32Tick++;
  __Set(KEY_IF_RST, 0);                      //Clear TIM3 interrupt flag
  if(Cnt_mS > 0)   Cnt_mS--;
  else {                                     //Read keys per 20mS
    Cnt_mS =20;
    if(Wait_Cnt >0)  Wait_Cnt--;
    if(Delay_Cnt >20) Delay_Cnt -= 20;
    else              Delay_Cnt  = 0;
    if(Beep_mS >=20)  Beep_mS   -= 20;
    else              __Set(BEEP_VOLUME, 0); // Beep off
    if(Cnt_20mS < 50) Cnt_20mS++;
    else {                                   // Do it pre sec.
      Cnt_20mS = 0;
      __Set(BETTERY_DT, 1);                  //Battery Detect
      Sec_Cnt++;
      if(PD_Cnt > 0) PD_Cnt--;
    }
    Cursor_Cnt++;
    if(Cursor_Cnt >= 12) {                   //12*20mS=240mS
//	  __Display_Str(0, 0, 0x8888, 0, tmp);
//	  if (++tmp[0] > 'Z')
//		tmp[0] = 'A';

      Cursor_Cnt=0;
      Twink=!Twink;
      Blink = 1;
    }
    if(Key_Wait_Cnt)    Key_Wait_Cnt--;
    if(Key_Repeat_Cnt)  Key_Repeat_Cnt--;
    KeyCode=0;//Read_Keys();
    if(KeyCode !=0) {
      Key_Buffer = KeyCode;
      //__Set(BEEP_VOLUME, 5*(Title[VOLUME][CLASS].Value-1));// Volume
      Beep_mS = 60; 
    }
  }
}

void TIM4_IRQHandler(void)
{}

void I2C1_EV_IRQHandler(void)
{}

void I2C1_ER_IRQHandler(void)
{}

void I2C2_EV_IRQHandler(void)
{}

void I2C2_ER_IRQHandler(void)
{}

void SPI1_IRQHandler(void)
{}

void SPI2_IRQHandler(void)
{}

void USART1_IRQHandler(void)
{
//  if ((USART1->SR & USART_IT_RXNE) != /*(u16)RESET*/ 0)
	{		
	__Set(BEEP_VOLUME, 50);
	Beep_mS = 200;
  	usartch = (u16)(USART1->DR & (u16)0x01FF);
//			usartch = USART_ReceiveData(USART1);
	}	

//	BIOS__USART__Handler();
}

void USART2_IRQHandler(void)
{}

void USART3_IRQHandler(void)
{}

void EXTI15_10_IRQHandler(void)
{}

void RTCAlarm_IRQHandler(void)
{}

void USBWakeUp_IRQHandler(void)
{}
/*******************************************************************************
* Function Name  : TIM8_BRK_IRQHandler
* Description    : This function handles TIM8 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_BRK_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : TIM8_UP_IRQHandler
* Description    : This function handles TIM8 overflow and update interrupt
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_UP_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : TIM8_TRG_COM_IRQHandler
* Description    : This function handles TIM8 Trigger and commutation interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_TRG_COM_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : TIM8_CC_IRQHandler
* Description    : This function handles TIM8 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_CC_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : ADC3_IRQHandler
* Description    : This function handles ADC3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC3_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : FSMC_IRQHandler
* Description    : This function handles FSMC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FSMC_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : SDIO_IRQHandler
* Description    : This function handles SDIO global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SDIO_IRQHandler(void)
{ 
  /* Process All SDIO Interrupt Sources */
//  SD_ProcessIRQSrc();
  
}

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : This function handles TIM5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : SPI3_IRQHandler
* Description    : This function handles SPI3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI3_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : UART4_IRQHandler
* Description    : This function handles UART4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART4_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : UART5_IRQHandler
* Description    : This function handles UART5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART5_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : TIM6_IRQHandler
* Description    : This function handles TIM6 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM6_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : This function handles TIM7 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM7_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : DMA2_Channel1_IRQHandler
* Description    : This function handles DMA2 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel1_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : DMA2_Channel2_IRQHandler
* Description    : This function handles DMA2 Channel 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel2_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : DMA2_Channel3_IRQHandler
* Description    : This function handles DMA2 Channel 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel3_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : DMA2_Channel4_5_IRQHandler
* Description    : This function handles DMA2 Channel 4 and DMA2 Channel 5
*                  interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel4_5_IRQHandler(void)
{}

/********************************* END OF FILE ********************************/
