IMPORT( void, BIOS::SYS::Beep, (int ms), pBiosSysBeep );
IMPORT( ui32, BIOS::SYS::GetTick, (), pBiosSysGetTick );
IMPORT( void, BIOS::LCD::Pattern, (int x1, int y1, int x2, int y2, const ui16 *pat, int l), pBiosLcdPattern );
IMPORT( ui16, BIOS::KEY::GetKeys, (), pBiosKeyGetKeys );


