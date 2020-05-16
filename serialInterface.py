################################################################################
## COPYRIGHT 2020 Ventilators In Peoria (VIP)
## See license.txt in for more info
##
## THE SOFTWARE IS PROVIDED "AS IS". IT IS NOT FIT FOR ANY PARTICULAR PURPOSE,
## CONFORMS TO NO REGULATORY BODY REQUIREMENTS, AND IS GENERALLY NOT SAFE
## FOR ANY APPLICATION INVOLVING HUMAN LIFE. USE AT YOUR OWN RISK.
################################################################################

import os, sys
import serial
import time
import multiprocessing

LOG_NAME_PREFIX = "./serial_log"
NUM_EXPECTED_DATA_VALUES = 3

class SerialInterface:

    def __init__(self, pipe):

        self.pipe = pipe

        self.ser = serial.Serial(
            port='/dev/ttyACM0',
            baudrate=115200,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS
        )

        self.ser.isOpen()

    def run(self):
        tmp = ""
        logName = LOG_NAME_PREFIX + time.strftime("%Y%m%d-%H%M%S") + ".txt"

        with open(logName, "w") as fp:
            while 1:
                time.sleep(0.01)
                line = ser.readline()
                rxTime = time.time()
                if line != '':
                    lineWithTime = "[" + str(rxTime) + "]:" + line
                    print(lineWithTime)
                    fp.write(lineWithTime)
                    self.pipe.send(self.parseData(line, rxTime))

    def parseData(self, serRxString, serRxTime):
        vals = serRxString.split(',')
        if(len(vals) != NUM_EXPECTED_DATA_VALUES):
            print("WARNING malformed data from serial port: " + serRxString)
        return [str(serRxTime)] + vals

    def __del__(self):
        self.ser.close()
        self.pipe.close()
                    
