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
from statusDisplay import StatusDisplay
from serialInterface import SerialInterface
import time, math, multiprocessing

GUI_PEIRODIC_RATE_MS = 50

def moveCursorToCorner():
    from Xlib import X, display
    d = display.Display()
    s = d.screen()
    root = s.root
    root.warp_pointer(0,0)
    d.sync()




class MainGUI(tk.Tk):

    def initSerialInterface(self):
        # creating a pipe 
        self.pipeSerialInfConn, self.pipeGuiCon = multiprocessing.Pipe(duplex=True) 
    
        self.serInf = SerialInterface(self.pipeSerialInfConn)
        self.serInfProc = multiprocessing.Process(target=self.serInf.run)
        self.statusDisplay.setStatusText("Connected \n Port: {} \n Baud: {}".format(self.serInf.ser.port, self.serInf.ser.baudrate))
        self.serInfProc.start()

    def initGuiObjects(self):
        
        # Move mouse pointer out of the way
        self.event_generate('<Motion>', warp=True, x=0, y=0)
        
        screen_width  = self.winfo_screenwidth()
        screen_height = self.winfo_screenheight()

        self.chartTest1 = StripChart(self, "Pressure", "kPa", screen_width, screen_height/4, "yellow",  5.0,   0, 1024)
        self.chartTest2 = StripChart(self, "Insp", "bpm",     screen_width, screen_height/4, "cyan",    5.0,   0, 1024)
        self.chartTest3 = StripChart(self, "Cycles", "count", screen_width, screen_height/4, "magenta", 5.0,   0, 1024)
        
        self.statusDisplay = StatusDisplay(self, screen_width, screen_height/4)
        self.statusDisplay.setStatusText("Init...")
        self.statusDisplay.update()
        
        self.bind("<Escape>", self.escapeKeyHandler)

        
    def shutDownSerialInf(self):
        self.pipeGuiCon.send("END")
        self.serInfProc.join()


    def guiPeriodicUpdate(self):
        curTime = time.time()

        while(self.pipeGuiCon.poll() > 0):
            # Serial reader process had sent (or is in the process of sending)
            #  a packet of serial data
            serRxData = self.pipeGuiCon.recv()
            self.chartTest1.addNewValue(serRxData[0], serRxData[1])
            self.chartTest2.addNewValue(serRxData[0], serRxData[2])
            self.chartTest3.addNewValue(serRxData[0], serRxData[3])
            
            #Fault display TEST ONLY
            if(serRxData[1] > 600):
                self.statusDisplay.setFault(True, "Test Fault \n Not actually a real fault.")
            else:
                self.statusDisplay.setFault(False, "")



        
        #Sample Time Vectors
        # self.chartTest1.addNewValue(curTime, 3 + 3*math.sin(2*math.pi*0.25*curTime))
        # self.chartTest2.addNewValue(curTime, abs(6*math.sin(2*math.pi*0.87*curTime)))
        # self.chartTest3.addNewValue(curTime, 15*math.sin(2*math.pi*0.6*curTime))

        self.chartTest1.update(curTime)
        self.chartTest2.update(curTime)
        self.chartTest3.update(curTime)
        self.statusDisplay.update()

        self.after(GUI_PEIRODIC_RATE_MS, self.guiPeriodicUpdate)
    
    def escapeKeyHandler(self, event):
        self.destroy()



if __name__ == "__main__": 

    print("Init GUI...")
    moveCursorToCorner()
    
    # Create the base GUI object
    master = MainGUI()
    
    # Make things go fullscreen
    master.attributes("-fullscreen", True)
    
    master.initGuiObjects()
    
    print("Init Serial Interface...")
    master.initSerialInterface()

    print("Bootstrapping peiorid update...")
    master.guiPeriodicUpdate()
    
    print("Init done!")
    
    print("Starting main GUI application.")
    master.mainloop()
    
    print("Starting Shutdown sequence")
    master.shutDownSerialInf()
    
    print("All shut down, goodbye!")
