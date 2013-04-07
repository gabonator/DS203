Flash images provided by JPA,
use stm32flash.exe for recovering devices that are unable to boot

stm32flash -w dso_quad_bootloader_flash.bin COM1


on connector CN7 connect pin 2 (BT0) to pin1 (VCC), and use USB to UART converter connected to pins 4 (RX), 5 (TX) and 6 (GND)