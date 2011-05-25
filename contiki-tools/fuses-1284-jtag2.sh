#!/bin/sh
## We need to extract stuff from the elf binary.
#avr-objcopy -R .eeprom -R .fuse -R .signature -O ihex $NAME.elf $NAME.hex
#
#echo "Make sure the fuses are set to:"
#avr-objdump -d -S -j .fuse $NAME.elf
echo "Setting fuses via  JTAG2 and JTAG."
avrdude  -c jtag2 -P usb -p m1284p -U lfuse:w:0xE2:m -U hfuse:w:0x99:m -U efuse:w:0xFF:m
#echo "NOT programming the eeprom."
#avrdude  -c usbasp -p m3290p -U eeprom:w:$NAME.eep

