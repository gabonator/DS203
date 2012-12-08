Making your device wireless
---------------------------

Adding serial port
------------------

In this tutorial I will show, how to attach a Bluetooth module to your DS203. At first we will drill small rectangular hole to the device casing and put there small 4 pin header. This will provide power for the bluetooth module and communication lines (RX, TX). On the main board, there is already a preparation for this task - a connector CN7 (higlighted in red)

!(images/board_h.jpg)

From the schematics, we see that this connector has all the necessary signals we will need (4:RX, 5:TX, 6:GND), the only problem is 2.8volt power supply which is too low to power the bluetooth module:

!(images/schem_cn7.png)

Let's have a look on the device schematics and find suitable power line. We will need voltage between 3.3V and 5V:

!(images/schem_power.png)

This picture shows the power regulating circuit with USB on left, battery charge controller in middle and 2.8V regulator on right. The internal battery perfectly fits our needs, it has 3.6V and the power switch connects the battery to the regulator U15. We do not want to the voltage be present all the time on our connector, nor when the device is turned off. So the best choice is to take the voltage directly from power switch (marked in red), it's pins are big and easy to solder a wire on them.

!(images/photo_1.jpg)

On this picture we can see how is the wire passd through the CN7 connector to the opposite side of board.

!(images/photo_2.jpg)

Here is the 4 pin socket connector in final position with sticky tape to prevent short circuit with LCD frame.

!(images/photo_3.jpg)

You will need to cut the protrusions of the connector to make it fit the small space between plastic casing and LCD frame.

!(images/photo_4.jpg)

Here is the connector I used - RM2 1x9 pins.

!(images/photo_5.jpg)

Small rectangular hole was cut into plastic casing

!(images/photo_6.jpg)

And voila, this is how it looks when it's assembled.

!(images/photo_7.jpg)

The connector perfectly sits on its place without any protrusions.

!(images/photo_8.jpg)


Bluetooth module
----------------
I have decided to use bluetooth module OBS421 from company ConnectBlue (http://www.connectblue.com) based in Sweden. Their devices are easy to integrate and they provide very nice desktop utility which can be used to configure their devices on air without any programmer or UART cable.
I buy these devices from company spezial-electronic (http://www.spezial.com), when you will be placing your order, ask them to configure the devices with "allow configuration over air" option. By doing this, you only need to connect the module to power supply and all configuration can be done over air with their utility. Otherwise you will need USB-to-UART cable with hardware flow control, you probably already own some sort of USB to UART cable, but you will need cable that has RTS/CTS (cheap converters usually have only RX/TX lines). The hardware flow control is the default uart configuration after device reset, but with the configuration utility it can be disabled. Hardware reset of this device is the only option when you forget the pin code. So do not forget to note it down somewhere. 

!(images/obs421.jpg)

On the bottom side, there are many soldering pads forming three connectors. Its up to the designer which pins will use, the same signals present on the J6 connector are also present on J2. I used following pads for connection:

!(images/obs421pins.png)

The RX pin is used for serial reception no the bluetooth module and should be connected to the TX pin of DS203, TX pin on bluetooth module should be connected to RX pin (do not laugh, there are always many people who do not understand this).

I placed small 4 pin header on the bottom of this bluetooth module, connected to suitable pins and fixed with melted glue.

!(images/photo_9.jpg)

From the top it looks better :)

!(images/photo_10.jpg)

To prevent damaging the module (or oscilloscope) by incorrect connection I marked GND and VCC pins with permanent marker. You should be always careful when connecting this module, by reversing VCC/GND lines you can damage the module. When you are not sure, check the temperature of OBS module, I noticed that it heats up when it is not connected properly :)

And the final photos of attached module:

!(images/photo_11.jpg)

!(images/photo_12.jpg)



Configuring the module
----------------------

We will need to configure the OBS module to set it a name, device class number, pairing pin code and uart settings. All these parameters can be configured with "Serial Port Adapter Toolbox" utility which can be obtained from connectBlue webpage.

From your computer, pair with the bluetooth module (devices and printers->add device), check the port number and open the utility. You should be able to connect to it wirelessly by clicking the Connect button and selecting the port number. You don't need to change the baudrate settings (you need to set them only when using UART cable). 

!(images/toolbox_0.png)

Click on Read All button, set the Local Name and Class of device to 0x8050C. This is important, because some android devices will refuse to work with the module when the class of device is not properly set. Here is a nice online generator for COD: http://bluetooth-pentest.narod.ru/software/bluetooth_class_of_device-service_generator.html

!(images/toolbox_1.png)

In the Security tab, select Enabled and set some simple pin code. I noticed that android phone refuses to use SPP (serial port profile) when there is no paring code...

!(images/toolbox_2.png)

The DS203 has its serial port configured to 115200 bauds, 8 bits, no parity. Set these parameters in Serial tab and click Write. These values are applied after a power cycle, so turn off and then on your DS203.


Android application
-------------------

In your android phone settings, add and pair with the OBS module. Download and install the DsoQuad.apk from this GIT repository, click Connect, select your bluetooth device and wait until a message saying whether the connection was established or not. Then you can click one of the buttons on bottom to check the communication.

Working connection with android tablet:
!(images/photo_tablet.jpg)

