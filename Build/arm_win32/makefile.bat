@echo off
setlocal ENABLEDELAYEDEXPANSION

rem DS203 Win32 GCC support by valky.eu ver 2.0
rem USER DEFINED VALUES
rem ===================================================
set CBASE=..\Toolchain\arm-2011.03-lite\bin\
set TFILE=GABOUI
set APP=1
rem ===================================================

Echo DS203 Build tool by valky.eu

echo Locating DFU drive...
FOR /F "eol=# tokens=1,* delims==" %%i in ('_identify.bat') do (
  set %%i=%%j
)

if "%TARGET%"=="" (
  echo Could not find the DFU drive.
  echo.
	set online=no
) else (
	set online=yes
)

Echo DFU Drive: !TARGET!

call :CheckPathSpaces "%CD%" %CD%

set CROSS=!CBASE!arm-none-eabi-
set CC=!CROSS!gcc
set CPP=!CROSS!g++
set OBJCOPY=!CROSS!objcopy
set LD=!CROSS!ld
set AS=!CROSS!as
set STRIP=!CROSS!strip

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

rem Revision macros cannot contain spaces!
call :CheckRevSpaces "%GIT_BUILDPC%" %GIT_BUILDPC%

cd ../../
set BIN=Bin
if not exist !BIN! (
  mkdir !BIN!
) else (
  rmdir /s /q !BIN!
  mkdir !BIN!
)

cd !BIN!

rem Verify compiler
if not exist !CC!.* (
  echo Compiler not found !
	echo.
	echo Current path: %CD%
  echo Toolchain path: %CBASE%
	echo Testing file: !CC!
  echo.
	echo Please download it from http://pub.valky.eu/arm-2011.03-lite.zip and place it to Toolchain folder
  goto :eof
)

echo Compiling...
!CC! !WIN32_ARM_GCC_AFLAGS! -c !ASM_SRC1! -o !ASM_OUT1!
!CC! !WIN32_ARM_GCC_AFLAGS! -c !ASM_SRC2! -o !ASM_OUT2!
!CC! !WIN32_ARM_GCC_CFLAGS! !WIN32_ARM_GCC_INCLUDES! -c !C_SRCS!
!CPP! !WIN32_ARM_GCC_GPPFLAGS! !WIN32_ARM_GCC_INCLUDES! !REVISION! -c !CPP_SRCS!

if "!online!"=="no" (
  echo Linking... 

  rem alternative slots
  !CC! -o !TFILE!_1.elf !WIN32_ARM_GCC_LDFLAGS! -T ../Source/HwLayer/ArmM3/lds/app1_win.lds !OBJS!
  !OBJCOPY! -O ihex !TFILE!_1.elf !TFILE!_1.hex
  !CC! -o !TFILE!_2.elf !WIN32_ARM_GCC_LDFLAGS! -T ../Source/HwLayer/ArmM3/lds/app2_win.lds !OBJS!
  !OBJCOPY! -O ihex !TFILE!_2.elf !TFILE!_2.hex
  !CC! -o !TFILE!_3.elf !WIN32_ARM_GCC_LDFLAGS! -T ../Source/HwLayer/ArmM3/lds/app3_win.lds !OBJS!
  !OBJCOPY! -O ihex !TFILE!_3.elf !TFILE!_3.hex
  !CC! -o !TFILE!_4.elf !WIN32_ARM_GCC_LDFLAGS! -T ../Source/HwLayer/ArmM3/lds/app4_win.lds !OBJS!
  !OBJCOPY! -O ihex !TFILE!_4.elf !TFILE!_4.hex

  goto :eof
)

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

:CheckPathSpaces
if not %1=="%2" (
  echo.
  echo Your current path contains spaces ^(%1^), it can cause some problems...
  pause
)
goto :eof

:CheckRevSpaces
if not %1=="%2" (
  echo.
  echo Your computer name contains spaces ^(%1^), please modify _revision.bat so the GIT_BUILDPC will return string without spaces
  pause
)
goto :eof

