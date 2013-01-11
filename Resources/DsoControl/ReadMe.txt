This application can be used for remote control of DSO through bluetooth or uart connection.
It passes the mouse and keyboard events to DSO evaluator.

The purpose of this program is testing the mouse/touchscreen control of user interface. While
I dont have any touchscreen which would fit the DSO screen, this is simple experiment which 
creates a small window application and this sends corresponding mouse events to the DSO.
By moving your PC's mouse you can control the small blinking cursor on DSO screen.

The commands sent over uart have following form, every command is ended with \n character:

  MAIN.Mouse(x_coord,y_coord,button_down);
  MAIN.Mouse(20,20,0);
  WND.Message(WND::WmKey, KEY::Left);


Dont forget to set up proper port number:

dscontrol.cpp:
	CComm() : CSyncSerialComm( "\\\\.\\COM11" )

For serial communication I used the first library I found on internet (SyncSerialComm).
This one is not very reliable and should be replaced with something better. I also experimented
with transferring the bitmap screen to PC, but it takes too long time. This could be improved
by rewriting the LCD routines, because currently the GetPixel function sets the pixel coordinates
and then transfers the pixel value. Setting the pixel coordinate could be omitted when the
X/Y increment registers are set to right value and the transfer speed would be doubled.