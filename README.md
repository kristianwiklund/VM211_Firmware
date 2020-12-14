Requirements
============

You need to install a few libraries to use this code, on Linux, you can install them by running the '''dependencies.sh''' script, if you have installed arduino-cli.
Otherwise, check the dependencies.sh file to find out which libs to install

Preparing the ESP-01 device
===========================

Then you need to flash your ESP-01 to use espressif firmware 1.7.3, which is found here: https://www.espressif.com/en/support/download/at
Instructions for flashing is available in the WiFiEspAT README.

The Wifi credentials are not set in the VM211 sketch, use the tools/setuppersistentwificonnection example from wifiespat to do that.

Connecting the ESP-01 to the arduino
====================================

You mustconnect the ESP01 to the Arduino through a level converter, 5V arduino side, 3.3V ESP side. 
You also need a 3.3V regulator connected to the power input port on the arduino to power the ESP, the 3.3V arduino power is not good enough and neither is the 5V power rail. 

The ESP01 need to use serial3 (which is the only easily accessible port in the VM211)

I soldered my esp in place, it was easier for me than to try to use the connectors. Please adjust to your own skill level as needed. 

Configuration
=============

As mentioned above, wifi credentials are handled through the wifiespat tool example.
MQTT broker host is set in config.h

Documentation for the alarm clock feature
=========================================

The alarm clock functionality is built around two main components: 

1. (Implemented) Real time chip. This is required to display the clock. Currently the DS1307 is supported (that is what I had around). This adds a ninth "screen" and a config.h file to set parameters. 

2. (Not Implemented Yet) mqtt control of the alarms

-------------------------------------------------------

Change log v4.0.4.kw
- Changed from Sparkfun BME280 library to Adafruit BME280 library.

Change log v4.0.3.kw
- Changed from Sparkfun CCS811 library to Adafruit CCS811 library in an attempt to get rid of crazy high CO2 readings. Seems to work better. Don't know why.
- Changed to use the boot animation as feedback, instead of blocking the boot, it now cycles its colors as part of wifi connection

Change log v4.0.2.kw
- refactored the CCS811 code in an attempt to get the correct readings. Code moved to separate c++ file.
- move from included CCS811 to use sparkfun's latest, install with arduino-cli lib install "sparkfun ccs811 arduino library"@2.0.1, I'm assuming that those who do this mod are competent to install libraries themselves
- changed the sensor read frequency to a less hysterical 10 seconds (configurable)

Change log v4.0.x.kw
- Wifi feature, using ESP-01. 
- Wifi feature connects to mqtt broker and transmits the measurements every 300 seconds. Topic vm211/# . Please note, currently, the mqtt feature do not check if the sensor exists before transmitting the value
- Refactored the screen drawing code into objects that can be enabled/disabled for example by autodetecting the hardware.
- Screens without any sensor will not be drawn
- Fixed the original source code to compile on linux

Change log v4.0-kw.1
- Adding functionality to be able to use the device as an alarm clock.
- Code to use a ds1307 for time and draw an analogue clock

Change log v4.0
- fixed some spelling mistakes
- checks now if Earthlistener is booted for the first time, if true, sets default values
- logging interval can now be set (1 second - 3600 seconds interval, default is now 10 seconds)

Change log v3.8
- modified setup page to icons instead of text
- lightning sensor sensitivity can now be set by icon on setup page (also stored in EEPROM, low by default)
- both IIC & SPI interfaces are available for the lightning sensor via setup page (also stored in EEPORM, SPI by default)
- indoor / outdoor setting is now changeable without reboot
- removed all refs to altitude screen (it wasn't used)
- optimized printing of the graphs, these are now 1 pixel in height instead of 18 pixels. Line is drawn 18x. Huge memory optimization!
- fixed some spelling mistakes

Change log v3.7
- changed lightning sensor library & interrupt function to change

Change log v3.6
- spelling mistakes fixed & extra comments added for better documentation
- changed interrupt function

---------------------------------------------------

EARTH LISTENER (VM211 & VM211SE)

Ever wondered how good the air quality is in your living room? Or do you need proof of why you fall asleep in a non-ventilated meeting room after 3 hours? Look no further! This DIY experimental kit has all the answers!
The Earth Listener is an Arduino based sensor device that reads multiple environmental values to indicate the air quality. This kit is assembled in 15 minutes and you don’t need a soldering iron! Just connect the parts and wires, put everything in the case and connect it to a 5V USB socket. The Mega development board is pre-programmed and shows eCO2, TVOC (particles in the air), barometric pressure, temperature and humidity values onto the TFT touchscreen. To top thing off, the Earth Listener also contains a lightning detection sensor. If lightning (or electronic discharge) is detected, the Earth Listener will give an alarm tone and log how far the lightning is located. Since everything is open source, you can add an additional 6 sensors to the Earth Listener via the sensor shield. Logging is possible via the micro SD card (not included).

Buy it here: https://www.vellemanformakers.com/earth-listener/
Follow the tutorial here: https://manuals.velleman.eu/category.php?id=378 

