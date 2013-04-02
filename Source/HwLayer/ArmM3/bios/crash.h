// based on https://svn.kapsi.fi/jpa/dsoquad/pawn/DS203/

#define __irq__ __attribute__((interrupt("IRQ"), optimize("O2")))

/* Tools for handling crashes & debugging */
/*
#include <stdbool.h>
#include <stdint.h>
#include "BIOS.h"
#include <stdio.h>
#include "stm32f10x.h"
#include "ff.h"
#include "irq.h"
*/

/* Memory dumps */

#define fprintf(f, ... ) BIOS::DBG::Print(__VA_ARGS__)
#define snprintf(str, len, ...) BIOS::DBG::sprintf(str, __VA_ARGS__)
extern unsigned long _etext;
extern unsigned long _sidata;
extern unsigned long _sdata;
extern unsigned long _edata;
extern unsigned long _sbss;
extern unsigned long _ebss;
extern unsigned long _estack;


struct TCRASHDATA {
    uint32_t PC;
    uint32_t LR;
    uint32_t SP;
    uint32_t PSP;
    uint32_t R7;
    char BUILDID[32];
} CRASHDATA;

const char BUILDID[32] = __DATE__ " " __TIME__;

static bool make_memdump()
{
    FIL file;
    
    // Note: It's not important exactly at what point we capture the
    // data, as long as we do it all coherently and don't pop from
    // the stack before writing the memory dump.
    asm("mov %0, pc" : "=r"(CRASHDATA.PC) : :);
    asm("mov %0, lr" : "=r"(CRASHDATA.LR) : :);
    asm("mov %0, sp" : "=r"(CRASHDATA.SP) : :);
    asm("mov %0, r7" : "=r"(CRASHDATA.R7) : :);
    asm("mrs %0, psp" : "=r"(CRASHDATA.PSP) : :);
    memcpy(CRASHDATA.BUILDID, BUILDID, sizeof(BUILDID));
    
    unsigned bytes;
    f_open(&file, "memory.dmp", FA_WRITE | FA_CREATE_ALWAYS);
    f_write(&file, (void*)0x20000000, 0xC000, (UINT*)&bytes);
    return (f_close(&file) == FR_OK && bytes == 0xC000);
}

/* Stack parsing */

extern uint32_t _scode, _ecode, _estack;

// Find newest 4 code addresses on the stack, they are probably the
// previous functions on call stack.
void print_callers(uint32_t *sp)
{
    uint32_t callers[4] = {0};
    int i = 0;
    uint32_t *p = sp;
    while (i < 4 && p < &_estack)
    {
        if (*p >= (uint32_t)&_scode && *p <= (uint32_t)&_ecode)
            callers[i++] = *p;
        
        p++;
    }
    fprintf(stderr, "Callers: %08x %08x %08x %08x\n",
            callers[0], callers[1], callers[2], callers[3]);
}

/* Soft crashes (asserts) */

// Message is free-form error message,
// caller is the value from __builtin_return_address(0).
void crash_with_message(const char *message, void *caller)
{
    __Clear_Screen(0b0000000000011111);
    __Set(BEEP_VOLUME, 0);
    
    fprintf(stderr, "   %s   \n", message);
    fprintf(stderr, "Caller: %08x\n", (uint32_t)caller);
    
    print_callers((uint32_t*)__builtin_frame_address(0));
    
    if (make_memdump())
        fprintf(stderr, "Memory dump saved to memory.dmp.\n");
    else
        fprintf(stderr, "Memory dump failed.\n");
    
    while(1);
}

void __assert_fail(const char *assert, const char *file, unsigned int line)
{
    char tmp[50];
    snprintf(tmp, 50, "assert failed: %s:%d: %s", file, line, assert);
    crash_with_message(tmp, __builtin_return_address(0));
}

/* Hard crashes (invalid memory access etc.) */

// These are separate from the memory dump stuff because they are collected
// at a different time, i.e. they are not necessarily coherent with the dump.
static uint32_t *SP;
static uint32_t R4, R5, R6, R7, R8, R9, R10, R11;

void __irq__ print_hardfault()
{
    __Clear_Screen(0b0000000000011111);
    __Set(BEEP_VOLUME, 0);
    
    
    fprintf(stderr, "\n\n   HARDFAULT   \n");
    fprintf(stderr, "SP:%08x PC:%08x LR:%08x\n",
            (uint32_t)SP, *(SP + 6), *(SP + 5));
    fprintf(stderr, "SCB HFSR:%08x CFSR:%08x BFAR:%08x\n\n",
            SCB->HFSR, SCB->CFSR, SCB->BFAR);
    
    fprintf(stderr, "R0:%08x R1:%08x R2:%08x R3:%08x\n",
            SP[0], SP[1], SP[2], SP[3]);
    fprintf(stderr, "R4:%08x R5:%08x R6:%08x R7:%08x\n",
            R4, R5, R6, R7);
    fprintf(stderr, "R8:%08x R9:%08x\n",
            R8, R9);
    fprintf(stderr, "R10:%08x R11:%08x R12:%08x\n\n", R10, R11, *(SP + 4));
    
    
    print_callers(SP + 7);
    
    if (make_memdump())
        fprintf(stderr, "Memory dump saved to memory.dmp.\n");
    else
        fprintf(stderr, "Memory dump failed.\n");
    while(1);
}

void __attribute__((naked)) HardFaultException()
{
    // Rescue stack pointer and register values
    asm("mrs %0, msp" : "=r"(SP) : :);
    asm("str r4, %0" : "=m"(R4) : :);
    asm("str r5, %0" : "=m"(R5) : :);
    asm("str r6, %0" : "=m"(R6) : :);
    asm("str r7, %0" : "=m"(R7) : :);
    asm("str r8, %0" : "=m"(R8) : :);
    asm("str r9, %0" : "=m"(R9) : :);
    asm("str r10, %0" : "=m"(R10) : :);
    asm("str r11, %0" : "=m"(R11) : :);
    
    // Print message to screen and halt.
    print_hardfault();
}



#undef fprintf
#undef snprintf
