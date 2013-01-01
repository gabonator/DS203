#ifndef AQW
#define AQW

extern vu32 vu32Tick;
extern vu16 Delay_Cnt;
extern vu16 Beep_mS;
int m_nBeepVolume = 50;

/*static*/ ui32 BIOS::SYS::GetTick()
{
	return vu32Tick;
}


void BIOS::SYS::Standby( bool bEnterSleep )
{
    __Set(STANDBY, bEnterSleep ? EN : DN);
}

/*static*/ void BIOS::SYS::DelayMs(unsigned short ms)
{
    Delay_Cnt = ms;
    while (Delay_Cnt > 0){}
}

/*static*/ void BIOS::SYS::Beep( int ms )
{
	__Set(BEEP_VOLUME, m_nBeepVolume);
	Beep_mS = ms;
//	if ( vol > -1 )
//		__Set( BEEP_VOLUME, vol ); // 0..100
}

/*static*/ void BIOS::SYS::Init()
{
	SYS::DelayMs(500);
	LCD::Init();
	ADC::Init();
	SERIAL::Init();
}

char ttp[8]; // TODO: cannot use static char ttp[8] in itoa() scope !
char* itoa(ui16 n)
{
	int i = 0;
	while (n > 0)
	{
		ttp[i++] = '0'+(n%10);
		n /= 10;
	}
	ttp[i] = 0;
	for (int j=0; j<i/2;j++)
	{
		char t = ttp[j];
		ttp[j] = ttp[i-j-1];
		ttp[i-j-1] = t;
	}
	return ttp;
}

void Assert(const char*msg, int n)
{
//      BIOS::Print(6*8, 30, RGB565(ff0000), RGB565(ffffff), "FPGA configuration error");
	BIOS::LCD::Print(40, 16*3, 0xffff, 0x001f, (char*)"Assertion failed!");
	BIOS::LCD::Print(40, 16*2, 0xffff, 0x001f, (char*)msg);
	BIOS::LCD::Print(40, 16*1, 0xffff, 0x001f, (char*)itoa(n));
	BIOS::SYS::DelayMs(1000);
}

/*static*/ int BIOS::SYS::GetBattery()
{
	// returns value in percents
  int Vb = __Get(V_BATTERY, 0);
/*
  Level =0;
  if(Vb > 3000 ) Level +=1;
  if(Vb > 3500 ) Level +=1;
  if(Vb > 3900 ) Level +=1;
  if(Vb > 4100 ) Level +=1;
*/
                          
	// 3000 -> 10%
	// 4300 -> 100%         
	int nPerc = 10+(Vb-3000)*(100-10)/(4300-3000);
	if ( nPerc < 0 )
		return 0;
	if ( nPerc > 100 )
		return 100;

	return nPerc;
}

/*static*/ void BIOS::SYS::SetBacklight(int nLevel) // 0..100
{
	if (nLevel < 1)
		nLevel = 1;
	__Set(BACKLIGHT, nLevel);
}

/*static*/ void BIOS::SYS::SetVolume(int nLevel) // 0..100
{
	nLevel /= 2;
  m_nBeepVolume = nLevel;
}

#define APP4_BASE ((u32)(0x08024000)) // Size = 32KB
#define APP3_BASE ((u32)(0x0801C000)) // Size = 32KB
#define APP2_BASE ((u32)(0x08014000)) // Size = 32KB
#define APP1_BASE ((u32)(0x0800C000)) // Size = 32KB
#define SYS_BASE ((u32)(0x08004000)) // Size = 32KB
#define DFU_BASE ((u32)(0x08000000)) // Size = 16KB 

void BIOS::SYS::Execute( int nCode )
{
	u32 dwGotoAddr = NULL;
	switch ( nCode )
	{
		case BIOS::SYS::EApp1: dwGotoAddr = APP1_BASE; break;
		case BIOS::SYS::EApp2: dwGotoAddr = APP2_BASE; break;
		case BIOS::SYS::EApp3: dwGotoAddr = APP3_BASE; break;
		case BIOS::SYS::EApp4: dwGotoAddr = APP4_BASE; break;
		case BIOS::SYS::ESys: dwGotoAddr = SYS_BASE; break;
		case BIOS::SYS::EDfu: dwGotoAddr = DFU_BASE; break;
	}
	// what about interrupt handlers? NVIC?
	if ( !dwGotoAddr )
		return;

	u32 dwGotoBase = *(vu32*)(dwGotoAddr/*+4*/);
	typedef void (*pFunc)(void);
	pFunc GotoApp = (pFunc)(dwGotoBase);

	__MSR_MSP(dwGotoAddr);
	GotoApp();
}

void BIOS::SYS::Set( int nKey, int nValue )
{
	__Set( nKey, nValue );
}

int BIOS::SYS::Get( int nKey, int nSub  )
{
	if ( nKey == ToWord('r', 'f') )
	{
		return __Read_FIFO();
	}
	return __Get( nKey, nSub );
}

#if 0
int BIOS::SYS::GetTemperature()
{
	// http://wiki.leaflabs.com/index.php?title=Accessing_the_Maple%27s_internal_temperature_sensor
	#define adc_reg_map ADC_TypeDef
	#define ADC1 ((ADC_TypeDef*)ADC1_BASE)
	#define ADC_CR2_TSEREFE ADC_CR2_TSVREFE
  adc_reg_map *regs = ADC1 /*->regs*/;

// 3. Set the TSVREFE bit in the ADC control register 2 (ADC_CR2) to wake up the
//    temperature sensor from power down mode.  Do this first 'cause according to
//    the Datasheet section 5.3.21 it takes from 4 to 10 uS to power up the sensor.

  regs->CR2 |= ADC_CR2_TSEREFE;

// 2. Select a sample time of 17.1 µs
// set channel 16 sample time to 239.5 cycles
// 239.5 cycles of the ADC clock (72MHz/6=12MHz) is over 17.1us (about 20us), but no smaller
// sample time exceeds 17.1us.

  regs->SMPR1 = (0b111 << (3*6));     // set channel 16, the temp. sensor

  SYS::DelayMs(1);

  return adc_read(ADC1, 16);
}
#endif

#include "Source/HwLayer/ArmM3/stm32f10x/src/stm32f10x_adc.c"

void RCC_APB2PeriphResetCmd(u32 RCC_APB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->APB2RSTR |= RCC_APB2Periph;
  }
  else
  {
    RCC->APB2RSTR &= ~RCC_APB2Periph;
  }
}
void RCC_AHBPeriphClockCmd(u32 RCC_AHBPeriph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB_PERIPH(RCC_AHBPeriph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->AHBENR |= RCC_AHBPeriph;
  }
  else
  {
    RCC->AHBENR &= ~RCC_AHBPeriph;
  }
}

void RCC_APB2PeriphClockCmd(u32 RCC_APB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->APB2ENR |= RCC_APB2Periph;
  }
  else
  {
    RCC->APB2ENR &= ~RCC_APB2Periph;
  }
}

int BIOS::SYS::GetTemperature()
{
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
#if 0
  ADC_InitTypeDef ADC_InitStructure;
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	 
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;			  
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	 
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;		   
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

#endif
  /* ADC1 regular channe16 configuration */ 
  //ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);  
  /* Enable the temperature sensor and vref internal channel */ 
  ADC_TempSensorVrefintCmd(ENABLE);    
  /* Enable ADC1 */
//  ADC_Cmd(ADC1, ENABLE);
//  BIOS::SYS::DelayMs(5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_55Cycles5);  
  BIOS::SYS::DelayMs(1);
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)!=SET);

	int ADCConvertedValue = ADC_GetConversionValue(ADC1);
//	float fTemp = (1.42f - ADCConvertedValue*3.3f/4096)*1000/4.35f + 25;
  // 351.43 - 0.18521*ADC
//	ADC_TempSensorVrefintCmd(DISABLE);

  return ADCConvertedValue;
//	return (int)(fTemp);
}

int BIOS::SYS::GetCoreVoltage()
{
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
#if 0
  ADC_InitTypeDef ADC_InitStructure;
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	 
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;			  
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	 
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;		   
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
#endif

  /* ADC1 regular channe16 configuration */ 
  //ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);  
  /* Enable the temperature sensor and vref internal channel */ 
  ADC_TempSensorVrefintCmd(ENABLE);    
  /* Enable ADC1 */
//  ADC_Cmd(ADC1, ENABLE);
//  BIOS::SYS::DelayMs(5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 1, ADC_SampleTime_55Cycles5);  
  BIOS::SYS::DelayMs(1);
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)!=SET);

	int ADCConvertedValue = ADC_GetConversionValue(ADC1);
  return ADCConvertedValue;
}

#endif
