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

#endif