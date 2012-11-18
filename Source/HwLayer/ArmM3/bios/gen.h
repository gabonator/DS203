/*static*/ void BIOS::GEN::ConfigureSq(ui16 psc, ui16 arr, ui16 ccr)
{
    __Set(DIGTAL_PSC, psc);
    __Set(DIGTAL_ARR, arr);
    __Set(DIGTAL_CCR, ccr);
}

/*static*/ void BIOS::GEN::ConfigureDc(ui16 nData)
{
	static ui16 sData;
	sData = nData;
	ConfigureWave( &sData, 1 );
}

/*static*/ void BIOS::GEN::ConfigureWave(ui16* pData, ui16 cnt)
{
/*
    DMA2_Channel4->CCR &= ~DMA_CCR1_EN;
    __Set(ANALOG_PSC,  A_Tab[_Frqn].PSC);
    __Set(ANALOG_CNT, 72);
    __Set(ANALOG_PTR, (u32)ATT_DATA);
    DMA2_Channel4->CCR |= DMA_CCR1_EN;
    __Set(ANALOG_ARR, A_Tab[_Frqn].ARR);
*/


	#define DMA2_CMAR4  (*((vu32 *)(0x40020400+0x50)))
	#define DMA2_CNDTR4 (*((vu32 *)(0x40020400+0x48)))
	#define DMA2_CCR4   (*((vu32 *)(0x40020400+0x44)))
	#define DMA2_CPAR4  (*((vu32 *)(0x40020400+0x4C)))
    #define DAC_DHR12R1_ADDR ((vu32)(0x40007400+0x08))


//    DMA_Channelx->CCR |= CCR_ENABLE_Set;

    #define CCR_ENABLE_Reset        ((u32)0xFFFFFFFE)
    #define CCR_ENABLE_Set          ((u32)0x00000001)

  DMA2_CCR4 &= CCR_ENABLE_Reset;
//  DMA2_CCR4 |= CCR_ENABLE_Set;

  DMA2_CPAR4 = DAC_DHR12R1_ADDR;   
  DMA2_CCR4  = 0x000035B0;
  DMA2_CNDTR4= cnt;
  DMA2_CMAR4 = (u32)pData;

  DMA2_CCR4 |= CCR_ENABLE_Set;

//	DMA2_CNDTR4 = cnt;
//  __Set(ANALOG_ARR, arr);

/*
    __Set(ANALOG_CNT, 7);
    (*((vu32 *)(0x40020400+0x50))) = (u32)&pData[0];
    __Set(ANALOG_ARR, arr);
*/
/*
  case ANALOG_ARR:  GPIOB_CRL = 0x34BBB438;  TIM4_CR1 = 0x0080;  // SQR_OUT = Disnable  
                    GPIOA_CRL   = 0x111011BB;  DAC_CR = 0x0001;  // DAC = Ensable 
                    TIM7_ARR = Value;  TIM7_CR1 = 0x0085;  break;// DAC_CLK = Enable
  case ANALOG_PTR:  DMA2_CMAR4  = Value;
                    break;
  case ANALOG_CNT:  DMA2_CNDTR4 = Value;       // Fout = (Cnt*(ARR+1)/72)KHz
                    break;



  tmpreg |= DMA_InitStruct->DMA_DIR | DMA_InitStruct->DMA_Mode |
            DMA_InitStruct->DMA_PeripheralInc | DMA_InitStruct->DMA_MemoryInc |
            DMA_InitStruct->DMA_PeripheralDataSize | DMA_InitStruct->DMA_MemoryDataSize |
            DMA_InitStruct->DMA_Priority | DMA_InitStruct->DMA_M2M;
  DMA_Channelx->CCR = tmpreg;
  DMA_Channelx->CNDTR = DMA_InitStruct->DMA_BufferSize;
  DMA_Channelx->CPAR = DMA_InitStruct->DMA_PeripheralBaseAddr;
  DMA_Channelx->CMAR = DMA_InitStruct->DMA_MemoryBaseAddr;



#define DMA2_CCR1   (*((vu32 *)(0x40020400+0x08)))
#define DMA2_CNDTR1 (*((vu32 *)(0x40020400+0x0C)))
#define DMA2_CPAR1  (*((vu32 *)(0x40020400+0x10)))
#define DMA2_CMAR1  (*((vu32 *)(0x40020400+0x14)))
#define DMA2_CCR2   (*((vu32 *)(0x40020400+0x1C)))
#define DMA2_CNDTR2 (*((vu32 *)(0x40020400+0x20)))
#define DMA2_CPAR2  (*((vu32 *)(0x40020400+0x24)))
#define DMA2_CMAR2  (*((vu32 *)(0x40020400+0x28)))
#define DMA2_CCR4   (*((vu32 *)(0x40020400+0x44)))
#define DMA2_CNDTR4 (*((vu32 *)(0x40020400+0x48)))
#define DMA2_CPAR4  (*((vu32 *)(0x40020400+0x4C)))
#define DMA2_CMAR4  (*((vu32 *)(0x40020400+0x50)))

*/
}


/*static*/ void BIOS::GEN::ConfigureWaveRate(ui16 arr)
{
  __Set(ANALOG_ARR, arr);
}
