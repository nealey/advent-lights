FQBN = digistump:avr:digispark-tiny
#FQBN = attiny:avr:ATtinyX5:cpu=attiny85,clock=internal8

install: advent-lights.ino
	arduino --upload --board $(FQBN) $@
