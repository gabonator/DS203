/* Define memory regions. */
MEMORY
{
  RAM (rwx) : ORIGIN = 0x20003000, LENGTH = 34K
	ROM (rx) : ORIGIN = 0x0804C000, LENGTH = 200K
  ENTRY (rx) : ORIGIN = 0x0800C000, LENGTH = 2K
/*  BIOSRAM (rwx) : ORIGIN = 0x20003000, LENGTH = 2K
	BIOSROM (rwx) : ORIGIN = 0x0800C800, LENGTH = 30K*/
  COOKIES (rwx) : ORIGIN = 0x20000800, LENGTH = 1K
}

/*
	// 0x20000000 .. 0x20000574  BIOS      (1396 bytes)
	// 0x20000800 .. 0x20000C00  COOKIES   (1k)
	// 0x20000C00 .. 0x20001800  GBIOS     (3k)
	// 0x20001800 .. 0x20002000  JPA       (2k)
	// 0x20002000 .. 0x20003000            (4k)
	// 0x20003000 .. 0x2000B800  APP RAM   (34k)
	// 0x2000B800 .. 0x2000C000  STACK     (2k)
*/
INCLUDE ../Source/HwLayer/ArmM3/lds/main.lds
