# this will be called from the folder /bin, relative path ".." refers to top level, where
# Source, Build, Lib folders are placed


# files 

OBJS := cbios.o Application.o Main.o stm32f10x_nvic.o cortexm3_macro.o interrupt.o startup.o Generator.o Graph.o Controls.o Settings.o Utils.o Wnd.o MainWnd.o MenuInput.o Toolbar.o MainMenu.o FFTCM3.o MenuSpectMain.o Calibration.o Meter.o Serialize.o
C_SRCS := ../Source/HwLayer/ArmM3/stm32f10x/src/stm32f10x_nvic.c ../Source/HwLayer/ArmM3/src/interrupt.c ../Source/HwLayer/ArmM3/src/startup.c
ASM_SRCS := ../Source/HwLayer/ArmM3/stm32f10x/asm/cortexm3_macro.s ../Source/HwLayer/ArmM3/src/BIOS.S ..\Source\HwLayer\ArmM3\bios\FFTCM3.S
CPP_SRCS := ../Source/HwLayer/ArmM3/src/main.cpp ../Source/HwLayer/ArmM3/src/cbios.cpp ../Source/Core/Controls.cpp ../Source/Core/Settings.cpp ../Source/Core/Utils.cpp ../Source/Framework/Wnd.cpp ../Source/Gui/Generator/Generator.cpp ../Source/Gui/Oscilloscope/Graph.cpp ../Source/Gui/MainWnd.cpp ../Source/Gui/Oscilloscope/Input/MenuInput.cpp ../Source/Main/Application.cpp ../Source/Gui/Toolbar.cpp ../Source/Gui/MainMenu.cpp ../Source/Gui/Spectrum/MenuSpectMain.cpp ../Source/Gui/User/Calibration.cpp ../Source/Gui/User/Meter.cpp ../Source/Core/Serialize.cpp



# building for ARM on Win32 platform

WIN32_ARM_GCC_CFLAGS := -Wall -Os -Werror -fno-common -mcpu=cortex-m3 -mthumb -msoft-float -MD
WIN32_ARM_GCC_AFLAGS := -mcpu=cortex-m3 -mthumb
WIN32_ARM_GCC_LDFLAGS := -nostartfiles -mcpu=cortex-m3 -mthumb -march=armv7 -mfix-cortex-m3-ldrd -msoft-float -lc -lgcc
WIN32_ARM_GCC_GPPFLAGS := -Wall -Os -Werror -fno-common -mcpu=cortex-m3 -mthumb -msoft-float -MD -D _ARM -fno-exceptions -fno-rtti -fno-threadsafe-statics -Wno-psabi
WIN32_ARM_GCC_INCLUDES := -I .. -I ../Source/HwLayer/ArmM3/stm32f10x/inc -I ../Source/HwLayer/ArmM3/src
# win32 batch file doesn't support string tokenizing
ASM_SRC1 := ../Source/HwLayer/ArmM3/stm32f10x/asm/cortexm3_macro.s
ASM_OUT1 := cortexm3_macro.o
ASM_SRC2 := ../Source/HwLayer/ArmM3/src/BIOS.S
ASM_OUT2 := bios.o
ASM_SRC3 := ../Source/HwLayer/ArmM3/bios/FFTCM3.S
ASM_OUT3 := FFTCM3.o


# building for ARM on Linux platform

LINUX_ARM_CFLAGS := -Wall -Os -Werror -fno-common -mcpu=cortex-m3 -mthumb -msoft-float -MD
LINUX_ARM_AFLAGS := -mcpu=cortex-m3 -mthumb
LINUX_ARM_LDFLAGS := -nostartfiles -mcpu=cortex-m3 -mthumb -march=armv7 -mfix-cortex-m3-ldrd -msoft-float -lc -lgcc
LINUX_ARM_INCLUDES := -I .. -I ../Source/HwLayer/ArmM3/stm32f10x/stm32f10x/inc -I ../Source/HwLayer/ArmM3/src
LINUX_ARM_GPPFLAGS := -Wall -Os -Werror -fno-common -mcpu=cortex-m3 -mthumb -msoft-float -MD -D _ARM -fno-exceptions -fno-rtti 