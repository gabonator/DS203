bool g_bAdcEnabled = false;
#define ADCSIZE 4096
BIOS::ADC::TSample g_ADCMem[ADCSIZE];  // only 16 bits for sample, no wasted memory :)

/*static*/ void BIOS::ADC::Init()
{
    if(__Get(FPGA_OK)== 0){
      BIOS::LCD::Print(6*8, 30, RGB565(ff0000), RGB565(ffffff), "FPGA configuration error");
      while (1){};
    }
//    __Set(ADC_CTRL, EN );
    __Set(STANDBY, DN);          // ÍË³öÊ¡µç×´Ì¬
}

/*static*/ void BIOS::ADC::Enable(bool bEnable)
{
		g_bAdcEnabled = bEnable;
    __Set(ADC_CTRL, bEnable ? EN : DN );
}

/*static*/ bool BIOS::ADC::Enabled()
{
	return g_bAdcEnabled;
}

/*static*/ void BIOS::ADC::Configure(ui8 nACouple, ui8 nARange, ui16 nAOffset, ui8 nBCouple, ui8 nBRange, ui16 nBOffset, ui16 nTimePsc, ui16 nTimeArr)
{      
//    __Set(TRIGG_MODE, UNCONDITION);

    // init channel
//    __Set(ADC_CTRL, EN);       
    __Set(ADC_MODE, SEPARATE);               

    __Set(CH_A_COUPLE, nACouple); //0);
    __Set(CH_A_RANGE,  nARange); //1);
    __Set(CH_A_OFFSET, nAOffset); //100);

    __Set(CH_B_COUPLE, nBCouple);
    __Set(CH_B_RANGE,  nBRange);
    __Set(CH_B_OFFSET, nBOffset);

   //  {" 1mS ",    10-1,   240-1,   120,  1024,    33333},   
    __Set(T_BASE_PSC, nTimePsc);//0);
    __Set(T_BASE_ARR, nTimeArr);//480-1);
}

/*static*/ unsigned char BIOS::ADC::Ready()
{
	return __Get(FIFO_FULL);
}

/*static*/ unsigned long BIOS::ADC::Get()
{
	return __Read_FIFO();
}

/*static*/ void BIOS::ADC::Restart()
{
	__Set(FIFO_CLR, W_PTR);
}

void BIOS::ADC::ConfigureTrigger(ui16 nTThreshold, ui16 nVThreshold, ui8 nSource, ui8 nType)
{
/*
nSource:
#define TRACK1     0
#define TRACK2     1
#define TRACK3     2
#define TRACK4     3
nType:
0  <Vt
1  >Vt


CH_A Trigger source & kind select => 
0x00: by Negedge;   0x01: by Posedge;   0x02: by low level; 0x03: by high level
0x04: TL < Delta_T; 0x05: TL > Delta_T; 0x06: TH < Delta_T; 0x07: TH > Delta_T;

CH_B Trigger source & kind select =>
0x08: by Negedge;   0x09: by Posedge;   0x0A: by low level; 0x0B: by high level
0x0C: TL < Delta_T; 0x0D: TL > Delta_T; 0x0E: TH < Delta_T; 0x0F: TH > Delta_T;

CH_C Trigger source & kind select =>
0x10: by Negedge;   0x11: by Posedge;   0x12: by low level; 0x13: by high level
0x04: TL < Delta_T; 0x05: TL > Delta_T; 0x06: TH < Delta_T; 0x07: TH > Delta_T;

CH_D Trigger source & kind select =>
0x18: by Negedge;   0x19: by Posedge;   0x1A: by low level; 0x1B: by high level
0x1C: TL < Delta_T; 0x1D: TL > Delta_T; 0x1E: TH < Delta_T; 0x1F: TH > Delta_T;

0x20~0xFF  =>  Unconditional trigger

*/
  if ( nType == -1 )
	{
    __Set(TRIGG_MODE, UNCONDITION);
    return;
  }
  
	__Set(T_THRESHOLD, nTThreshold);  
	__Set(V_THRESHOLD, nVThreshold);  
	__Set(TRIGG_MODE,  (nSource << 3) | nType);
}

/*static*/ void BIOS::ADC::Copy(int nCount)
{
	_ASSERT( nCount <= ADCSIZE );
	for ( int i = 0; i < ADCSIZE; i++ )
  { 
		g_ADCMem[i] = Get(); // & 0xffff;
  }
}

/*static*/ unsigned long BIOS::ADC::GetCount()
{
	return ADCSIZE;
}

/*static*/ BIOS::ADC::TSample& BIOS::ADC::GetAt(int i)
{
	_ASSERT( i >= 0 && i < ADCSIZE );
	return g_ADCMem[i];
}
