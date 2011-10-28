#ifndef AQW
#define AQW
extern vu32 vu32Tick;
extern vu16 Delay_Cnt;

/*static*/ ui32 BIOS::GetTick()
{
	return vu32Tick;
}

/*static*/ void BIOS::DelayMs(unsigned short ms)
{
    Delay_Cnt = ms;
    while (Delay_Cnt > 0){}
}

/*static*/ void BIOS::Init()
{
	DelayMs(500);
	LCD::Init();
	ADC::Init();
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
	BIOS::DelayMs(1000);
}

#endif