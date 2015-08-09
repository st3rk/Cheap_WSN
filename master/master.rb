#!/usr/bin/env ruby
# encoding: UTF-8

require 'serialport'

port_str = '/dev/ttyS1'
baud_rate = 9600
data_bits = 8
stop_bits = 1
parity = SerialPort::NONE

dummy_start = [169, 244, 190]
dummy_end = [190, 244, 169]
window_size = 12

sp = SerialPort.new(port_str, baud_rate, data_bits, stop_bits, parity)

data_window = Array.new

loop do
	byte = sp.getc
	
	data_window.push(byte.bytes.to_a[0])
	data_window.shift if data_window.count > window_size

	data_start = Array.new(data_window)
	data_start.pop while data_start.count > dummy_start.count

	data_end = Array.new(data_window)
	data_end.shift while data_end.count > dummy_end.count

	if data_start.eql?(dummy_start) and data_end.eql?(dummy_end)
		valid_data = true
		if data_window[3] + data_window[4] == 254
			humidity = data_window[3]
			puts "humidity ok"
		else
			humidity = nil
		end
		if data_window[5] + data_window[6] == 255
			temperature = data_window[5] - 25
			puts "temp ok"
		else
			temperature = nil
		end
		if data_window[7] + data_window[8] == 255
			puts "redund ok"
			if temperature == nil and humidity != nil
				puts "compute correct temp"
				temperature = data_window[7] - humidity - 25
			elsif temperature != nil and humidity == nil
				puts "compute correct hum"
				humidity = data_window[7] - temperature - 25
			end
		end
		p data_window[7]
		p data_window[8]
		if temperature != nil and humidity != nil
			sp.putc(69)
			puts "température: #{temperature}°C, humidité: #{humidity}%"
		end
	end
end
