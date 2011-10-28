/********************* (C) COPYRIGHT 2010 e-Design Co.,Ltd. ********************
 File Name : LCD.h  
 Version   : DS203_SYS Ver 1.2x                                 Author : bure
*******************************************************************************/
#ifndef __LCD_H
#define __LCD_H

#include "stm32f10x_lib.h"

#define BLACK      0x0000
#define GRAY       0x7BEF   
#define CYAN       0xFFE0
#define YEL        0x07FF
#define ORANGE     0x051F
#define PURPL      0xF81F
#define GRN        0x07E0
#define LIGHT      0x7BEF
#define WHT        0xFFFF
#define RED        0x001F
#define BLUE       0xF800

#define PRN		0
#define INV		1

extern vu16 Delay_Cnt;
extern u32 LCD_Type;

void Delayms(u16 mS); 
void LCD_WR_REG(u16 Reg, u16 Data);
void LCD_Initial(void); 
void Clear_Screen(u16 Color);

void Point_SCR(u16 x0, u16 y0);
u16  Get_TAB_8x14(u8 Code, u16 Row);

void Display_Str(u16 x0, u16 y0, u16 Color, u8 Mode, u8 *s);
void Display_Logo(u16 x0, u16 y0);
void LCD_Set_Block(u16 x1, u16 x2, u16 y1, u16 y2); 

void LCD_WR_Ctrl(u16 Reg); 
void LCD_RD_Type(void); 
u16  LCD_RD_Data(void); 
u16  Get_Pixl(void);

#endif

/********************************* END OF FILE ********************************/
