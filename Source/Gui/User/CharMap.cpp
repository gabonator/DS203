#include "CharMap.h"
#ifdef ENABLE_CHARMAP

#include <Source/Core/Settings.h>
#include <Source/Core/Utils.h>

int nCursor = 0;

void DrawBig(int _x, int _y, int s, int ch)
{
	ui8* pData = (ui8*)BIOS::LCD::GetCharRom();
	pData += ch*14;
	CRect rcRect(_x-1, _y-1, _x+8*s+1, _y+14*s+1);
	BIOS::LCD::Rectangle(rcRect, RGB565(ffffff));
	for (int y=0; y<14; y++)
	{
		int nBits = ~pData[y];
		for (int x=0; x<8; x++)
		{
			ui16 clr = nBits & (128>>x) ? RGB565(ffffff) : RGB565(202020);
			BIOS::LCD::Bar( _x + x*s, _y + y*s, _x + (x+1)*s, _y + (y+1)*s, clr ); 
		}
	}

}

void UpdateChar( int nPos, bool bSel )
{
	char str[2] = { nPos, 0 };
	int bx = nPos % 32;
	int by = nPos / 32;
	int x = 42 + bx*11 + ((bx >= 16) ? 4 : 0);
	int y = 100 + by*16;
	str[0] = nPos ? nPos : ' ';
	BIOS::LCD::Print( x, y, bSel ? RGB565(ffffff) : RGB565(b0b0b0), 
		bSel ? RGB565(0000b0) : RGB565(000000), str );

	if ( bSel )
	{
		DrawBig( 20, 28, 4, nPos );
		BIOS::LCD::Printf(80, 34+16*0, RGB565(ffffff), 0, "Decimal     = %d  ", nPos);
		BIOS::LCD::Printf(80, 34+16*1, RGB565(ffffff), 0, "Hexadecimal = 0x%02x", nPos);
		BIOS::LCD::Printf(80, 34+16*2, RGB565(ffffff), 0, "Binary      = %s", CUtils::itoa2(nPos));
	}
}

void CWndCharMap::Create(CWnd *pParent, ui16 dwFlags)
{
	CWnd::Create("CWndCharMap", dwFlags, CRect(0, 16, 400, 240), pParent);
}

void CWndCharMap::OnPaint()
{
	char str[2] = {0, 0};

	for ( int i = 0; i < 255; i++ )
	{
		int bx = i%32;
		int by = i/32;
		int x = 42 + bx*11 + ((bx >= 16) ? 4 : 0);
		int y = 100 + by*16;
		str[0] = i ? i : ' ';
		BIOS::LCD::Print( x, y, RGB565(b0b0b0), RGB565(000000), str );
	}

	for ( int i=0; i<256; i+=32 )
	{
		int x = 4;
		int y = 100 + (i/32)*16;
		BIOS::LCD::Printf(x, y, RGB565(00ff00), 0, "0x%02x", i);
	}

	UpdateChar( nCursor, HasFocus() ? true : false );
}

void CWndCharMap::OnKey(ui16 nKey)
{
	if ( //( nKey == BIOS::KEY::KeyLeft && nCursor == 0 ) ||
		( nKey == BIOS::KEY::KeyUp && nCursor < 32 ) )
	{
		UpdateChar( nCursor, false );
		CWnd::OnKey( BIOS::KEY::KeyUp );
		return;
	}

	if ( nKey == BIOS::KEY::KeyLeft && nCursor > 0 )
	{
		UpdateChar(nCursor, false);
		nCursor--;
		UpdateChar(nCursor, true);
		return;
	}
	if ( nKey == BIOS::KEY::KeyRight && nCursor < 255 )
	{
		UpdateChar(nCursor, false);
		nCursor++;
		UpdateChar(nCursor, true);
		return;
	}
	if ( nKey == BIOS::KEY::KeyUp && nCursor >= 32 )
	{
		UpdateChar(nCursor, false);
		nCursor-=32;
		UpdateChar(nCursor, true);
		return;
	}
	if ( nKey == BIOS::KEY::KeyDown && nCursor <= 255-32 )
	{
		UpdateChar(nCursor, false);
		nCursor+=32;
		UpdateChar(nCursor, true);
		return;
	}
}

void CWndCharMap::OnMessage(CWnd* pSender, ui16 code, ui32 data)
{
	// LAYOUT ENABLE/DISABLE FROM TOP MENU BAR
	if (code == ToWord('L', 'D') )
	{
		return;
	}

	if (code == ToWord('L', 'E') )
	{
		return;
	}
}

#endif