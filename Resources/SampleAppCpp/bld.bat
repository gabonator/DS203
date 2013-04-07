@echo off
setlocal ENABLEDELAYEDEXPANSION

rem DS203 Win32 GCC support by valky.eu
rem USER DEFINED VALUES
rem ===================================================
set CBASE=C:\Programs\Devel\Gcc\arm-2011.03\bin\
set TARGET=F:\
set TFILE=SNAKE
set APP=R
rem ===================================================

set CROSS=!CBASE!arm-none-eabi-
set CC=!CROSS!gcc
set CPP=!CROSS!g++
set OBJCOPY=!CROSS!objcopy
set LD=!CROSS!ld
set AS=!CROSS!as
set STRIP=!CROSS!strip
set READELF=!CROSS!readelf

if not exist !CC!.* (
  echo Compiler not found !
  goto :eof
)

set CPPFLAGS=-Wall -Os -Werror -fno-common -mcpu=cortex-m3 -mthumb -msoft-float -MD -D _ARM -fno-exceptions -fno-rtti -fno-threadsafe-statics -Wno-psabi -I../../../  -Wno-return-type
set LDFLAGS=-nostartfiles -mcpu=cortex-m3 -mthumb -march=armv7-m -mfix-cortex-m3-ldrd -msoft-float -lc -lgcc -Wl,-emain

rmdir bin /s /q 2> nul
mkdir bin
cd bin

!CPP! !CPPFLAGS! -c ../../../Source/Framework/Wnd.cpp ../../../Source/Core/Settings.cpp ../../../Source/Core/Utils.cpp ../Application.cpp ../Main.cpp ../gbios.cpp
!CC! -o !TFILE!_!APP!.elf !LDFLAGS! -T ../app!APP!main.lds main.o gbios.o Application.o Wnd.o
!STRIP! -x -s --strip-unneeded --keep-symbol=_estack --remove-section=.ARM.attributes --remove-section=.comment !TFILE!_!APP!.elf -o !TFILE!_!APP!s.elf

..\..\stripelf\stripelf.exe !TFILE!_!APP!s.elf !TFILE!_!APP!m.elf

!readelf! -all !TFILE!_!APP!m.elf
