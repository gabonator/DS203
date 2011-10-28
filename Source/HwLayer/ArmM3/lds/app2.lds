/* Define memory regions. */
MEMORY
{
  rom (rx) : ORIGIN = 0x08014000, LENGTH = 96K
  ram (rwx) : ORIGIN = 0x20003000, LENGTH = 36K
}

INCLUDE ../Source/HwLayer/ArmM3/lds/main.lds
