import os, sys
import serial
import time

LOG_NAME_PREFIX = "./serial_log"

ser = serial.Serial(
    port='/dev/ttyACM0',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

ser.isOpen()

tmp = ""

logName = LOG_NAME_PREFIX + time.strftime("%Y%m%d-%H%M%S") + ".txt"

with open(logName, "w") as fp:
	while 1:
		time.sleep(0.1)
		while ser.inWaiting() > 0:
			tmp += ser.read(1).decode("utf-8")

		if tmp != '':
			print(tmp)
			fp.write(tmp)
			
