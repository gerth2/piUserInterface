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
        
        self.closeCommandCounter = 0

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
            while(self.closeRequested() is False):
                
                rxBytes = self.ser.readline()
                rxTime = time.time()
                
                try:
                    line = rxBytes.decode('utf-8')
                except Exception as e:
                    print("Warning: error decoding bytes to string:" + str(e))
                    continue
                    
                
                if line != '':
                    lineWithTime = "[" + str(rxTime) + "]:" + line
                    # print(lineWithTime)
                    fp.write(lineWithTime)
                    vals = self.parseData(line, rxTime)
                    if(vals is not None):
                        self.pipe.send(vals)
        
        # We're all done, shut everything done
        self.ser.close()
        self.pipe.close()

    def parseData(self, serRxString, serRxTime):
        try:
            vals = serRxString.strip().split(",")
            if(len(vals) != NUM_EXPECTED_DATA_VALUES):
                print("WARNING malformed data from serial port: " + serRxString)
                return None
            vals.insert(0,str(serRxTime))
            vals = [float(i) for i in vals]
            return vals
        except Exception as e:
            print("Warning: serial data parse exception: "+ str(e))
            return None
            
            
    def closeRequested(self):
        self.closeCommandCounter += 1
        if(self.closeCommandCounter % 100 == 0):
            if(self.pipe.poll() > 0):
                rxCmd = self.pipe.recv()
                if("END" in rxCmd):
                    print("Serial interface process exiting")
                    return True
                else:
                    return False
            else:
                return False
            
        else:
            return False

                    
