DS203 customized firmware by Gabriel Valky
======================

DS203 is a nice open source oscilloscope. And this is an attempt to design improved GUI for the scope offering rich functions, better than those provided by original not-so-user-friendly firmware. The source code is in C++ for better code organisation and it is possible to compile it on Windows and Linux machine. For speeding up the development process, you can compile this application as native Win32 app, this offers you the ability to test new features on PC before compiling and uploading the firmware onto DS203 device. For windows users, there is a Visual Studio 2010 project file (Build/desktop_win32). For compiling the firmware for ARM Cortex M3 target device, there is a script in "Build" folder (arm_win32 for win32 platform, arm_linux for linux platform)

For owners of DS203 who would like to try this firmware:

1. download hex file from Bin/APP_G251.hex
2. power on your oscilloscope while holding the first button
3. on device's display, there should be displayed "DFU tool" and a new USB disk should appear in "My computer"
4. copy the hex file to your oscilloscope
5. after successful upload, on the USB disk will disconnect itself and reconnect again. Sometimes it will show some error message during uploading (don't worry, the computer has probably uploaded the file successfully, there is probably some bug in the DFU program) 
6. turn off
7. turn on without holding any button

For developers that want to build the firmware, but don't want to use github:

1. download whole repository: https://github.com/gabonator/DS203/zipball/master
2. follow instructions in /Man/ folder

For developers that would like to contribute to this project:
Just contact me, or make a modification of the source code and push me a request.

Controls:
======================
The DS203 device has 4 push buttons and two slide buttons. The function of first three buttons is following:

- PlayPause button - Enter
- Square button - Escape
- Circle button - Show toolbox
- Triagle button - User assignable button
- The two slider buttons are used for navigation and value change.

Screenshots:
======================
![Image 1](/gabonator/DS203/raw/master/Man/Screenshot/uart_decoder_1.png)

![Image 2](/gabonator/DS203/raw/master/Man/Screenshot/midi.png)

![Image 3](/gabonator/DS203/raw/master/Man/Screenshot/cursor.png)
           
![Image 4](/gabonator/DS203/raw/master/Man/Screenshot/waveman_meas.png)

![Image 5](/gabonator/DS203/raw/master/Man/Screenshot/spectrum.png)

![Image 6](/gabonator/DS203/raw/master/Man/Screenshot/about.png)

![Image 7](/gabonator/DS203/raw/master/Man/Screenshot/calibration.png)

![Image 12](/gabonator/DS203/raw/master/Man/Screenshot/envelope.png)

![Image 8](/gabonator/DS203/raw/master/Man/Screenshot/svgexport.png)

![Image 9](/gabonator/DS203/raw/master/Man/Screenshot/wave_export_ac.png)

![Image 10](/gabonator/DS203/raw/master/Man/Screenshot/tuner.png)

![Image 11](/gabonator/DS203/raw/master/Man/Screenshot/android.jpg)

![Image 12](/gabonator/DS203/raw/master/Man/Screenshot/pid1.png)

![Image 13](/gabonator/DS203/raw/master/Man/Screenshot/pid3.png)

![Image 14](/gabonator/DS203/raw/master/Man/Screenshot/snake.png)

![Image 15](/gabonator/DS203/raw/master/Man/Screenshot/sincinterpolator.png)

Feature highlights:
======================

- UART, MIDI, CANBUS decoder
- Mask tests, pass/fail calculations (create envelope of your signal and catch disturbances which exceed this envelope)
- Advanced mathematical operators (3 operands)
- User defined FIR filter (in Oscilloscope/Math)
- Cursors with lock-to-channel function
- Spectral analysis
- PID Controller
- Export to Wave (use your favourite sound editing program for signal examination)
- Export to CSV file
- Export to interactive SVG
- Chromatic tuner utility with very precise frequency counter
- Remote control with android device over UART/bluetooth (or with DsoControl application)
- Touchscreen support
- Serial port monitor with selectable baud rate
- Snake game :)

Todo list:
======================
- finish oscilloscope module - 92% done
- calibration - 80% done
- fft view - 80% done
- signal generator - 90% done
- connection with android tablet/phone - 90% done
- optimize ROM usage (many bitmaps are stored as 8bits per pixel) - 0%

Calibration:
======================

This firmware finally implements a simple method for analog channel calibration.  It's good idea to calibrate the DAC (wave out) first, it can be handy if you dont have regulated power supply. The device should be connected to a PC during calibration.
 - Calibrating Adc: 
   1. Go to User app/Calib tab, select Adc menu item
   2. Connect volt meter to Waveout
   3. Change the value (four digits) next to 0.5V label until you get exactly 0.5V on your voltmeter or multimeter.
   4. Do the same for 1.5V value
   5. Click on "Use labels", the dialog will hide
   6. Now you can check proper DAC calibration by changing the voltage shown on Adc menu item with navigation buttons, the voltage shown on display shoud match the voltage measured on the WaveOut connector within the error +-0.001V
 - Calibrating analog channel
   1. Enter "Analog-Simple" calibration dialog
   2. Select desired channel and resolution
   3. Connect the probe to selected channel and connect tip of the probe with the crocodile (ground).
   4. Select "Reset Vpos" and wait a few seconds
   5. Connect the probe to a precise power supply and by changing the K value, try to match Vin value with the real voltage of power supply. If you dont have any regulated PSU, you can connect the probe with WaveOut 
   6. Hit save and try changing the input voltage and check whether you get correct reading at the Vin value. Use a sensible voltage while matching the K value, for example - if you are calibrating 200mV range, use 4x200mV = 800mV at input.
 - After finishing, select "Save calib data"

Development:
======================

If you want to build this firmware with your own modules, follow the instructions in man/Readme.txt.

References:
======================

- Author's webpage - www.valky.eu
- Seeedstudio forums - http://www.seeedstudio.com/forum/viewforum.php?f=26
- DSO Quad wiki pages - http://www.seeedstudio.com/wiki/DSO_Quad

Alternative firmwares and useful resouces:
======================

 - Petteri Aimonen - http://essentialscrap.com/dsoquad/
 - Pedro Simoes - https://github.com/pmos69/dso203_gcc
 - Marco Sinatti - https://sites.google.com/site/marcosinatti/elettronica/dso

