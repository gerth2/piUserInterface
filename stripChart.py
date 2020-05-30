################################################################################
## COPYRIGHT 2020 Ventilators In Peoria (VIP)
## See license.txt in for more info
##
## THE SOFTWARE IS PROVIDED "AS IS". IT IS NOT FIT FOR ANY PARTICULAR PURPOSE,
## CONFORMS TO NO REGULATORY BODY REQUIREMENTS, AND IS GENERALLY NOT SAFE
## FOR ANY APPLICATION INVOLVING HUMAN LIFE. USE AT YOUR OWN RISK.
################################################################################


from tkinter import *

class StripChart:

    def __init__(self, master, title, units, width, height, color, timeDur, minVal, maxVal):
        # Configuration
        self.width = width
        self.height = height
        self.timeDur = timeDur
        self.maxVal = maxVal
        self.minVal = minVal
        self.color = color
        self.bgColor = "#111111"
        self.title = title
        self.units = units
        self.decimationFactor = 1
        self.decimationCounter = 0

        assert(self.maxVal > self.minVal)
        assert(self.timeDur > 0)

        # State
        self.valsList = []
        self.displayedValue = 0
        self.displayedMin = 0
        self.displayedMax = 0
        
        # GUI Objects
        self.guiRefCurVal = None
        self.guiRefMaxVal = None
        self.guiRefMinVal = None
        self.guiRefLinSegList = []

        # Calculated Constants
        self.PIXEL_ORIGIN_X = self.width * 0.75
        self.PIXEL_ORIGIN_Y = self.height * 0.90
        self.PIXELS_PER_UNIT_TIME = self.width * 0.70 / self.timeDur
        self.PIXELS_PER_UNIT_VALUE = (self.height * 0.8) / (self.maxVal - self.minVal)

        # Init the canvas and draw initial content
        self.w = Canvas(master, width=self.width, height=self.height, borderwidth=0, highlightthickness=0)
        self.w.pack()
        self.initText()
        self.initChart()
        
    def addNewValue(self, time, value):
        #Only pack plotted values at the decimation rate
        # Plot samples at the decimation factor rate
        if(self.decimationCounter % self.decimationFactor == 0):
            self.valsList.append((time, value))
            
        self.decimationCounter += 1

        self.displayedValue = value 

    def update(self, cur_time):
        self.updateChart(cur_time)
        self.updateText()
        
    def initChart(self):
        self.w.create_rectangle(0, 0, self.width * 0.8, self.height, fill=self.bgColor, outline=self.color, width=4)

    def updateChart(self, cur_time):
        
        lineSegIdx = 0;

        if(len(self.valsList) > 1):
            # Need at least two points to draw anything

            # Init the "prev" values with the first point in the list
            end = len(self.valsList) - 1
            prevVal = self.valsList[end][1]
            prevTime = cur_time - self.valsList[end][0]

            self.displayedMax = prevVal
            self.displayedMax = prevVal

            # Iterate over all other points in the list. We go backward over the list so as to to iterate backward in time, from new to old
            for idx, sample in enumerate(reversed(self.valsList[1:])):
                
                val = sample[1]
                time = cur_time - sample[0]
                if(time > self.timeDur):
                    # We hit the end of the points on the chart we want to plot.
                    # "cull" the remaining points to keep RAM usage down.
                    startLen = len(self.valsList)
                    numRemove = startLen - idx - 1
                    del self.valsList[:numRemove]
                    break
                else:

                    # Plot this point with a line connecting to the previous value
                    startX = self.toPixelX(prevTime)
                    startY = self.toPixelY(prevVal)
                    endX   = self.toPixelX(time)
                    endY   = self.toPixelY(val)
                    
                    # Create a new line segement if needed, or update the existing corresponding one.
                    if(lineSegIdx >= len(self.guiRefLinSegList)):
                        self.guiRefLinSegList.append(self.w.create_line(startX, startY, endX, endY, fill=self.color, width=2))
                    else:
                        self.w.coords(self.guiRefLinSegList[lineSegIdx], startX, startY, endX, endY)
                        
                    prevVal = val
                    prevTime = time
                    lineSegIdx += 1

                    # Basic transform to displayed values
                    # TODO - better formatting
                    if(val > self.displayedMax):
                        self.displayedMax = val
                    if(val < self.displayedMin):
                        self.displayedMin = val
                    


    def toPixelY(self, val):
        return self.PIXEL_ORIGIN_Y - (val - self.minVal) * self.PIXELS_PER_UNIT_VALUE

    def toPixelX(self, time):
        return self.PIXEL_ORIGIN_X - time * self.PIXELS_PER_UNIT_TIME

    def initText(self):
        # Static objects
        self.w.create_rectangle(self.width * 0.8, 0, self.width, self.height, fill=self.bgColor, outline=self.color, width=4)
        self.w.create_text(self.width * 0.9, self.height * 0.1,  fill = self.color, font=('Arial',14,'bold italic'), text=str(self.title))
        self.w.create_text(self.width * 0.9, self.height * 0.500,fill = self.color, font=('Arial',13,'bold italic'), text=str(self.units))
        
        # Mutable Objects
        self.guiRefMaxVal = self.w.create_text(self.width * 0.9, self.height * 0.7,  fill = self.color, font=('Arial',9,'bold'), text="Max:" )
        self.guiRefMinVal = self.w.create_text(self.width * 0.9, self.height * 0.8,  fill = self.color, font=('Arial',9,'bold'), text="Min:" )
        self.guiRefCurVal = self.w.create_text(self.width * 0.9, self.height * 0.30, fill = self.color, font=('Arial',22,'bold'), text="")

    def updateText(self):
        self.w.itemconfig(self.guiRefMaxVal, text="Max:" + self.numToStr(self.displayedMax))
        self.w.itemconfig(self.guiRefMinVal, text="Min:" + self.numToStr(self.displayedMin))
        self.w.itemconfig(self.guiRefCurVal, text= self.numToStr(self.displayedValue))


    def numToStr(self, number):
        return "{:.2f}".format(number)
