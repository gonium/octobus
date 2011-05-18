#!/bin/sh
NAME=ravenlcd_3290
# We need to extract stuff from the elf binary.
avr-objcopy -R .eeprom -R .fuse -R .signature -O ihex $NAME.elf $NAME.hex

echo "Make sure the fuses are set to:"
avr-objdump -d -S -j .fuse $NAME.elf

#avrdude  -c jtag2 -P usb -p m3290p -U flash:w:$NAME.hex
#avrdude  -c usbasp -p m3290p -U flash:w:$NAME.hex
avrdude  -c buspirate -P /dev/cu.usbserial-A700eEaD -p m3290p -U flash:w:$NAME.hex
echo "NOT programming the eeprom."
#avrdude  -c usbasp -p m3290p -U eeprom:w:$NAME.eep

