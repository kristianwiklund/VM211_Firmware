#/bin/sh

arduino-cli compile --fqbn arduino:avr:mega && arduino-cli upload -v --fqbn arduino:avr:mega --port /dev/ttyACM0 && screen /dev/ttyACM0 9600
