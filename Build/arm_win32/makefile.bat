@echo off
setlocal ENABLEDELAYEDEXPANSION

rem DS203 Win32 GCC support by valky.eu ver 2.0
rem USER DEFINED VALUES
rem ===================================================
set CBASE=C:\Programs\Devel\Gcc\arm-2011.03\bin\
set TFILE=APP_G251
set APP=1
rem ===================================================

Echo DS203 Build tool by valky.eu

echo Locating DFU drive...
FOR /F "eol=# tokens=1,* delims==" %%i in ('_identify.bat') do (
  set %%i=%%j
)

if "%TARGET%"=="" (
  echo Could not find the DFU drive.
  echo Please turn on your DSO while holding the first button and then connect it to your computer
  echo.
  rem echo Press any key
  rem pause > nul
  rem goto :eof
)

Echo DFU Drive: !TARGET!

call :CheckSpaces "%CD%" %CD%

set CROSS=!CBASE!arm-none-eabi-
set CC=!CROSS!gcc
set CPP=!CROSS!g++
set OBJCOPY=!CROSS!objcopy
set LD=!CROSS!ld
set AS=!CROSS!as
set STRIP=!CROSS!strip

if not exist !CC!.* (
  echo Compiler not found !
  goto :eof
)

FOR /F "eol=# tokens=1,* delims=:= " %%i in (../common/build.mk) do (
  set %%i=%%j
)

echo Getting GIT revision...
FOR /F "eol=# tokens=1,* delims==" %%i in ('_revision.bat') do (
  set %%i=%%j
)

if NOT "%GIT_REVNUMBER%"=="" (
  set D1=-DGIT_REVISION=%GIT_REVNUMBER%
) else (
  set D1=
)

if NOT "%GIT_REVHASH%"=="" (
  set D2=-DGIT_HASH=\"%GIT_REVHASH%\"
) else (
  set D2=
)

if NOT "%GIT_BUILDPC%"=="" (
  set D3=-DGIT_BUILDER=\"%GIT_BUILDPC%\"
) else (
  set D3=
)

set REVISION=%D1% %D2% %D3%

cd ../../
set BIN=Bin
if not exist !BIN! (
  mkdir !BIN!
) else (
  rmdir /s /q !BIN!
  mkdir !BIN!
)

cd !BIN!
rem goto link

echo Compiling...
!CC! !WIN32_ARM_GCC_AFLAGS! -c !ASM_SRC1! -o !ASM_OUT1!
!CC! !WIN32_ARM_GCC_AFLAGS! -c !ASM_SRC2! -o !ASM_OUT2!
!CC! !WIN32_ARM_GCC_CFLAGS! !WIN32_ARM_GCC_INCLUDES! -c !C_SRCS!
!CPP! !WIN32_ARM_GCC_GPPFLAGS! !WIN32_ARM_GCC_INCLUDES! !REVISION! -c !CPP_SRCS!
:link
echo Linking...
!CC! -o !TFILE!_!APP!.elf !WIN32_ARM_GCC_LDFLAGS! -T ../Source/HwLayer/ArmM3/lds/app!APP!_win.lds !OBJS!
if not exist !TFILE!_!APP!.elf (
  echo Build failed
  goto :eof
)
!OBJCOPY! -O binary !TFILE!_!APP!.elf !TFILE!.bin
!OBJCOPY! -O ihex !TFILE!_!APP!.elf !TFILE!.hex
rem ..\Build\arm_win32\elfstrip.exe !TFILE!_!APP!.elf !TFILE!_clean.elf

if not exist !TFILE!.hex (
  echo Build failed
  goto :eof
) else (
  echo Build succeed !
)

if not exist !TARGET! (
  echo Target device not ready
  goto :eof
)

if exist !TARGET!\*.WPT (
  echo Please start the device in DFU mode
  echo ^(Hold the first button while powering on^)
  goto :eof
)

Echo Downloading...
copy !TFILE!.hex !TARGET!!TFILE!.hex > nul 2> nul
rem dir !TARGET! > nul

rem del *.elf *.hex *.bin
Echo Waiting for the device...
:loop
if exist !TARGET!!TFILE!.not (
  echo Failed to download
  goto :eof
)
if exist !TARGET!!TFILE!.rdy (
  echo Download ok
  goto :eof
)
goto loop

:CheckSpaces
if not %1=="%2" (
  echo.
  echo Your current path contains spaces, it can cause some problems...
  pause
)
goto :eof

