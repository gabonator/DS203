/********************* (C) COPYRIGHT 2010 e-Design Co.,Ltd. ********************
 File Name : BIOS.h  
 Version   : DS203_APP Ver 2.5x                                  Author : bure
*******************************************************************************/
#ifndef __BIOS_H
#define __BIOS_H

#include "stm32f10x_lib.h"


//============================= Flash 空间分配 =================================

#define BIN_BASE                ((u32)(0x0802C000)) // Size < 68KB  
#define PRM_BASE                BIN_BASE + 68*1024  // Size =  2KB   
#define INF_BASE                BIN_BASE + 70*1024  // Size < 10KB   
#define APP4_BASE               ((u32)(0x08024000)) // Size = 32KB  
#define APP3_BASE               ((u32)(0x0801C000)) // Size = 32KB  
#define APP2_BASE               ((u32)(0x08014000)) // Size = 32KB  
#define APP1_BASE               ((u32)(0x0800C000)) // Size = 32KB  
#define SYS_BASE                ((u32)(0x08004000)) // Size = 32KB   
#define DFU_BASE                ((u32)(0x08000000)) // Size = 16KB 

//====================== 函数 Set 中 Object 及 Value 定义 ======================

#define CH_A_OFFSET       0     // A通道垂直位移    Value = 0~200
#define CH_B_OFFSET       1     // B通道垂直位移    Value = 0~200
#define BACKLIGHT         2     // 背光亮度         Value = 0~100
#define BEEP_VOLUME       3     // 蜂鸣器音量       Value = 0~100
#define BETTERY_DT        4     // 电池电压检测     Value = 1: 启动
#define ADC_MODE          5     // ADC工作模式      Value = 1/0
#define FIFO_CLR          6     // FIFO指针复位     Value = 1/0: W_PTR/R_PTR
  #define R_PTR           0       // FIFO读地址指针复位
  #define W_PTR           1       // FIFO写地址指针复位
#define T_BASE_PSC        7     // 时基预分频值     Value = 0~65535
#define T_BASE_ARR        8     // 时基分频值       Value = 0~65535
#define CH_A_COUPLE       9     // A通道耦合方式    Value = 1/0: AC/DC
  #define DC              0
  #define AC              1
#define CH_A_RANGE       10     // A通道输入量程    Value = 0~5
#define CH_B_COUPLE      11     // B通道耦合方式    Value = 1/0: AC/DC
//#define DC              0
//#define AC              1
#define CH_B_RANGE       12     // B通道输入量程    Value = 0~5
#define ANALOG_ARR       13     // 模拟输出分频值   Value = 0~65535
#define ANALOG_PTR       14     // 模拟输出指针     Value = 0~65535
#define ANALOG_CNT       15     // 每周期合成点数   Value = 0~65535
#define DIGTAL_PSC       16     // 脉冲输出预分频值 Value = 0~65535
#define DIGTAL_ARR       17     // 脉冲输出分频值   Value = 0~65535
#define DIGTAL_CCR       18     // 脉冲输出占空比值 Value = 0~65535
#define KEY_IF_RST       19     // 定时中断标志复位 Value = 0
#define STANDBY          20     // 进入降功耗等待   Value = 0
#define FPGA_RST         31     // FPGA 复位        Value = 0

#define TRIGG_MODE       32+0  // 触发模式          Value = Mode
#define V_THRESHOLD      32+1  // 电压触发门限      Value = 0~200
#define T_THRESHOLD      32+2  // 脉宽触发时间门限  Value = 0~65535
#define ADC_CTRL         32+4  // ADC 工作状态设置  Value = 1/0 EN/DN
#define A_POSITION       32+5  // CH_A的零点位置    Value = 0~200
#define B_POSITION       32+6  // CH_B的零点位置    Value = 0~200
#define REG_ADDR         32+7  // 该地址决定了FPGA中哪一组寄存器的数据被MCU读入

//==================== 函数 Set 中 TRIGG_MODE 的 Value 定义=====================
/*
CH_A Trigger source & kind select => 
0x00: by Negedge;   0x01: by Posedge;   0x02: by low level; 0x03: by high level
0x04: TL < Delta_T; 0x05: TL > Delta_T; 0x06: TH < Delta_T; 0x07: TH > Delta_T;

CH_B Trigger source & kind select =>
0x08: by Negedge;   0x09: by Posedge;   0x0A: by low level; 0x0B: by high level
0x0C: TL < Delta_T; 0x0D: TL > Delta_T; 0x0E: TH < Delta_T; 0x0F: TH > Delta_T;

CH_C Trigger source & kind select =>
0x10: by Negedge;   0x11: by Posedge;   0x12: by low level; 0x13: by high level
0x04: TL < Delta_T; 0x05: TL > Delta_T; 0x06: TH < Delta_T; 0x07: TH > Delta_T;

CH_D Trigger source & kind select =>
0x18: by Negedge;   0x19: by Posedge;   0x1A: by low level; 0x1B: by high level
0x1C: TL < Delta_T; 0x1D: TL > Delta_T; 0x1E: TH < Delta_T; 0x1F: TH > Delta_T;

0x20~0xFF  =>  Unconditional trigger
*/
#define UNCONDITION       0x20        // 无条件触发采样        

//================ 函数 Set 中 ADC_CTRL & STANDBY 的 Value 定义 ================

#define DN            0
#define EN            1

//===================== 函数 Set 中 ADC_MODE 的 Value 定义 =====================

#define SEPARATE      0    // ADC独立采样模式
#define INTERLACE     1    // ADC交替采样模式

//========================= 函数 Get 中的 Kind 定义 ============================

#define FIFO_DIGIT             0    // 16bits FIFO digital data
#define FIFO_EMPTY             1    // FIFO empty flag: 1 = empty
#define FIFO_START             2    // FIFO start flag: 1 = start
#define FIFO_FULL              3    // FIFO full flag: 1 = Full
#define KEY_STATUS             4    // Current keys status 
  #define K_ITEM_D_STATUS      0x0008    // 0 = Key push on
  #define K_ITEM_S_STATUS      0x0040    // 0 = Key push on
  #define KEY3_STATUS          0x0100    // 0 = Key push on
  #define KEY4_STATUS          0x0200    // 0 = Key push on
  #define K_INDEX_D_STATUS     0x0400    // 0 = Key push on
  #define K_INDEX_I_STATUS     0x0800    // 0 = Key push on
  #define K_INDEX_S_STATUS     0x1000    // 0 = Key push on
  #define KEY2_STATUS          0x2000    // 0 = Key push on
  #define KEY1_STATUS          0x4000    // 0 = Key push on
  #define K_ITEM_I_STATUS      0x8000    // 0 = Key push on
#define USB_POWER              5    // USB power status: 1 = Power ON
#define V_BATTERY              6    // Battery voltage (mV)
#define VERTICAL               7    // 垂直通道属性指针
#define HORIZONTAL             8    // 水平通道属性指针
#define GLOBAL                 9    // 整体属性指针
#define TRIGGER                10   // 触发通道属性指针
#define FPGA_OK                11   // FPGA 配置成功 1 = FPGA config ok
#define CHARGE         12       // 电池充电状态
#define HDWVER         13       // 设备硬件版本号 
#define DFUVER         14       // DFU程序模块版本号 
#define SYSVER         15       // SYS程序模块版本号 
#define FPGAVER        16       // FPGA配置程序版本号 

#define ADC_DATA       32+0     // 0~7:ADC_CH_A 8~15:ADC_CH_B 16~17:CH_C&CH_D 
#define PRE_SAMPLING   32+1     // 0~15:Pre-sampling depth
#define ALL_SAMPLING   32+2     // 0~15:Total sampling depth
#define CH_A_MIN_MAX   32+3     // 0~7:VMIN 8~15:VMAX 
#define CH_A_V_SUM     32+4     // 0~15:CH_A voltage sum
#define CH_A_V_SSQ     32+5     // 0~15:CH_A voltage sum of squares
#define CH_A_NEDGE     32+6     // 0~15:CH_A number of edge
#define CH_A_FREQ      32+7     // 0~15:CH_A frequence
#define CH_A_PHW_MAX   32+8     // 0~15:CH_A pulse high width MAX
#define CH_A_PHW_MIN   32+9     // 0~15:CH_A pulse high width MIN
#define CH_A_PLW_MAX   32+10    // 0~15:CH_A pulse low width MAX
#define CH_A_PLW_MIN   32+11    // 0~15:CH_A pulse low width MIN

// =============================================================================

typedef struct  // 硬件综合属性 
{
  u16 LCD_X;    // 屏幕水平显示点数
  u16 LCD_Y;    // 屏幕垂直显示点数
  u16 Yp_Max;   // 垂直档位最大值     
  u16 Xp_Max;   // 水平档位最大值     
  u16 Tg_Num;   // 触发档位最大值
  u16 Yv_Max;   // 垂直位移最大值 
  u16 Xt_Max;   // 水平位移最大值 
  u16 Co_Max;   // 耦合方式最大值 
  u8  Ya_Num;   // 模拟通道数目
  u8  Yd_Num;   // 数字通道数目
  u8  INSERT;   // 开始应用插值的档位
  u16 KpA1;     // A通道位移补偿系数1
  u16 KpA2;     // A通道位移补偿系数2
  u16 KpB1;     // B通道位移补偿系数1
  u16 KpB2;     // B通道位移补偿系数2
} G_attr ;

typedef struct  // 垂直通道属性 
{
  char STR[8];   // 档位标识字符串
  s16 KA1;      // A通道位移误差校正系数1
  u16 KA2;      // A通道斜率误差校正系数2
  s16 KB1;      // B通道位移误差校正系数1
  u16 KB2;      // B通道斜率误差校正系数2
  u32 SCALE;    // 垂直通道比例系数
} Y_attr ;

typedef struct  // 水平通道属性 
{
  char STR[8];   // 档位标识字符串
  s16 PSC;      // 预分频系数
  u16 ARR;      // 分频系数
  u16 CCR;      // 占空比系数
  u16 KP;       // 插值系数
  u32 SCALE;    // 水平通道比例系数
} X_attr ; 

typedef struct  // 触发通道属性 
{
  char STR[8];   // 触发方式标识字符串
  u8  CHx;      // 触发通道编号
  u8  CMD;      // 触发方式控制字
} T_attr ; 

extern Y_attr *Y_Attr; 
extern X_attr *X_Attr; 
extern G_attr *G_Attr; 
extern T_attr *T_Attr; 

//==============================================================================
//                        System function entrance
//==============================================================================
 void __CTR_HP(void);     //USB_HP_Interrupt
 void __USB_Istr(void);   //USB_LP_Interrupt
 void __USB_Init(void);                      
 
 void __LCD_Initial(void);
 void __Clear_Screen(u16 Color);
 void __Point_SCR(u16 x0, u16 y0);
 void __LCD_SetPixl(u16 Color);
 u16  __LCD_GetPixl(void);  
 u16  __Get_TAB_8x14(u8 Code, u16 Row);
 void __LCD_Set_Block(u16 x1, u16 x2, u16 y1, u16 y2);
 
 void __LCD_Copy(uc16 *pBuffer, u16  NumPixel); // Send a row data to LCD
 void __LCD_Fill(u16 *pBuffer,u16 NumPixel);    // Fill number of pixel by DMA 
 void __LCD_DMA_Ready(void);                    // Wait LCD data DMA ready
 
 void __Row_Copy(uc16 *S_Buffer,u16 *T_Buffer); // Copy one row base data to buffer
 void __Row_DMA_Ready(void);                    // Wait row base data DMA ready
 
 u32  __Read_FIFO(void);                        // Read data from FIFO & Ptr+1
 void __Display_Str(u16 x0, u16 y0, u16 Color, u8 Mode, char *s);                      
 
 u32  __Input_Lic(u16 x0, u8 y0);               //Return: 32Bits Licence 
 u32  __GetDev_SN(void);                        // Get 32bits Device Serial Number

 u8   __Ident(u32 Dev_ID, u32 Proj_ID, u32 Lic_No);
 
 void __Set(u8 Object, u32 Value);
 u32  __Get(u8 Object);
 
 void __ExtFlash_PageRD(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
 void __ExtFlash_PageWR(u8* pBuffer, u32 WriteAddr);
 u8   __ReadDiskData(u8* pBuffer, u32 ReadAddr, u16 Lenght);
 u8   __ProgDiskPage(u8* pBuffer, u32 ProgAddr);
// u8   __ClashSt(void); 
// void __ClashClr(void); 
 
 u8   __FLASH_Erase(u32 Address);
 u8   __FLASH_Prog(u32 Address, u16 Data);
 void __FLASH_Unlock(void);
 void __FLASH_Lock(void);
 
 u8   __Chk_SYS(u32 Licence);
 u8*  __Chk_DFU(void);
 u8*  __Chk_HDW(void);

 u8 __OpenFileWr(u8* Buffer, char* FileName, u16* Cluster, u32* pDirAddr);
 u8 __OpenFileRd(u8* Buffer, char* FileName, u16* Cluster, u32* pDirAddr);
 u8 __ReadFileSec(u8* Buffer, u16* Cluster);
 u8 __ProgFileSec(u8* Buffer, u16* Cluster);
 u8 __CloseFile(u8* Buffer, u32 Lenght, u16* Cluster, u32* pDirAddr);
/**/
 
#endif  
/*******************************  END OF FILE  ********************************/
