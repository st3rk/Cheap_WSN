# Cheap_WSN

A simple program to interconnect a master computer with a slave, arduino based, wireless sensor.

## The slave

- Reads temperature and humidity from a DHT11 sensor.
- Sends the dummy bytes (for synchronization) and data bytes by RF using a serial to 433MHz transceiver.
- Waits for the ACK.
- Sleeps (power-down mode) during 8 seconds.

## The master

- Listens for dummy bytes sequence.
- Checks data integrity, repairs using redundancy bytes if needed.
- Sends the ACK and prints the result on standard output.

## Required

- On master side:
	- Ruby
	- [serialport](https://github.com/hparra/ruby-serialport) ruby gem

- On slave side:
	- [DHT sensor](https://github.com/adafruit/DHT-sensor-library.git) library
	- [LowPower](https://github.com/rocketscream/Low-Power.git) library
