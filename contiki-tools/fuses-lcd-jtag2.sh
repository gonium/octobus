#!/bin/sh
#NAME=ravenlcd_3290
## We need to extract stuff from the elf binary.
#avr-objcopy -R .eeprom -R .fuse -R .signature -O ihex $NAME.elf $NAME.hex
#
#echo "Make sure the fuses are set to:"
#avr-objdump -d -S -j .fuse $NAME.elf
echo "Setting fuses via  JTAG2 and ISP."
avrdude  -c jtag2isp -P usb -p m3290p -U lfuse:w:0xE2:m -U hfuse:w:0x99:m -U efuse:w:0xFF:m
#echo "NOT programming the eeprom."
#avrdude  -c usbasp -p m3290p -U eeprom:w:$NAME.eep

