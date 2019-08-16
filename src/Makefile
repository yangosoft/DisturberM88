all: blink.cpp
	avr-g++ -std=c++11 -Wall -g -Os -mmcu=atmega88 -c blink.cpp
	avr-g++ -std=c++11 -Wall -g -mmcu=atmega88 -o blink.elf blink.o
	avr-objcopy -j .text -j .data -O ihex blink.elf blink.hex
	avr-size blink.elf

flash: all
	sudo avrdude -c usbasp -p m88 -U flash:w:blink.hex

clean:
	rm -rf blink.o blink.elf blink.hex
