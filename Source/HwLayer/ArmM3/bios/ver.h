#define BIN_BASE ((u32)(0x0802C000)) // Size < 68KB
#define PRM_BASE BIN_BASE + 68*1024 // Size = 2KB
#define INF_BASE BIN_BASE + 70*1024 // Size < 10KB 

/*static*/ const char* BIOS::VER::GetDfuVersion()
{
	return (const char*)__Get(DFUVER, 0);//__Chk_DFU();
}

/*static*/ const char* BIOS::VER::GetHardwareVersion()
{
	return (const char*)__Get(HDWVER, 0);
}

/*static*/ const char* BIOS::VER::GetSystemVersion()
{
	return (char*)__Get(SYSVER, 0);
}

/*static*/ const char* BIOS::VER::GetFpgaVersion()
{
	return (const char*)__Get(FPGAVER, 0);
}

/*static*/ ui32 BIOS::VER::GetSerialNumber()
{
	return __GetDev_SN();
}

/*static*/ void BIOS::VER::DrawLogo(int x0, int y0)
{
  ui8 *Ptr;
  ui16 i, x, y, Color[16];
  
  Ptr = (u8*)INF_BASE;
  x = x0;
  y = y0+63;
  for(i=0; i<16; i++){ 
    Color[i]=((Ptr[0x36 + i*4]>>3)<<11)+
      ((Ptr[0x37 + i*4]>>2)<<5)+(Ptr[0x38 + i*4]>>3);
  }
  for(i=0; i<256*64/2; ++i){
		BIOS::LCD::PutPixel(x, y, Color[(Ptr[0x76 + i]&0xF0)>>4]);
    x++;
    if(x > x0+255) { x = x0; y--;}
		BIOS::LCD::PutPixel(x, y, Color[Ptr[0x76 + i]&0x0F]);
    x++;
    if(x > x0+255) { x = x0; y--;}
  }
}
