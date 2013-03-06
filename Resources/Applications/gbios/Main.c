#include "gbios.h"

unsigned int PRNG()
{
    static unsigned int nSeed = 5323;
    static unsigned int nX = 0;
    nSeed = (8253729 * nSeed + 2396403);
    nSeed += nX++;
    return nSeed & 32767;
}

int main(void)
{ 
	gBiosInit();
	IMPORT(PutPixel, BIOS::LCD::PutPixel, void, (int, int, ui16));
	IMPORT(Print, BIOS::LCD::Print, int, (int, int, ui16, ui16, const char*));
	IMPORT(GetKeys, BIOS::KEY::GetKeys, ui16, ());	
	IMPORT(Printf, BIOS::LCD::Printf, int, (int x, int y, unsigned short clrf, unsigned short clrb, const char * format, ...) );

  for (int x=0; x<400; x++)
    for (int y=16; y<240; y++)
    {
      u32 dx = (x-200);
      u32 dy = (y-120);
      dx *= dx;
      dy *= dy;
      u32 dl = dx+dy;

      PutPixel(x, y, (uc16)(dl>>3) & 0x001f); // len do 32k
    }

  extern u8 _vectors[];
	Print(20, 180, 0x0000, 0xffff, "hello gbios!");
	Printf(20, 200, 0x0000, 0xffff, "app @ 0x%08x", _vectors);
	Printf(20, 220, 0x0000, 0xffff, "formatted output 0x%08x %c", 0x1234, '!');

  int x[3] = {20, 380, 220};
  int y[3] = {20, 40, 220};

  int curx = x[0];
  int cury = y[0];
  while ( !(GetKeys() & 32) ) 
  {
    int c = PRNG();
    int r = c%3;
    curx = (curx+x[r]) >> 1;
    cury = (cury+y[r]) >> 1;
  	PutPixel(curx, cury, c<<1);
  }
	gBiosExit();
  return 0;
}

