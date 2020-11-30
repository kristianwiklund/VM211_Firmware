Requirements
============

To use the wifi-mqtt feature, you need to install the WiFiEspAt library - it is not included in the Velleman-legacy "src" directory.

This can be done using ```arduino-cli lib install wifiespat```, source and README is available here: https://github.com/jandrassy/WiFiEspAT
I'm assuming that those that are able to build this addon will be able to use the library manager in the Arduino IDE or the arduino-cli

Preparing the ESP-01 device
===========================

Then you need to flash your ESP-01 to use espressif firmware 1.7.3, which is found here: https://www.espressif.com/en/support/download/at
Instructions for flashing is available in the WiFiEspAT README.

The Wifi credentials are not set in the VM211 sketch, use the tools/setuppersistentwificonnection example from wifiespat to do that.

Connecting the ESP-01 to the arduino
====================================

You need to connect the ESP01 to the Arduino through a level converter. You also need a 3.3V regulator connected to the 5V rail on the arduino,
the 3.3V arduino power is not good enough. The ESP01 need to use serial3 (which is the only easily accessible port in the VM211)

Note to self: It is likely that the 5V power isn't good enough either, in that case, it has to be connected to the input power on the arduino

Configuration
=============

As mentioned above, wifi credentials are handled through the wifiespat tool example.
MQTT broker host is set in config.h
