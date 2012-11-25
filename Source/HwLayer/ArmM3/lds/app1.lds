/* Define memory regions. */
MEMORY
{
  ROM (rx) : ORIGIN = 0x0800C000, LENGTH = 126K
/* STM32F103VCT6 shoud have 48kb ram */
/*  RAM (rwx) : ORIGIN = 0x20003000, LENGTH = 36K */ /* 4kB should be allocated to stack */
  RAM (rwx) : ORIGIN = 0x20003000, LENGTH = 34K
  EXTRA (rx) : ORIGIN = 0x08064000, LENGTH = 128K
}

INCLUDE ../Source/HwLayer/ArmM3/lds/main.lds
