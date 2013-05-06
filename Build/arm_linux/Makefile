TFILE := APP_M251
APP := 1

BASE_DIR := ../..
SRC_DIR := $(BASE_DIR)/Source

ASM_SRC1 := $(SRC_DIR)/HwLayer/ArmM3/stm32f10x/asm/cortexm3_macro.s
ASM_OUT1 := cortexm3_macro.o
ASM_SRC2 := $(SRC_DIR)/HwLayer/ArmM3/src/BIOS.S
ASM_OUT2 := BIOS.o


# building for ARM on Linux platform

LINUX_ARM_CFLAGS := -Wall -Os -Werror -fno-common -mcpu=cortex-m3 -mthumb -msoft-float -MD -D_VERSION2
LINUX_ARM_AFLAGS := -mcpu=cortex-m3 -mthumb
LINUX_ARM_LDFLAGS := -nostartfiles -mcpu=cortex-m3 -mthumb -march=armv7 -mfix-cortex-m3-ldrd -msoft-float -lc -lgcc
LINUX_ARM_INCLUDES := -I $(BASE_DIR) -I $(SRC_DIR)/HwLayer/ArmM3/stm32f10x/inc -I $(SRC_DIR)/HwLayer/ArmM3/src
LINUX_ARM_GPPFLAGS := -Wall -Os -fno-common -mcpu=cortex-m3 -mthumb -msoft-float -MD -D _ARM -fno-exceptions -fno-rtti -Wno-psabi  -D_VERSION2

OBJS= cbios.o Application.o Main.o stm32f10x_nvic.o cortexm3_macro.o interrupt.o startup.o GraphOsc.o Controls.o Settings.o Utils.o Wnd.o MainWnd.o MenuInput.o Toolbar.o MainMenu.o FFTCM3.o MenuSpectMain.o Calibration.o Serialize.o CalibAnalog.o CalibDac.o CalibMenu.o Calibration.o MenuMarker.o ToolBox.o MenuMeas.o Manager.o ChannelMath.o MenuMath.o MenuDisp.o SpectrumGraph.o MenuSpectMarker.o Annot.o Export.o MenuGenMain.o MenuGenEdit.o MenuGenModulation.o CoreGenerator.o Import.o CoreOscilloscope.o FFT.o Shapes.o Statistics.o _Modules.o MenuMask.o

CROSS=arm-none-eabi-
CC=$(CROSS)gcc
CPP=$(CROSS)g++
OBJCOPY=$(CROSS)objcopy
LD=$(CROSS)ld
AS=$(CROSS)as

all: BIOS.o cortexm3_macro.o cbios.o Application.o Main.o stm32f10x_nvic.o cortexm3_macro.o interrupt.o startup.o GraphOsc.o Controls.o Settings.o Utils.o Wnd.o MainWnd.o MenuInput.o Toolbar.o MainMenu.o MenuSpectMain.o Calibration.o Serialize.o CalibAnalog.o CalibDac.o CalibMenu.o Calibration.o MenuMarker.o ToolBox.o MenuMeas.o Manager.o ChannelMath.o MenuMath.o MenuDisp.o SpectrumGraph.o MenuSpectMarker.o Annot.o Export.o MenuGenMain.o MenuGenEdit.o MenuGenModulation.o CoreGenerator.o Import.o CoreOscilloscope.o FFT.o Shapes.o Statistics.o _Modules.o MenuMask.o FirFilter.o APP_M251.hex

.PHONY: clean

clean:
	rm -rf *.o
	rm -rf *.d
	rm -rf *.hex
	rm -rf *.bin
	rm -rf *.elf

APP_M251.bin: APP_M251.elf
	$(OBJCOPY) -O binary APP_M251.elf APP_M251.bin

APP_M251.hex:APP_M251.elf
	$(OBJCOPY) -O ihex APP_M251.elf APP_M251.hex

APP_M251.elf: BIOS.o cortexm3_macro.o cbios.o Application.o Main.o stm32f10x_nvic.o cortexm3_macro.o interrupt.o startup.o GraphOsc.o Controls.o Settings.o Utils.o Wnd.o MainWnd.o MenuInput.o Toolbar.o MainMenu.o MenuSpectMain.o Calibration.o Serialize.o CalibAnalog.o CalibDac.o CalibMenu.o Calibration.o MenuMarker.o ToolBox.o MenuMeas.o Manager.o ChannelMath.o MenuMath.o MenuDisp.o SpectrumGraph.o MenuSpectMarker.o Annot.o Export.o MenuGenMain.o MenuGenEdit.o MenuGenModulation.o CoreGenerator.o Import.o CoreOscilloscope.o FFT.o Shapes.o Statistics.o _Modules.o MenuMask.o cbios.o Application.o Main.o stm32f10x_nvic.o cortexm3_macro.o interrupt.o startup.o GraphOsc.o Controls.o Settings.o Utils.o Wnd.o MainWnd.o MenuInput.o Toolbar.o MainMenu.o MenuSpectMain.o Calibration.o Serialize.o CalibAnalog.o CalibDac.o CalibMenu.o Calibration.o MenuMarker.o ToolBox.o MenuMeas.o Manager.o ChannelMath.o MenuMath.o MenuDisp.o SpectrumGraph.o MenuSpectMarker.o Annot.o Export.o MenuGenMain.o MenuGenEdit.o MenuGenModulation.o CoreGenerator.o Import.o CoreOscilloscope.o FFT.o Shapes.o Statistics.o _Modules.o MenuMask.o FirFilter.o waveram.o
	$(CC) -o APP_M251.elf $(LINUX_ARM_LDFLAGS) -T $(SRC_DIR)/HwLayer/ArmM3/lds/app1.lds cbios.o Application.o Main.o stm32f10x_nvic.o cortexm3_macro.o interrupt.o startup.o GraphOsc.o Controls.o Settings.o Utils.o Wnd.o MainWnd.o MenuInput.o Toolbar.o MainMenu.o MenuSpectMain.o Calibration.o Serialize.o CalibAnalog.o CalibDac.o CalibMenu.o Calibration.o MenuMarker.o ToolBox.o MenuMeas.o Manager.o ChannelMath.o MenuMath.o MenuDisp.o SpectrumGraph.o MenuSpectMarker.o Annot.o Export.o MenuGenMain.o MenuGenEdit.o MenuGenModulation.o CoreGenerator.o Import.o CoreOscilloscope.o FFT.o Shapes.o Statistics.o _Modules.o MenuMask.o BIOS.o FirFilter.o waveram.o

cortexm3_macro.o:
	$(CC) $(LINUX_ARM_AFLAGS) -c $(ASM_SRC1) -o $(ASM_OUT1)

BIOS.o:
	$(CC) $(LINUX_ARM_AFLAGS) -c $(ASM_SRC2) -o $(ASM_OUT2)

stm32f10x_nvic.o:
	$(CC) $(LINUX_ARM_CFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/HwLayer/ArmM3/stm32f10x/src/stm32f10x_nvic.c -o stm32f10x_nvic.o

interrupt.o:
	$(CC) $(LINUX_ARM_CFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/HwLayer/ArmM3/src/interrupt.c -o interrupt.o

startup.o:
	$(CC) $(LINUX_ARM_CFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/HwLayer/ArmM3/src/startup.c -o startup.o

waveram.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/HwLayer/ArmM3/src/waveram.c -o waveram.o

Main.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/HwLayer/ArmM3/src/main.cpp -o Main.o

cbios.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/HwLayer/ArmM3/src/cbios.cpp -o cbios.o

Controls.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Core/Controls.cpp -o Controls.o

Settings.o: $(SRC_DIR)/Core/Settings.cpp
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Core/Settings.cpp -o Settings.o
Utils.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Core/Utils.cpp -o  Utils.o
Wnd.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Framework/Wnd.cpp -o Wnd.o
MenuGenMain.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Generator/Main/MenuGenMain.cpp -o MenuGenMain.o
CoreGenerator.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Generator/Core/CoreGenerator.cpp -o CoreGenerator.o
MenuGenEdit.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Generator/Edit/MenuGenEdit.cpp -o MenuGenEdit.o
MenuGenModulation.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Generator/Modulation/MenuGenModulation.cpp -o MenuGenModulation.o
GraphOsc.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Oscilloscope/Controls/GraphOsc.cpp -o GraphOsc.o
MenuMarker.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Oscilloscope/Marker/MenuMarker.cpp -o MenuMarker.o
MainWnd.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) -fno-threadsafe-statics $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/MainWnd.cpp -o MainWnd.o
MenuInput.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Oscilloscope/Input/MenuInput.cpp -o MenuInput.o
Application.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) -fno-threadsafe-statics $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Main/Application.cpp -o Application.o
Toolbar.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) -fno-threadsafe-statics $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Toolbar.cpp -o Toolbar.o
MainMenu.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) -fno-threadsafe-statics $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/MainMenu.cpp -o MainMenu.o
MenuSpectMain.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Spectrum/Main/MenuSpectMain.cpp -o MenuSpectMain.o
Serialize.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Core/Serialize.cpp -o Serialize.o
CalibAnalog.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Calibration/CalibAnalog.cpp -o CalibAnalog.o
CalibDac.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Calibration/CalibDac.cpp -o CalibDac.o
CalibMenu.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Calibration/CalibMenu.cpp -o CalibMenu.o
Calibration.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Calibration/Calibration.cpp -o Calibration.o
ToolBox.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/ToolBox/ToolBox.cpp -o ToolBox.o
Import.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/ToolBox/Import.cpp -o Import.o
MenuMeas.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Oscilloscope/Meas/MenuMeas.cpp -o MenuMeas.o
Statistics.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Oscilloscope/Meas/Statistics.cpp -o Statistics.o
Manager.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/ToolBox/Manager.cpp -o Manager.o
FirFilter.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Oscilloscope/Math/FirFilter.cpp -o FirFilter.o
ChannelMath.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Oscilloscope/Math/ChannelMath.cpp -o ChannelMath.o
MenuMath.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Oscilloscope/Math/MenuMath.cpp -o MenuMath.o
MenuDisp.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Oscilloscope/Disp/MenuDisp.cpp -o MenuDisp.o
SpectrumGraph.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Spectrum/Controls/SpectrumGraph.cpp -o SpectrumGraph.o
MenuSpectMarker.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Spectrum/Marker/MenuSpectMarker.cpp -o MenuSpectMarker.o
Annot.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Spectrum/Controls/Annot.cpp -o Annot.o
Export.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/ToolBox/Export.cpp -o Export.o
CoreOscilloscope.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Oscilloscope/Core/CoreOscilloscope.cpp -o CoreOscilloscope.o
FFT.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Spectrum/Core/FFT.cpp -o FFT.o
Shapes.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Core/Shapes.cpp -o Shapes.o
_Modules.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/User/_Modules.cpp -o _Modules.o
MenuMask.o:
	$(CPP) $(LINUX_ARM_GPPFLAGS) $(LINUX_ARM_INCLUDES) -c $(SRC_DIR)/Gui/Oscilloscope/Mask/MenuMask.cpp -o MenuMask.o

.c.o:
	$(CC) $(LINUX_ARM_CFLAGS) $(LINUX_ARM_INCLUDES) -c -o $@ $*.c

.cpp.o:
	$(CPP) $(LINUX_ARM_CPPFLAGS) $(LINUX_ARM_INCLUDES) -c -o $@ $*.c

.S.o:
	$(CC) $(LINUX_ARM_AFLAGS) -c -o $@ $*.S

%.elf: %.lds $(OBJS)
	$(CC) -o $@ $(OBJS) $(LINUX_ARM_LDFLAGS) -T $<

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@
