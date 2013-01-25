#define PERIPH_BASE           ((u32)0x40000000)
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
#define GPIOA_BASE            (APB2PERIPH_BASE + 0x0800)
#define GPIOB_BASE            (APB2PERIPH_BASE + 0x0C00)
#define GPIOC_BASE            (APB2PERIPH_BASE + 0x1000)
#define GPIOD_BASE            (APB2PERIPH_BASE + 0x1400)
#define GPIOE_BASE            (APB2PERIPH_BASE + 0x1800)
#define GPIOF_BASE            (APB2PERIPH_BASE + 0x1C00)
#define GPIOG_BASE            (APB2PERIPH_BASE + 0x2000)

/*
typedef struct
{
  vu32 CRL;
  vu32 CRH;
  vu32 IDR;
  vu32 ODR;
  vu32 BSRR;
  vu32 BRR;
  vu32 LCKR;
} GPIO_TypeDef;
*/

const static ui32 arrGpioBase[] = {GPIOA_BASE, GPIOB_BASE, GPIOC_BASE, GPIOD_BASE, GPIOE_BASE, GPIOF_BASE, GPIOG_BASE};

/*static*/ ui32* BIOS::GPIO::GetRegister(int nPort, int nReg)
{
	ui32 dwAddr = arrGpioBase[nPort];

	dwAddr |= nReg*4; 
	return (ui32*)dwAddr;
}

/*static*/ void BIOS::GPIO::SetState(int nPort, int nPin, int nState)
{
  _ASSERT( nState >= 0 && nState <= 15 );
	_ASSERT( nPin >= 0 && nPin <= 15 );
	
	ui32 dwAddr;

	if ( nPin < 8 )
		dwAddr = arrGpioBase[nPort] + 0;	// CRL
	else
	{
		dwAddr = arrGpioBase[nPort] + 4;	// CRH
		nPin &= 7;
	}

	ui32 dwMask = ~(0xf << (nPin*4));
	ui32 dwBits = nState << (nPin*4);

	ui32* pCR = ((ui32*)dwAddr);
	*pCR = (*pCR & dwMask) | dwBits;
}

#if 0
#define GPIO_Pin_0                 ((u16)0x0001)  /* Pin 0 selected */
#define GPIO_Pin_1                 ((u16)0x0002)  /* Pin 1 selected */
#define GPIO_Pin_2                 ((u16)0x0004)  /* Pin 2 selected */
#define GPIO_Pin_3                 ((u16)0x0008)  /* Pin 3 selected */
#define GPIO_Pin_4                 ((u16)0x0010)  /* Pin 4 selected */
#define GPIO_Pin_5                 ((u16)0x0020)  /* Pin 5 selected */
#define GPIO_Pin_6                 ((u16)0x0040)  /* Pin 6 selected */
#define GPIO_Pin_7                 ((u16)0x0080)  /* Pin 7 selected */
#define GPIO_Pin_8                 ((u16)0x0100)  /* Pin 8 selected */
#define GPIO_Pin_9                 ((u16)0x0200)  /* Pin 9 selected */
#define GPIO_Pin_10                ((u16)0x0400)  /* Pin 10 selected */
#define GPIO_Pin_11                ((u16)0x0800)  /* Pin 11 selected */
#define GPIO_Pin_12                ((u16)0x1000)  /* Pin 12 selected */
#define GPIO_Pin_13                ((u16)0x2000)  /* Pin 13 selected */
#define GPIO_Pin_14                ((u16)0x4000)  /* Pin 14 selected */
#define GPIO_Pin_15                ((u16)0x8000)  /* Pin 15 selected */
#endif

/*static*/ void BIOS::GPIO::SetPin(int nPort, int nPin, bool bValue)
{
	ui32 dwAddr = arrGpioBase[nPort];
	if ( bValue )
	{
		ui32* pBSRR = &((ui32*)dwAddr)[4];
		*pBSRR = 1<<nPin;
	} else
	{
		ui32* pBRR = &((ui32*)dwAddr)[5];
		*pBRR = 1<<nPin;
	}
}

/*static*/ bool BIOS::GPIO::GetPin(int nPort, int nPin)
{
	ui32 dwAddr = arrGpioBase[nPort];
	ui32 dwIDR = ((ui32*)dwAddr)[2];
	return (dwIDR & (1<<nPin)) ? 1 : 0;
}
