@echo off
setlocal ENABLEDELAYEDEXPANSION

rem DS203 Win32 GCC support by valky.eu
rem USER DEFINED VALUES
rem ===================================================
set CBASE=C:\Programs\Devel\Gcc\arm-2011.03\bin\
set TARGET=F:\
set TFILE=APP
set APP=2
rem ===================================================

set CROSS=!CBASE!arm-none-eabi-
set CC=!CROSS!gcc
set OBJCOPY=!CROSS!objcopy
set LD=!CROSS!ld
set AS=!CROSS!as
set STRIP=!CROSS!strip
set READELF=!CROSS!readelf

if not exist !CC!.* (
  echo Compiler not found !
  goto :eof
)

set CFLAGS=-Wall -Os -fno-common -mcpu=cortex-m3 -mthumb -msoft-float -MD -std=c99
set AFLAGS=-mcpu=cortex-m3 -mthumb
set LDFLAGS=-nostartfiles -mcpu=cortex-m3 -mthumb -march=armv7 -mfix-cortex-m3-ldrd -msoft-float 

rmdir bin /s /q 2> nul
mkdir bin
cd bin

!CC! !CFLAGS! -c ../main.c ../gbios.c
!CC! -o !TFILE!_!APP!.elf !LDFLAGS! -T ../app!APP!main.lds main.o gbios.o

!OBJCOPY! -O ihex !TFILE!_!APP!.elf !TFILE!_!APP!.hex
!OBJCOPY! -I ihex -O elf32-littlearm !TFILE!_!APP!.hex !TFILE!_!APP!z.elf

!readelf! -all !TFILE!_!APP!.elf  > log!APP!.txt 
!readelf! -all !TFILE!_!APP!z.elf  > log!APP!z.txt 
