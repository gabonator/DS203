DS203 customized firmware by Gabriel Valky

DS203 is a nice open source oscilloscope. And this is an attempt to design improved GUI for the scope offering rich functions, better than those provided by original not-so-user-friendly firmware. The source code is in C++ for better code organisation and it is possible to compile it on Windows and Linux machine. For speeding up the development process, you can compile this application as native Win32 app, this offers you the ability to test new features on PC before compiling and uploading the firmware onto DS203 device. For windows users, there is a Visual Studio 2010 project file (Build/desktop_win32). For compiling the firmware for ARM Cortex M3 target device, there is a script in "Build" folder (arm_win32 for win32 platform, arm_linux for linux platform)

For owners of DS203 who would like to try this firmware:
1. download hex file from Bin/APP_G251.hex
2. power on your oscilloscope while holding the first button
3. on device's display, there should be displayed "DFU tool" and a new USB disk should appear in "My computer"
4. copy the hex file to your oscilloscope
5. after successful upload, on the USB disk will disconnect itself and reconnect again. Sometimes it will show some error message during uploading (don't worry, the computer has probably uploaded the file successfully, there is probably some bug in the DFU program) 
5. turn off
6. turn on without holding any button

For developers that want to build the firmware, but don't want to use github:
1. download whole repository: https://github.com/gabonator/DS203/zipball/master
2. follow instructions in /Man/ folder

For developers that would like to contribute to this project:
Just contact me, or make a modification of the source code and push me a request.

Controls:
The DS203 device has 4 push buttons and two slide buttons. The function of first three buttons are following:
 >||  Enter
 []   Escape
 ()   Show toolbox
The two slider buttons are used for navigation and value change.

Screenshots:
![Image 1](https://raw.github.com/gabonator/DS203/master/Man/Screenshot/uart_decoder_1.png)
![Image 2](https://raw.github.com/gabonator/DS203/master/Man/Screenshot/cursor.png)
![Image 3](https://raw.github.com/gabonator/DS203/master/Man/Screenshot/waveman_meas.png)

Feature highlights:
  UART decoder
  Advanced mathematical operators
  Cursors with lock-to-channel function
  Export to CSV file

References:
  Authors webpage - www.valky.eu
  Seeedstudio forums - http://www.seeedstudio.com/forum/viewtopic.php?f=10&t=2362
  DSO Quad wiki pages - http://www.seeedstudio.com/wiki/DSO_Quad
