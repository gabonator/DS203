#define GPIOD_BRR   (*((vu32 *)(0x40011400+0x14)))
#define GPIOD_BSRR  (*((vu32 *)(0x40011400+0x10)))

#define LCD_PORT    (*((vu16 *)(0x60000000+0x00)))
#define LCD_PORT_ADDR          (0x60000000+0x00)

#define LCD_nRST_LOW()    GPIOD_BRR  = GPIO_Pin_11
#define LCD_nRST_HIGH()   GPIOD_BSRR = GPIO_Pin_11
#define LCD_RS_LOW()      GPIOD_BRR  = GPIO_Pin_12
#define LCD_RS_HIGH()     GPIOD_BSRR = GPIO_Pin_12

extern vu16 Delay_Cnt;

//
ui32 LCD_Type;

void LCD_DelayMs(unsigned short ms)
{
    Delay_Cnt = ms;
    while (Delay_Cnt > 0){}
}

void LCD_WR_Ctrl(u16 Reg) 
{
  LCD_RS_LOW();
  LCD_PORT = Reg;        //Reg. Addr.
  LCD_RS_HIGH();
}
void LCD_RD_Type(void) 
{ 
  LCD_WR_Ctrl(0xEF);
  LCD_Type  = (LCD_PORT&0xFF);
  LCD_Type  = (LCD_PORT&0xFF)<<24;
  LCD_Type |= (LCD_PORT&0xFF)<<16;
  LCD_Type |= (LCD_PORT&0xFF)<<8;
  LCD_Type |= (LCD_PORT&0xFF);
}

void LCD_WR_REG(u16 Reg, u16 Data) 
{
  LCD_RS_LOW();
  LCD_PORT = Reg;        //Reg. Addr.
  LCD_RS_HIGH();
  LCD_PORT  = Data;       //Reg. Data 
}

// exports

void LCD_Init()
{
  LCD_RS_HIGH();
  LCD_nRST_HIGH(); 
  LCD_DelayMs(1);           //RST=1£¬Delay 1ms      
  LCD_nRST_LOW(); 
  LCD_DelayMs(10);          //RST=0 Reset LCD£¬Delay 1ms      
  LCD_nRST_HIGH(); 
  LCD_DelayMs(50);          //RST=1£¬Delay 5 ms  
  LCD_RD_Type();
  _ASSERT( LCD_Type == 0x02049327 );

  // ILI9327
  LCD_WR_Ctrl(0xE9); 
  LCD_PORT =  0x20; 

  LCD_WR_Ctrl(0x11);   //Exit Sleep  
  LCD_DelayMs(100);           

  LCD_WR_Ctrl(0xD1);	// VCom control
  LCD_PORT =  0x00; 
  LCD_PORT =  0x5E; 
  LCD_PORT =  0x17; //1F 

  LCD_WR_Ctrl(0xD0);	// Power setting
  LCD_PORT =  0x07; 
  LCD_PORT =  0x07; 
  LCD_PORT =  0x0C; //80 
  
//  LCD_WR_Ctrl(0x36);	// SetAddress mode - scanning direction
//  LCD_PORT =  0x40; // 48
  LCD_WR_Ctrl(0x36);
  LCD_PORT = 0b01000000; 
  /* 
	7: page address order		0=top to bottom			1=bottom to top
	6: column address order		0=left to right			1=right to left
    5: page/column selection	0=normal mode			1=reverse mode
    4: vertical order			0=refresh top to btm	1=lcd refresh bottom to top
	3: RGB/BGR order			0=rgb					1=bgr
	2: 0
	1: horizontal flip			0=normal				1=flipped
	0: vertical flip			0=normal				1=flipped
  */

  
  LCD_WR_Ctrl(0x0C);	// Get pixel format ?
  LCD_PORT =  0x55; 
  
  LCD_WR_Ctrl(0x3A); 	// Set pixel format - DPI / BPI, 565
  LCD_PORT =  0x55; 
  
  LCD_WR_Ctrl(0xC1); 	// Display Timing setting
  LCD_PORT =  0x10; 
  LCD_PORT =  0x10; 
  LCD_PORT =  0x02; 
  LCD_PORT =  0x02; 
  
  LCD_WR_Ctrl(0xC0);   // Panel Driving Setting 
  LCD_PORT =  0x00; 
  LCD_PORT =  0x35; 
  LCD_PORT =  0x00; 
  LCD_PORT =  0x00; 
  LCD_PORT =  0x01; 
  LCD_PORT =  0x02; 
  
  LCD_WR_Ctrl(0xC5);   // Frame rate control - 88 Hz
  LCD_PORT =  0x01; 

  LCD_WR_Ctrl(0xD2);   // Power setting for normal mode
  LCD_PORT =  0x01; 
  LCD_PORT =  0x22; 

  LCD_WR_Ctrl(0xC8);   // Gamma Setting
  LCD_PORT =  0x01; 
  LCD_PORT =  0x73; 
  LCD_PORT =  0x37; 
  LCD_PORT =  0x00; 
  LCD_PORT =  0x0F; 
  LCD_PORT =  0x02; 
  LCD_PORT =  0x04; 
  LCD_PORT =  0x40; 
  LCD_PORT =  0x67; 
  LCD_PORT =  0x00; 
  LCD_PORT =  0x02; 
  LCD_PORT =  0x0F; 
  LCD_PORT =  0x08; 
  LCD_PORT =  0x80; 
  LCD_PORT =  0x00; 
  
  LCD_WR_Ctrl(0x29);   // Set display on
  LCD_WR_Ctrl(0x2C);	// Write memory start



/*  char str[] = "0x00000000";
  const char hex[] = "0123456789abcdef";

  for (int i=0; i<8; i++)
  {
    str[10-i-1] = hex[(LCD_Type>>(i*4))&0xf];
  }

  BIOS::LCD::Print (0, 0, RGB565(ff0000), RGB565(ffffff), str);

  while (1);
*/
}
