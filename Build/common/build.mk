# this will be called from the folder /bin, relative path ".." refers to top level, where
# Source, Build, Lib folders are placed


# files 

OBJS := cbios.o Application.o Main.o stm32f10x_nvic.o cortexm3_macro.o interrupt.o startup.o GraphOsc.o Controls.o Settings.o Utils.o Wnd.o MainWnd.o MenuInput.o Toolbar.o MainMenu.o FFTCM3.o MenuSpectMain.o Calibration.o Meter.o Serialize.o CalibAnalog.o CalibDac.o CalibMenu.o Calibration.o MenuMarker.o ToolBox.o MenuMeas.o Manager.o ChannelMath.o MenuMath.o MenuDisp.o SpectrumGraph.o MenuSpectMarker.o Annot.o Export.o MenuGenMain.o MenuGenEdit.o MenuGenModulation.o CoreGenerator.o Import.o
C_SRCS := ../Source/HwLayer/ArmM3/stm32f10x/src/stm32f10x_nvic.c ../Source/HwLayer/ArmM3/src/interrupt.c ../Source/HwLayer/ArmM3/src/startup.c                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          	
ASM_SRCS := ../Source/HwLayer/ArmM3/stm32f10x/asm/cortexm3_macro.s ../Source/HwLayer/ArmM3/src/BIOS.S ..\Source\HwLayer\ArmM3\bios\FFTCM3.S
CPP_SRCS := ../Source/HwLayer/ArmM3/src/main.cpp ../Source/HwLayer/ArmM3/src/cbios.cpp ../Source/Core/Controls.cpp ../Source/Core/Settings.cpp ../Source/Core/Utils.cpp ../Source/Framework/Wnd.cpp ../Source/Gui/Generator/Main/MenuGenMain.cpp ../Source/Gui/Generator/Core/CoreGenerator.cpp ../Source/Gui/Generator/Edit/MenuGenEdit.cpp ../Source/Gui/Generator/Modulation/MenuGenModulation.cpp ../Source/Gui/Oscilloscope/Controls/GraphOsc.cpp ../Source/Gui/Oscilloscope/Marker/MenuMarker.cpp ../Source/Gui/MainWnd.cpp ../Source/Gui/Oscilloscope/Input/MenuInput.cpp ../Source/Main/Application.cpp ../Source/Gui/Toolbar.cpp ../Source/Gui/MainMenu.cpp ../Source/Gui/Spectrum/Main/MenuSpectMain.cpp ../Source/Gui/User/Meter.cpp ../Source/Core/Serialize.cpp ../Source/Gui/Calibration/CalibAnalog.cpp ../Source/Gui/Calibration/CalibDac.cpp ../Source/Gui/Calibration/CalibMenu.cpp ../Source/Gui/Calibration/Calibration.cpp ../Source/Gui/ToolBox/ToolBox.cpp ../Source/Gui/ToolBox/Import.cpp ../Source/Gui/Oscilloscope/Meas/MenuMeas.cpp ../Source/Gui/ToolBox/Manager.cpp ../Source/Gui/Oscilloscope/Math/ChannelMath.cpp ../Source/Gui/Oscilloscope/Math/MenuMath.cpp ../Source/Gui/Oscilloscope/Disp/MenuDisp.cpp ../Source/Gui/Spectrum/Controls/SpectrumGraph.cpp ../Source/Gui/Spectrum/Marker/MenuSpectMarker.cpp ../Source/Gui/Spectrum/Controls/Annot.cpp ../Source/Gui/Toolbox/Export.cpp



# building for ARM on Win32 platform

WIN32_ARM_GCC_CFLAGS := -Wall -Os -Werror -fno-common -mcpu=cortex-m3 -mthumb -msoft-float -MD
WIN32_ARM_GCC_AFLAGS := -mcpu=cortex-m3 -mthumb
WIN32_ARM_GCC_LDFLAGS := -nostartfiles -mcpu=cortex-m3 -mthumb -march=armv7 -mfix-cortex-m3-ldrd -msoft-float -lc -lgcc -Wl,-Map=myfile.map,--cref
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