#/bin/sh

arduino-cli compile --fqbn arduino:avr:mega && arduino-cli upload -v --fqbn arduino:avr:mega --port /dev/ttyUSB0 && screen /dev/ttyUSB0 9600
