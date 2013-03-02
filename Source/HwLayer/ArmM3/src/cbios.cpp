#include <Source/HwLayer/Types.h>
#include <Source/HwLayer/Bios.h>

extern "C" void __cxa_pure_virtual() { _ASSERT(!!!"Pure virtual call"); while(1);}

extern "C" {
#include "bios.h"
}

#include "../bios/core.h"
#include "../bios/font.h"
#include "../bios/lcd.h"
#include "../bios/key.h"
#include "../bios/adc.h"
#include "../bios/gen.h"
#include "../bios/files.h"
#include "../bios/serial.h"
#include "../bios/ver.h"
#include "../bios/mouse.h"
#include "../bios/gpio.h"
#include "../bios/flash.h"
#include "../bios/fat.h"

