#include "Interrupt.h"
#include "BIOS.h"

extern u8 _vectors[];

unsigned int PRNG()
{
    // our initial starting seed is 5323
    static unsigned int nSeed = 5323;
    static unsigned int nX = 0;

    // Take the current seed and generate a new value from it
    // Due to our use of large constants and overflow, it would be
    // very hard for someone to predict what the next number is
    // going to be from the previous one.
    nSeed = (8253729 * nSeed + 2396403);
    nSeed += nX++;

    // Take the seed and return a value between 0 and 32767
    return nSeed & 32767;
}

int GetKeys()
{
	return ~__Get(KEY_STATUS);
}

void Reboot()
{
	u32 dwGotoAddr = ((u32)(0x0800C000)); // APP1

  u32 *vector_table = (u32 *) dwGotoAddr;
  __MSR_MSP(vector_table[0]);
  ((void (*)(void)) vector_table[1])();
}

int main(void)
{ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, (u32)&_vectors);
  __USB_Init();
  //__Clear_Screen(0x0000);
  // display 400x240

  for (int x=0; x<400; x++)
    for (int y=0; y<240-16; y++)
    {
      u32 dx = (x-200);
      u32 dy = (y-120);
      dx *= dx;
      dy *= dy;
      u32 dl = dx+dy;

      __Point_SCR(x, y);
      __LCD_SetPixl((uc16)(dl>>3) & 0x001f);
    }

  __Display_Str(20, 200, 0xffff, 0, "Hello DS203 Test... ");

  int x[3] = {20, 380, 220};
  int y[3] = {20, 40, 220};

  int curx = x[0];
  int cury = y[0];
  while ( !(GetKeys() & KEY2_STATUS) ) 
  {
    int c = PRNG();
    int r = c%3;
    curx = (curx+x[r]) >> 1;
    cury = (cury+y[r]) >> 1;

    __Point_SCR(curx, cury);
    __LCD_SetPixl(c<<1); // len do 32k
  }
	Reboot();
  return 0;
}

