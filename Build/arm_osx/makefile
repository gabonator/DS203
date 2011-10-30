#/bin/sh

echo ahoj!

-include ../common/build.mk

CROSS=/Users/gabrielvalky/Applications/Development/cm3/bin/arm-none-eabi-
CC=$(CROSS)gcc
CPP=$(CROSS)g++
OBJCOPY=$(CROSS)objcopy
LD=$(CROSS)ld
AS=$(CROSS)as

TARGETS=app1.hex app2.hex app3.hex

all: BIOS.o $(OBJS) $(TARGETS)

.PHONY: clean

clean:
	rm -rf $(OBJS) $(TARGETS)

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



