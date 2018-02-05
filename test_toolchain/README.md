These sources can be used to test your AVR toolchain with an Arduino board and environment.
Some more information can be found here: https://wiki.ubuntuusers.de/AVR/

To use the avrdude and Arduino ISP use the following command:
```
avrdude -p atmega328p -c arduino -P /dev/ttyACM2 -b 115200 -D -U flash:w:main.hex:i
```
or for Arduino Nano:
```
avrdude -p atmega328p -c arduino -P /dev/ttyUSB0 -b 57600 -D -U flash:w:main.hex:i
```
