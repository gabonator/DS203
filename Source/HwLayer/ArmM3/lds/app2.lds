/* Define memory regions. */
MEMORY
{
  ROM (rx) : ORIGIN = 0x08014000, LENGTH = 96K
  RAM (rwx) : ORIGIN = 0x20003000, LENGTH = 36K
}

INCLUDE ../Source/HwLayer/ArmM3/lds/main.lds
