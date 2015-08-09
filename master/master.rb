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
window_size = 13

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
		mote_addr = data_window[3]
		# if mote address is 69, let's read humidity and temp
		if mote_addr == 69
			if data_window[4] + data_window[5] == 255
				humidity = data_window[4]
			else
				humidity = nil
				puts "Error in received humidity"
			end
			if data_window[6] + data_window[7] == 255
				temperature = data_window[6] - 25
			else
				temperature = nil
				puts "Error in received temperature"
			end
			if data_window[8] + data_window[9] == 255
				if temperature == nil and humidity != nil
					puts "Compute correct temperature"
					temperature = data_window[8] - humidity - 25
				elsif temperature != nil and humidity == nil
					puts "Error in received data, compute correct humidity"
					humidity = data_window[8] - temperature - 25
				end
			end
			# if data were correct, send an ACK and display the result
			if temperature != nil and humidity != nil
				sp.putc(mote_addr)
				puts "température: #{temperature}°C, humidité: #{humidity}%"
			else
				puts "data corrupted, wait for mote to resend"
			end
		end
	end
end
