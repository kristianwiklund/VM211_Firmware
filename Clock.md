Documentation for the alarm clock feature
=========================================

The alarm clock functionality is built around two main components: 

1. (Implemented) Real time chip. This is required to display the clock. Currently the DS1307 is supported (that is what I had around)
2. (Not Implemented yet). ESP-01 (or other ESP) with 2.x AT firmware. https://github.com/espressif/esp-at/releases/tag/v2.1.0.0_esp8266 . The 2.x firmware is needed for mqtt support out of the box.
