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
import time, math

GUI_PEIRODIC_RATE_MS = 50




class MainGUI(tk.Tk):

    def initGuiObjects(self):
        self.chartTest1 = StripChart(self, "Test 1", "kPa", 600, 150,   "yellow", 10.0, 0, 6)
        self.chartTest2 = StripChart(self, "Test 2", "bpm", 600, 150,   "cyan", 10.0, 0, 6)
        self.chartTest3 = StripChart(self, "Test 3", "count", 600, 150, "magenta", 10.0, -20, 20)

    def guiPeriodicUpdate(self):
        curTime = time.time()
        self.chartTest1.addNewValue(curTime, 3 + 3*math.sin(2*math.pi*0.25*curTime))
        self.chartTest1.update(curTime)

        self.chartTest2.addNewValue(curTime, abs(6*math.sin(2*math.pi*0.87*curTime)))
        self.chartTest2.update(curTime)

        self.chartTest3.addNewValue(curTime, 15*math.sin(2*math.pi*0.6*curTime))
        self.chartTest3.update(curTime)

        self.after(GUI_PEIRODIC_RATE_MS, self.guiPeriodicUpdate)




master = MainGUI()
master.initGuiObjects()
master.guiPeriodicUpdate()
master.mainloop()