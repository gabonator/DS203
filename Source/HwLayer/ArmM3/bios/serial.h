// inspired by https://github.com/neilstockbridge/dsoquad-BIOS/blob/master/src/serial.c

#include "Source/HwLayer/ArmM3/stm32f10x/inc/stm32f10x_lib.h"
#include "Source/HwLayer/ArmM3/stm32f10x/inc/stm32f10x_usart.h"

// Note: CPU_CLK / BAUD_RATE must fit within 16-bits
#define CPU_CLK 72000000
#define BAUD_RATE 115200

// Values for the 2-bit pairs for MODE found in the CRL/CRH register:
#define GPIO_MODE_INPUT 0x0
#define GPIO_MODE_OUTPUT_MAX_2MHZ 0x2

#define GPIO_CNF_AF_PUSH_PULL 0x2
#define GPIO_CNF_PULL_UP_OR_DOWN 0x2 // Set the bit corresponding to the port in the DR to 1 to choose UP rather than DOWN

#define PIN9_MODE_POS 4 // bit position 4 is bit0 of the MODE9 setting
#define PIN9_CNF_POS 6
#define PIN10_MODE_POS 8
#define PIN10_CNF_POS 10

#define USART1              ((USART_TypeDef *) USART1_BASE)

extern "C" {
	int UsartGet();
	int UsartEmpty();
}

/*static*/ void BIOS::SERIAL::Configure(int nBaudrate)
{
  USART1->BRR = CPU_CLK / nBaudrate;
}

/*static*/ void BIOS::SERIAL::Init()
{
  // Enable USART1:
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
  // Set the baud rate:
  USART1->BRR = CPU_CLK / BAUD_RATE;
  // Enable the USART for both transmission and reception:
  USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;

	// enable rx interrupt	
	USART1->SR &= ~USART_IT_RXNE;

  // I guess these might mean enable pull up for RX and set TX as hard-driven output
  // AFOUT_10 probably mean b10, i.e. function 2, which is AF output, push-pull
  //gpio_usart1_tx_mode(GPIO_AFOUT_10);
  //gpio_usart1_rx_mode(GPIO_HIGHZ_INPUT);

  // TX is pin 9 on port A (PA 9)
  // RX is pin 10 on port A (PA 10)
  // Mask out any current values for the MODE and CNF values for pins 9 and 10
  GPIOA->CRH = (GPIOA->CRH & ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9 | GPIO_CRH_MODE10 | GPIO_CRH_CNF10)) |
              (GPIO_MODE_OUTPUT_MAX_2MHZ << PIN9_MODE_POS) |
              (GPIO_CNF_AF_PUSH_PULL << PIN9_CNF_POS) |
              (GPIO_MODE_INPUT << PIN10_MODE_POS) |
              (GPIO_CNF_PULL_UP_OR_DOWN << PIN10_CNF_POS)
              ;
  // Enable the pull-up for PA 10 (RX):
  GPIOA->BSRR = GPIO_BSRR_BS10;
  Send("DS203 Boot...\n");
}

/*static*/ void BIOS::SERIAL::Send(const char* strBuf)
{
  char ch;
  while ( (ch = *strBuf++) != 0 )
  {
	  // Wait until DR is ready for the next frame:
		while ( !(USART1->SR & USART_SR_TXE) );
		USART1->DR = ch;
  }
}
/*static*/ int BIOS::SERIAL::Getch()
{
  return UsartGet();
}

/*static*/ void BIOS::SERIAL::Putch(char ch)
{
	while ( !(USART1->SR & USART_SR_TXE) );
	USART1->DR = ch;
}
