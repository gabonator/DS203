// bmconv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <Windows.h>
#include <crtdbg.h>

/*
typedef struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER, FAR *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER, FAR *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

*/

int nofs = 0;
void pushbyte(FILE*f, unsigned int a)
{
	static int nCnt = 0;
	_ASSERT( a>=0 && a<256);
	fprintf(f, "0x%02x, ", a);
	//printf("0x%02x, ", a);
	if (++nCnt == 32 )
	{
		fprintf(f, "\n	");
		printf("\n");
		nCnt = 0;
	}
	nofs++;
}

void pushvlc8(FILE*f, unsigned int a)
{
	if (a < 128)
		pushbyte(f, a);
	else
	{
		pushbyte( f, (a&127) | 128 );
		a >>= 7;
		_ASSERT( a < 128 );
		pushbyte(f, a);
	}
}

void pushword(FILE*f, unsigned int a)
{
	pushbyte(f, a>>8);
	pushbyte(f, a&0xff);
}

unsigned char accum = 0;
unsigned int naccum = 0;

void pushfix4(FILE* f, unsigned int a)
{
	if ( naccum == 0 )
	{
		accum = a;
		naccum = 1;
		return;
	}
	accum <<= 4;
	accum |= a;
	pushbyte(f, accum);
	naccum = 0;
}

void pushvlc4(FILE* f, unsigned int a)
{
	if ( a==0 )
	{
		pushfix4(f, 0);
	}
	while (a>0)
	{
		if ( a <= 7 )
			pushfix4(f, a&7);
		else
			pushfix4(f, (a&7) | 8);
		a >>= 3;
	}
}
void pushclose(FILE* f)
{
	if (naccum)
		pushfix4(f, 0);
}



int _tmain(int argc, _TCHAR* argv[])
{
	bool bPalette = true;
	FILE* f = fopen("C:\\Temp\\icon_about.bmp", "rb");
	FILE* fo = fopen("C:\\Temp\\icon_about.h", "w");
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	fread(&bmfh, sizeof(bmfh), 1, f);
	fread(&bmih, sizeof(bmih), 1, f);
	
	fprintf(fo, 
"LINKERSECTION(\".extra\")\n"
"/*static*/ const unsigned char CWndClassName::bitmapName[] = {\n\t");
	unsigned char pbmp[400*200];
	pushbyte(fo, 'G');
	pushbyte(fo, bPalette ? 'B' : 'b' );
	pushvlc8(fo, bmih.biWidth);
	pushvlc8(fo, bmih.biHeight);
	printf( "w:%d, h:%d, c:%d\n", bmih.biWidth, bmih.biHeight, bmih.biClrUsed );
	//bmih.biClrUsed = 16;
	int i;
	for (i=0; i<bmih.biClrUsed; i++)
	{
		RGBQUAD clr;
		fread(&clr, sizeof(clr), 1, f);
		printf("clr %2d: %02x %02x %02x\n", i, clr.rgbRed, clr.rgbGreen, clr.rgbBlue );
		
		#define RGB565RGB(r, g, b) (((r)>>3)|(((g)>>2)<<5)|(((b)>>3)<<11))
		if ( bPalette && i<16 )
			pushword( fo, RGB565RGB( clr.rgbRed, clr.rgbGreen, clr.rgbBlue ) );
		// push rgb565()
	}
	for (; bPalette && i<16; i++)
		pushword(fo, 0);

	int w = bmih.biWidth;
	int h = bmih.biHeight;
	for ( int y=0; y<h; y++)
	{
		/*
		while ( (ftell(f) & 3) != 0 )
		{
			unsigned char c;
			fread(&c, 1, 1, f);
		}*/

		int x;
		printf("\n");
		for ( x=0; x<w; x++)
		{
			unsigned char c;
			fread(&c, 1, 1, f);
			printf("%x", c);
			_ASSERT( c>=0 && c<=15 );
			if ((bmih.biHeight-y-1)&1)
				pbmp[(bmih.biHeight-y-1)*w+ w-x-1] = c;
			else
				pbmp[(bmih.biHeight-y-1)*w+ x] = c;

		}
		int p;
		while ( ( x++ & 3) != 0 )
		{

			unsigned char c;
			fread(&c, 1, 1, f);
			c=c;
			int p = ftell(f);
			int t =9;
			//_ASSERT( c == 255 );
		}

		/*
		int nExtra = ((w&3) == 0) ? 0 : (4-(w&3));
		if (nExtra > 0)
			fseek(f, nExtra, SEEK_CUR);
			*/
	}

	int nTotal = w*h;
	int nIndex = 0;
	bool bSame = true;
	do {
		// zisti pocet pixelov rovnakej farby
		int nCurrent = pbmp[nIndex];
		if ( bSame )
		{
			int j=nIndex+1;
			for (; j<nTotal && pbmp[j] == nCurrent; j++);
			int nLen = j-nIndex;
			nIndex = j;

			printf("clr(%x, %d) ", nCurrent, nLen);
			pushvlc4(fo, nLen-1);
			pushfix4(fo, nCurrent&0x0f);
			bSame = false;
		} else
		{	// unique
			int j = nIndex+1;
			for (; j<nTotal-1 && pbmp[j] != pbmp[j+1] && (j-nIndex)<=15; j++);

			int nLen = j-nIndex;
			printf("seq(%d: ", nLen);

			pushfix4(fo, nLen-1);
			for (int i=nIndex; i<j; i++)
			{
				printf("%d, ", pbmp[i]);
				pushfix4(fo, (pbmp[i])&0x0f);
			}
			printf(") ");

			nIndex = j;

			bSame = true;
		}
		// push VLC (nLen)
		// push nCurrent

	} while (nIndex < nTotal);

	pushclose(fo);

	//RGBQUAD          aColors[];	
	//BYTE             aBitmapBits[];
	fprintf(fo, "\n}; // %d bytes\n", nofs);
	fclose(f);
	fclose(fo);
	return 0;
}

