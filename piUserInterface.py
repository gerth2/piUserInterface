################################################################################
## COPYRIGHT 2020 Ventilators In Peoria (VIP)
## See license.txt in for more info
##
## THE SOFTWARE IS PROVIDED "AS IS". IT IS NOT FIT FOR ANY PARTICULAR PURPOSE,
## CONFORMS TO NO REGULATORY BODY REQUIREMENTS, AND IS GENERALLY NOT SAFE
## FOR ANY APPLICATION INVOLVING HUMAN LIFE. USE AT YOUR OWN RISK.
################################################################################


import tkinter as tk
from stripChart import StripChart
from serialInterface import SerialInterface
import time, math, multiprocessing

GUI_PEIRODIC_RATE_MS = 50




class MainGUI(tk.Tk):

    def __init__(self):

        # creating a pipe 
        self.pipeSerialInfConn, self.pipeGuiCon = multiprocessing.Pipe(duplex=True) 
    
        self.serInf = SerialInterface(self.pipeSerialInfConn)
        self.serInfProc = multiprocessing.Process(target=self.serInf.run)

    def initGuiObjects(self):
        self.chartTest1 = StripChart(self, "Test 1", "kPa", 600, 150,   "yellow", 10.0, 0, 6)
        self.chartTest2 = StripChart(self, "Test 2", "bpm", 600, 150,   "cyan", 10.0, 0, 6)
        self.chartTest3 = StripChart(self, "Test 3", "count", 600, 150, "magenta", 10.0, -20, 20)
        self.serInfProc.start()


    def guiPeriodicUpdate(self):
        curTime = time.time()

        while(self.pipeGuiCon.poll() > 0):
            # Serial reader process had sent (or is in the process of sending)
            #  a packet of serial data
            serRxData = self.pipeGuiCon.recv()
            self.chartTest1.addNewValue(serRxData(0), serRxData(1))
            self.chartTest2.addNewValue(serRxData(0), serRxData(2))
            self.chartTest3.addNewValue(serRxData(0), serRxData(3))

        
        #Sample Time Vectors
        # self.chartTest1.addNewValue(curTime, 3 + 3*math.sin(2*math.pi*0.25*curTime))
        # self.chartTest2.addNewValue(curTime, abs(6*math.sin(2*math.pi*0.87*curTime)))
        # self.chartTest3.addNewValue(curTime, 15*math.sin(2*math.pi*0.6*curTime))

        self.chartTest1.update(curTime)
        self.chartTest2.update(curTime)
        self.chartTest3.update(curTime)

        self.after(GUI_PEIRODIC_RATE_MS, self.guiPeriodicUpdate)



if __name__ == "__main__": 
    master = MainGUI()
    master.initGuiObjects()
    master.guiPeriodicUpdate()
    master.mainloop()