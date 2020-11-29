To use the wifi-mqtt feature, you need to install the WiFiEspAt library - it is not included in the Velleman-legacy "src" directory.

This can be done using ```arduino-cli lib install wifiespat```, source and README is available here: https://github.com/jandrassy/WiFiEspAT
I'm assuming that those that are able to build this addon will be able to use the library manager in the Arduino IDE or the arduino-cli

Then you need to flash your ESP-01 to use espressif firmware 1.7.3, which is found here: https://www.espressif.com/en/support/download/at
Instructions for flashing is available in the WiFiEspAT README.

You need to connect the ESP01 to the Arduino through a level converter. You also need a 3.3V regulator connected to the 5V rail on the arduino,
the 3.3V arduino power is not good enough. 

The Wifi credentials are not set in the VM211 sketch, use the tools/setuppersistentwificonnection example from wifiespat to do that.
