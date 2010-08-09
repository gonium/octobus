#!/bin/sh
NAME=webserver6
# We need to extract stuff from the elf binary.
echo "Extracting memory sections from ELF"
avr-objcopy -R .eeprom -R .fuse -R .signature -O ihex $NAME.elf $NAME.hex
avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" \
  --change-section-lma .eeprom=0 -O ihex $NAME.elf $NAME.eep

echo "Make sure the fuses are set to:"
avr-objdump -d -S -j .fuse $NAME.elf

echo "Writing flash, without erasing the chip (preserve EEPROM with IP address."
avrdude  -c jtag2 -P usb -p m1284p -U flash:w:$NAME.hex 
#echo "NOT programming the eeprom."
sleep 5
echo "Writing eeprom."
avrdude  -c jtag2 -P usb -p m1284p -U eeprom:w:$NAME.eep

