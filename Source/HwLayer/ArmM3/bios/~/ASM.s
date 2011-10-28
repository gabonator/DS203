;********************* (C) COPYRIGHT 2009 e-Design Co.,Ltd. *********************
; File Name : ASM.c       
; Version   : DS203_SYS Ver 1.5x                                  Author : bure
;*******************************************************************************
  RSEG  SYS_FUNC:CODE(2)
//  RSEG CODE:CODE(2)
;===============================================================================
;                        System function entrance
;===============================================================================
; void CTR_HP(void)     USB_HP_Interrupt
;-------------------------------------------------------------------------------
  IMPORT CTR_HP
  EXPORT __CTR_HP
__CTR_HP
    B.W    CTR_HP
;===============================================================================
; void USB_Istr(void)   USB_LP_Interrupt
;-------------------------------------------------------------------------------
  IMPORT USB_Istr
  EXPORT __USB_Istr
__USB_Istr
    B.W    USB_Istr
;===============================================================================
; void __LCD_Initial(void)  
;-------------------------------------------------------------------------------
  IMPORT LCD_Initial
  EXPORT __LCD_Initial 
__LCD_Initial 
    B.W    LCD_Initial
;===============================================================================
; void __Point_SCR(u16 x0, u16 y0)  
;-------------------------------------------------------------------------------
  IMPORT Point_SCR
  EXPORT __Point_SCR 
__Point_SCR 
    B.W    Point_SCR
;===============================================================================
; void __LCD_SetPixl(u16 Color)  
;-------------------------------------------------------------------------------

  EXPORT __LCD_SetPixl 
__LCD_SetPixl 
    B   LCD_SetPixl
    B   LCD_SetPixl

;===============================================================================
; void __Clear_Screen(u16 Color)  
;-------------------------------------------------------------------------------
  IMPORT Clear_Screen
  EXPORT __Clear_Screen 
__Clear_Screen 
    B.W    Clear_Screen
;===============================================================================
; u16 __Get_TAB_8x14(u8 Code, u16 Row)  
;-------------------------------------------------------------------------------
  IMPORT Get_TAB_8x14
  EXPORT __Get_TAB_8x14 
__Get_TAB_8x14 
    B.W    Get_TAB_8x14
;===============================================================================
; void __LCD_Set_Block(u16 x1, u16 x2, u16 y1, u16 y2)  
;-------------------------------------------------------------------------------
  IMPORT LCD_Set_Block
  EXPORT __LCD_Set_Block 
__LCD_Set_Block 
    B.W    LCD_Set_Block
;===============================================================================
; void __LCD_DMA_Ready(void)  Wait LCD data DMA ready
;-------------------------------------------------------------------------------

  EXPORT __LCD_DMA_Ready 
__LCD_DMA_Ready 
    B    LCD_DMA_Ready
    B    LCD_DMA_Ready

;===============================================================================
; void __LCD_Copy(uc16 *pBuffer, u16 NumPixel)  Send a row data to LCD
;-------------------------------------------------------------------------------

  EXPORT __LCD_Copy
  
__LCD_Copy
    B    LCD_Copy
    B    LCD_Copy
;===============================================================================
; void __LCD_Fill(u16  *pBuffer, u16 NumPixel)  Fill number of pixel by DMA 
;-------------------------------------------------------------------------------

  EXPORT __LCD_Fill
__LCD_Fill
    B    LCD_Fill
    B    LCD_Fill
;===============================================================================
; void __Row_DMA_Ready(void)  Wait row base data DMA ready
;-------------------------------------------------------------------------------

  EXPORT __Row_DMA_Ready 
__Row_DMA_Ready 
    B    Row_DMA_Ready
    B    Row_DMA_Ready
;===============================================================================
; void __Row_Copy(uc16 *S_Buffer,u16 *T_Buffer) Copy one row base data to buffer
;-------------------------------------------------------------------------------

  EXPORT __Row_Copy
__Row_Copy
    B    Row_Copy
    B    Row_Copy
;===============================================================================
; u32 __Read_FIFO(void) 
;-------------------------------------------------------------------------------

  EXPORT __Read_FIFO  
__Read_FIFO
    B    Read_FIFO
    B    Row_Copy
;===============================================================================
; u32 __Input_Lic(u16 x0, u8 y0, u16 Color);  Return: 32Bits Licence 
;-------------------------------------------------------------------------------
  IMPORT Input_Lic
  EXPORT __Input_Lic
__Input_Lic
    B.W    Input_Lic
;===============================================================================
; u32 __Dev_SN(void)   Get 32bits Device Serial Number
;-------------------------------------------------------------------------------

  EXPORT __Dev_SN
__Dev_SN
    B    Dev_SN 
    B    Dev_SN
;===============================================================================
; u8 __Chk_SYS(u32 Licence)   Check SYS licence            RET: 1 = licence ok
;-------------------------------------------------------------------------------

  EXPORT __Chk_SYS
__Chk_SYS
    B    Chk_SYS
    B    Chk_SYS
;===============================================================================
; u8 __Ident(u32 Dev_ID, u32 Proj_ID, u32 Lic_No)          RET: 1 = licence ok
;-------------------------------------------------------------------------------

  EXPORT __Ident
__Ident
    B    Ident
    B    Ident
;===============================================================================
; void __Display_Str(u16 x0, u16 y0, u16 Color, u8 Mode, u8 *s)                      
;-------------------------------------------------------------------------------
  IMPORT Display_Str
  EXPORT __Display_Str
__Display_Str
    B.W    Display_Str
;===============================================================================
; void __Set(u8 Device, u32 Value)            Hardware control device Settings
;-------------------------------------------------------------------------------
  IMPORT Set
  EXPORT __Set
__Set
    B.W    Set
;===============================================================================
; void Set_Param(u8 RegAddr, u8 Parameter)     Trigger control parameter set
;-------------------------------------------------------------------------------
  IMPORT Set_Param
  EXPORT __Set_Param
__Set_Param
    B.W    Set_Param
;===============================================================================
; u16 Get(u8 Kind)                      Get hardware attribute & status
;-------------------------------------------------------------------------------
  IMPORT Get
  EXPORT __Get
__Get
    B.W    Get
;===============================================================================
; void __ExtFlash_PageRW(u8* pBuffer, u32 WriteAddr);
;-------------------------------------------------------------------------------
  IMPORT ExtFlash_PageWR
  EXPORT __ExtFlash_PageWR
__ExtFlash_PageWR
    B.W    ExtFlash_PageWR
;===============================================================================
; void __ExtFlash_PageRD(u8* pBuffer, u32 ReadAddr, u16 Lenght)
;-------------------------------------------------------------------------------
  IMPORT ExtFlash_PageRD 
  EXPORT __ExtFlash_PageRD
__ExtFlash_PageRD
    B.W    ExtFlash_PageRD
;===============================================================================
; u8 ReadDiskData(u8* pBuffer, u32 ReadAddr, u16 Lenght)
;-------------------------------------------------------------------------------
  IMPORT ReadDiskData
  EXPORT __ReadDiskData
__ReadDiskData
    B.W    ReadDiskData
;===============================================================================
; u8 ProgDiskPage(u8* pBuffer, u32 ProgAddr)
;-------------------------------------------------------------------------------
  IMPORT ProgDiskPage
  EXPORT __ProgDiskPage
__ProgDiskPage
    B.W    ProgDiskPage
;===============================================================================
; u16 __LCD_GetPixl(void)  
;-------------------------------------------------------------------------------
  IMPORT Get_Pixl
  EXPORT __LCD_GetPixl 
__LCD_GetPixl 
    B.W    Get_Pixl
;===============================================================================
; void __USB_Init(void)
;-------------------------------------------------------------------------------
  IMPORT USB_Init
  EXPORT __USB_Init
__USB_Init
    B.W    USB_Init
;===============================================================================
; u8 __FLASH_Erase(u32 Address)                                    RET: 1 = ok
;-------------------------------------------------------------------------------
  IMPORT FLASH_Erase
  EXPORT __FLASH_Erase
__FLASH_Erase
    B.W    FLASH_Erase
;===============================================================================
; u8 __FLASH_Prog(u32 Address, u16 Data)                           RET: 1 = ok    
;-------------------------------------------------------------------------------
  IMPORT FLASH_Prog
  EXPORT __FLASH_Prog
__FLASH_Prog
    B.W    FLASH_Prog
;===============================================================================
; void __FLASH_Unlock(void)
;-------------------------------------------------------------------------------
  IMPORT FLASH_Unlock
  EXPORT __FLASH_Unlock
__FLASH_Unlock
    B.W    FLASH_Unlock
;===============================================================================
; void __FLASH_Lock(void)
;-------------------------------------------------------------------------------
  IMPORT FLASH_Lock
  EXPORT __FLASH_Lock
__FLASH_Lock
    B.W    FLASH_Lock
;===============================================================================
; u8*  __Chk_DFU(void)
;-------------------------------------------------------------------------------
  EXPORT __Chk_DFU
__Chk_DFU
    B    Chk_DFU
    B    Chk_DFU
;===============================================================================
; u8*  __Chk_HDW(void)
;-------------------------------------------------------------------------------
  EXPORT __Chk_HDW
__Chk_HDW
    B    Chk_HDW
    B    Chk_HDW
;===============================================================================
; u8 OpenFileWr(u8* Buffer, u8* FileName, u16* Cluster, u32* pDirAddr);
;-------------------------------------------------------------------------------
  IMPORT OpenFileWr
  EXPORT __OpenFileWr
__OpenFileWr
    B.W    OpenFileWr
;===============================================================================
; u8 OpenFileRd(u8* Buffer, u8* FileName, u16* Cluster, u32* pDirAddr);
;-------------------------------------------------------------------------------
  IMPORT OpenFileRd
  EXPORT __OpenFileRd
__OpenFileRd
    B.W    OpenFileRd
;===============================================================================
; u8 ReadFileSec(u8* Buffer, u16* Cluster);
;-------------------------------------------------------------------------------
  IMPORT ReadFileSec
  EXPORT __ReadFileSec
__ReadFileSec
    B.W    ReadFileSec
;===============================================================================
; u8 ProgFileSec(u8* Buffer, u16* Cluster);
;-------------------------------------------------------------------------------
  IMPORT ProgFileSec
  EXPORT __ProgFileSec
__ProgFileSec
    B.W    ProgFileSec
;===============================================================================
; u8 CloseFile(u8* Buffer, u32 Lenght, u16* Cluster, u32* pDirAddr);
;-------------------------------------------------------------------------------
  IMPORT CloseFile
  EXPORT __CloseFile
__CloseFile
    B.W    CloseFile
;===============================================================================
; void __Row_Fill(uc16 *S_Buffer,u16 *T_Buffer) Fill one row base data to buffer
;-------------------------------------------------------------------------------
  EXPORT __Row_Fill
__Row_Fill
    B     Row_Fill
    B     Row_Fill
;===============================================================================
    
Row_Fill                        ;Input: R0 = Source Ptr, R1 = Target Ptr
    LDR     R3,  =0x40020000        ;#DMA1_BASE=0x40020000
    MOVW    R2,  #0x7581
    STR     R2,  [R3, #0x1C]    ;CCR2   = 0x75C1 DMA1_CH2 enabled
Row_Fill_1
    LDR     R2,  [R3, #0x20]    ;R2 = CNDTR2 
    CMP     R2,  #0
    BNE     Row_Fill_1
    MOVW    R2,  #0x7580
    STR     R2,  [R3, #0x1C]    ;CCR2   = 0x75C0 DMA1_CH2 disabled 
    MOVW    R2,  #0x00C9
    STR     R2,  [R3, #0x20]    ;CNDTR2 = 201 
    STR     R0,  [R3, #0x24]    ;CPAR2  = R0 
    STR     R1,  [R3, #0x28]    ;CMAR2  = R1 
    MOVW    R2,  #0x7581
    STR     R2,  [R3, #0x1C]    ;CCR2   = 0x75C1 DMA1_CH2 enabled 
    BX      LR
    
Chk_DFU
    LDR     R3,  =0x0800002C        ;//0x0800002C
    MOVS    R0,  #0x0000
    LDR     R3,  [R3]
    BX      R3

Chk_HDW
    LDR     R3,  =0x08000038        ;//0x08000038
    MOVS    R0,  #0x0000
    LDR     R3,  [R3]
    BX      R3

Chk_SYS    
    LDR     R3,  =0x08000028        ;//0x08000028
    LDR     R3,  [R3]
    BX      R3

Ident    
    LDR     R3,  =0x08000024        ;//0x08000024
    LDR     R3,  [R3]
    BX      R3

Dev_SN
    LDR     R3,  =0x08000020      ;//0x08000020
    LDR     R3,  [R3]
    BX      R3

LCD_SetPixl
    LDR     R1,  =0x60000000      ;#LCD_PORT=0x60000000
    STRH    R0,  [R1, #0x00]     
    BX      LR

Row_DMA_Ready
    LDR     R1,  =0x40020000        ;#DMA1_BASE=0x40020000
    LDR     R0,  [R1, #0x1C]    ;R0 = CCR2
    MOVS    R2,  #0x0001
    ORR     R0,  R0, R2
    STR     R0,  [R1, #0x1C]    ;DMA1_CH2_EN=1
Row_DMA_Ready_1
    LDR     R0,  [R1, #0x20]    ;R0 = CNDTR2 
    CMP     R0,  #0
    BNE     Row_DMA_Ready_1
    BX      LR
    
LCD_DMA_Ready
    LDR      R1,  =0x40020400        ;#DMA2_BASE=0x40020400
    LDR     R0,  [R1, #0x08]    ;R0 = CCR1
    MOVS    R2,  #0x0001
    ORR     R0,  R0, R2
    STR     R0,  [R1, #0x08]    ;DMA_CH1_EN=1
LCD_DMA_Ready_1
    LDR     R0,  [R1, #0x0C]    ;R0 = CNDTR1 
    CMP     R0,  #0
    BNE     LCD_DMA_Ready_1
    BX      LR
    
Row_Copy                        ;Input: R0 = Source Ptr, R1 = Target Ptr
    LDR      R3,  =0x40020000        ;#DMA1_BASE=0x40020000
    MOVW    R2,  #0x75C1
    STR     R2,  [R3, #0x1C]    ;CCR2   = 0x75C1 DMA1_CH2 enabled
Row_Copy_1
    LDR     R2,  [R3, #0x20]    ;R2 = CNDTR2 
    CMP     R2,  #0
    BNE     Row_Copy_1
    MOVW    R2,  #0x75C0
    STR     R2,  [R3, #0x1C]    ;CCR2   = 0x75C0 DMA1_CH2 disabled 
    MOVW    R2,  #0x00C9
    STR     R2,  [R3, #0x20]    ;CNDTR2 = 201 
    STR     R0,  [R3, #0x24]    ;CPAR2  = R0 
    STR     R1,  [R3, #0x28]    ;CMAR2  = R1 
    MOVW    R2,  #0x75C1
    STR     R2,  [R3, #0x1C]    ;CCR2   = 0x75C1 DMA1_CH2 enabled 
    BX      LR
    
LCD_Copy                        ;Input: R0 = Source Ptr, R1 = Length
    LDR      R3,  =0x40020400        ;#DMA2_BASE=0x40020400
    MOVW    R2,  #0x5590
    STR     R2,  [R3, #0x08]    ;CCR1   = 0x5590 DMA_CH1 disabled 
    STR     R0,  [R3, #0x14]    ;CMAR1  = R0 
    STR     R1,  [R3, #0x0C]    ;CNDTR1 = R1 
    MOVW    R2,  #0x5591
    STR     R2,  [R3, #0x08]    ;CCR1   = 0x5591 DMA_CH1 enabled 
    BX      LR
    
LCD_Fill
    LDR      R3,  =0x40020400        ;#DMA2_BASE=0x40020400
    MOVW    R2,  #0x5511
    STR     R2,  [R3, #0x08]    ;CCR1   = 0x5511 DMA_CH1 enabled
LCD_Fill_1
    LDR     R2,  [R3, #0x0C]    ;R2 = CNDTR1 
    CMP     R2,  #0
    BNE     LCD_Fill_1
    MOVW    R2,  #0x5510
    STR     R2,  [R3, #0x08]    ;CCR1   = 0x5510 DMA_CH1 disabled
    STR     R0,  [R3, #0x14]    ;CMAR1  = R0 
    STR     R1,  [R3, #0x0C]    ;CNDTR1 = R1 
    MOVW    R2,  #0x5511
    STR     R2,  [R3, #0x08]    ;CCR1   = 0x5511 DMA_CH1 enabled
    BX      LR

Read_FIFO
    LDR      R1,  = 0x40011000     ; GPIO  PORT_C   
    MOVW    R2,  #0x20          ; H_L pin: GPIO_PORT_B_Bit5 
    LDR      R3,  =0x64000000    ; FIFO Port address
    STR     R2,  [R1, #0x14]    ; 0 -> H_L
    LDRH    R0,  [R3, #0]       ; FIFO Data -> R0
    LSL     R0,  R0,  #16

    STR     R2,  [R1, #0x10]    ; 1 -> H_L
    LDRH    R1,  [R3, #0]       ; FIFO Data -> R1  
    ADD     R0,  R0, R1
    BX      LR

;-------------------------------------------------------------------------------
  END

;******************************* END OF FILE ***********************************
  

