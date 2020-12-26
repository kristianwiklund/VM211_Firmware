all:
	arduino-cli compile --fqbn arduino:avr:mega
upload:
	./upload.sh

clean:
	rm -rf build
	rm *~
