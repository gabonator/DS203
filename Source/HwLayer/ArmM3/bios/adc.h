bool g_bAdcEnabled = true;
#define ADCSIZE 4096
extern BIOS::ADC::TSample g_ADCMem[ADCSIZE];
int g_nBufferLength = ADCSIZE;

float g_fTimePerDiv = 1.0f;
ui32 g_dwStarted = 0;

/*static*/ void BIOS::ADC::Init()
{
    if(__Get(FPGA_OK, 0)== 0){
      BIOS::LCD::Print(6*8, 30, RGB565(ff0000), RGB565(ffffff), "FPGA configuration error");
//      while (1){};
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

int _binary_search(ui32* A, ui32 key, int imin, int imax)
{
  // continue searching while [imin,imax] is not empty
  while (imax >= imin)
    {
      /* calculate the midpoint for roughly equal partition */
      int imid = (imin + imax)/2;
 
      // determine which subarray to search
      if      (A[imid] <  key)
        // change min index to search upper subarray
        imin = imid + 1;
      else if (A[imid] > key )
        // change max index to search lower subarray
        imax = imid - 1;
      else
        // key found at index imid
        return imid;
    }
  // key not found
  return -1;
}
/*static*/ void BIOS::ADC::Configure(ui8 nACouple, ui8 nARange, ui16 nAOffset, ui8 nBCouple, ui8 nBRange, ui16 nBOffset, float fTimePerDiv)
{      
	const static ui16 arrPsc[] = 
	{ 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 1-1, 10-1, 10-1, 10-1, 100-1, 100-1, 
	100-1, 1000-1, 1000-1, 1000-1, 1000-1, 1000-1 };
	const static ui16 arrArr[] = 
	{ 1-1, 1-1, 2-1, 3-1, 5-1, 12-1, 24-1, 48-1, 120-1, 240-1, 480-1, 120-1, 240-1, 480-1,
	120-1, 240-1, 480-1, 120-1, 240-1, 480-1, 1200-1, 1000-1};           
	const static float arrTime[] =
		{100e-9f, 200e-9f, 500e-9f,
		1e-6f, 2e-6f, 5e-6f,
		10e-6f, 20e-6f, 50e-6f, 100e-6f, 200e-6f, 500e-6f,
		1e-3f, 2e-3f, 5e-3f,
		10e-3f, 20e-3f, 50e-3f, 100e-3f, 200e-3f, 500e-3f,
		1.0f};

	// positive floats can be treated as dwords when compared
	void* pArray = (void*)arrTime; // prevent gcc error "strict alignment rules"
	void* pValue = (void*)&fTimePerDiv;
	int nI = _binary_search( (ui32*)pArray, *(ui32*)pValue, 0, (int)COUNT(arrTime)-1 );

	_ASSERT( nI >= 0 || nI < (int)COUNT(arrPsc) );
	if ( nI == -1 )
	{
		return;
	}

	g_fTimePerDiv = fTimePerDiv;
//	g_fSamplingFreq = 30.0f / fTimePerDiv;


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
    __Set(T_BASE_PSC, arrPsc[nI]);//0);
    __Set(T_BASE_ARR, arrArr[nI]);//480-1);
}

/*static*/ unsigned char BIOS::ADC::Ready()
{
	return __Get(FIFO_FULL, 0);
}

/*static*/ unsigned long BIOS::ADC::Get()
{
	return __Read_FIFO();
}

/*
#define DepthL               32+6       //[ 7:0] <= DataBuff;   4k,2k,1k,512,360      
#define DepthH               32+7       //[11:8] <= DataBuff;      

#define PerCntL              32+8       //[7:0] <= DataBuff;                         
#define PerCntH              32+9       //[11:8] <= DataBuff;        
*/

/*static*/ void BIOS::ADC::Restart()
{
/*
	__Set(DepthL, 20 & 0xff);
	__Set(DepthH, (20 >> 8) & 0xff);

	__Set(PerCntL, (0 & 0xff));
	__Set(PerCntH, ((0>>8) & 0xff));
*/
	__Set(FIFO_CLR, W_PTR);
	g_dwStarted = BIOS::SYS::GetTick();
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

/*
void BIOS::ADC::ConfigureBuffer(int nLength)
{
	BIOS::DBG::Print("setlen=%d\n", nLength);
	// only compatible with new SYS version
	_ASSERT( nLength <= ADCSIZE );
	g_nBufferLength = nLength;

	__Set(DepthL, nLength & 0xff);
	__Set(DepthH, (nLength >> 8) & 0xff);
	__Set(FIFO_CLR, W_PTR);

//	int nPreLen = 200;
//	__Set(PerCntL, (nPreLen & 0xff));
//	__Set(PerCntH, ((nPreLen>>8) & 0xff));
}
*/

/*
void BIOS::ADC::GetBufferRange(int& nBegin, int& nEnd)
{
	nBegin = 8;
	nEnd = g_nBufferLength;
}
*/

/*static*/ void BIOS::ADC::Copy(int nCount)
{
	_ASSERT( nCount <= ADCSIZE );
//	__Set(FIFO_CLR, R_PTR);

	for ( int i = 0; i < /*g_nBufferLength*/ ADCSIZE; i++ )
  { 
		ui32 nOld = g_ADCMem[i] & 0xff000000;
		g_ADCMem[i] = ( Get() & 0x00ffffff ) | nOld;
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

/*static*/ BIOS::ADC::EState BIOS::ADC::GetState()
{
	int nAux = 0;
	if ( __Get(FIFO_START, 0) & 1 )
		nAux |= BIOS::ADC::Start;
	if ( __Get(FIFO_EMPTY, 0) & 1 )
		nAux |= BIOS::ADC::Empty;
	if ( __Get(FIFO_FULL, 0) & 1 )
		nAux |= BIOS::ADC::Full;

	return (BIOS::ADC::EState)(nAux);
}

/*static*/ int BIOS::ADC::GetPointer()
{
	int nTime = BIOS::SYS::GetTick() - g_dwStarted;
	float fDivs = nTime / 1000.0f / g_fTimePerDiv;
	int nSamples = fDivs * 30.0f;
	if ( nSamples > 4096 )
		return 4096;
	return nSamples;
}

