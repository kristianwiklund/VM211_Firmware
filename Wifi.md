To use the wifi-mqtt feature, you need to install the WiFiEspAt library - it is not included in the Velleman-legacy "src" directory.

This can be done using ```arduino-cli lib install wifiespat```, source and README is available here: https://github.com/jandrassy/WiFiEspAT


Then you need to flash your ESP-01 to use espressif firmware 1.7.3, which is found here: https://www.espressif.com/en/support/download/at
Instructions for flashing is available in the WiFiEspAT README.

