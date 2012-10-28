/* Define memory regions. */
MEMORY
{
  ROM (rx) : ORIGIN = 0x0800C000, LENGTH = 128K
/* STM32F103VCT6 shoud have 48kb ram */
/*  RAM (rwx) : ORIGIN = 0x20003000, LENGTH = 45K */
  RAM (rwx) : ORIGIN = 0x20003000, LENGTH = 36K
}

INCLUDE ../Source/HwLayer/ArmM3/lds/main.lds
