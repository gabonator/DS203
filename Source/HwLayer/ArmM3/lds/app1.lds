/* Define memory regions. */
MEMORY
{
  RAM (rwx) : ORIGIN = 0x20003000, LENGTH = 34K
	ROM (rx) : ORIGIN = 0x804C000, LENGTH = 200K
  ENTRY (rx) : ORIGIN = 0x0800C000, LENGTH = 32K
  BIOSRAM (rwx) : ORIGIN = 0x20000800, LENGTH = 10K /* This is cheekily in SYS RAM space but the gcc SYS only uses 1396 bytes of the 12K */
}

INCLUDE ../Source/HwLayer/ArmM3/lds/main.lds
