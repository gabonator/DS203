/* Define memory regions. */
MEMORY
{
  ram (rwx) : ORIGIN = 0x20003000, LENGTH = 34K
  app (rwx) : ORIGIN = 0x20004800, LENGTH = 8K
/*  ram (rwx) : ORIGIN = 0x2005000, LENGTH = 8K*/
}

_estack = ORIGIN(ram)+LENGTH(ram)-0x2000;

/* Linker script for Olimex STM32-H103 eval board.
 *
 * Version: 0.1
 *
 *  Copyright 2007 CodeSourcery.
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 *
 * Modified by Olimex
 * 2009-04-07: Modified by Piotr Esden-Tempski <piotr@esden.net>
 */

SECTIONS
  {
/*    .  = 0x0;  */        /* From 0x00000000 */
	. = 0x08014000;
    .text : {
      *(.identify)
      *(.text*)        /* Program code */
      *(.rodata*)      /* Read only data */
      _etext = .;
      _sidata = _etext;
    } >app

/*    .  = 0x20000000;  */ /* From 0x20000000 */      

    .data : {
      _sdata = . ;
      *(.data)        /* Data memory */
      _edata = .;
    } >app

  .bss : {
    _sbss = .;
    *(.bss)         /* Zero-filled run time allocate data memory */
    _ebss = .;
    } >app
  }  
/*========== end of file ==========*/
	