################################################################################
## COPYRIGHT 2020 Ventilators In Peoria (VIP)
## See license.txt in for more info
##
## THE SOFTWARE IS PROVIDED "AS IS". IT IS NOT FIT FOR ANY PARTICULAR PURPOSE,
## CONFORMS TO NO REGULATORY BODY REQUIREMENTS, AND IS GENERALLY NOT SAFE
## FOR ANY APPLICATION INVOLVING HUMAN LIFE. USE AT YOUR OWN RISK.
################################################################################


from tkinter import *

class StatusDisplay:

    def __init__(self, master, width, height):
        # Configuration
        self.width = width
        self.height = height
        self.fgColor = "#FFFFFF"
        self.bgColor = "#111111"

        # State
        self.faultActive = False
        self.faultText   = ""
        self.statusText  = ""
        
        self.faultTextChanged = True
        self.statusTextChanged = True
        
        self.faultBlinkCounter = 0
        self.faultBlinkPeriod = 2

        
        # GUI Objects
        self.guiRefActiveFaultText = None
        self.guiRefFaultBorder = None
        self.guiRefFaultUserText= None
        self.guiRefStatusText = None

        # Init the canvas and draw initial content
        self.w = Canvas(master, width=self.width, height=self.height, borderwidth=0, highlightthickness=0)
        self.w.pack()
        self.initStatusPane()
        self.initFaultPane()

        
    def setStatusText(self, text):
        if(text != self.statusText):
            # Lazy update to reduce GUI update CPU load
            self.statusText = text
            self.statusTextChanged = True
            
        
    def setFault(self, faultActive, text):
        self.faultActive = faultActive
        
        if(text != self.faultText):
            self.faultText = text
            self.faultTextChanged = True

        
    def initStatusPane(self):
        self.w.create_rectangle(0, 0, self.width * 0.5, self.height, fill=self.bgColor, outline=self.fgColor, width=4)
        self.guiRefStatusText= self.w.create_text(self.width * 0.25, self.height * 0.5,  fill = self.fgColor, font=('Arial',10,'bold'), text=self.statusText )

    def initFaultPane(self):
        self.guiRefFaultBorder = self.w.create_rectangle(self.width * 0.5, 0, self.width, self.height, fill=self.bgColor, outline=self.fgColor, width=4)
        self.guiRefActiveFaultText = self.w.create_text(self.width * 0.75, self.height * 0.33,  fill = self.fgColor, font=('Arial',10,'bold'), text="" )
        self.guiRefFaultUserText = self.w.create_text(self.width * 0.75, self.height * 0.66,  fill = self.fgColor, font=('Arial',10,'bold'), text=self.faultText )

    def update(self):
        
        if(self.faultActive):
            faultStatusString = "FAULT!"
            faultFont=('Arial',24,'bold')
            faultFill = "#FF0000"
            if(self.faultBlinkCounter < self.faultBlinkPeriod/2):
                faultBorderColor = "#FF0000"
            else:
                faultBorderColor = self.fgColor
            self.faultBlinkCounter = (self.faultBlinkCounter + 1) % self.faultBlinkPeriod
        else:
            faultStatusString = "No Fault."
            faultFont=('Arial',15,)
            faultFill = "#00DD00"
            faultBorderColor = self.fgColor
            self.faultBlinkCounter = 0
        
        self.w.itemconfig(self.guiRefActiveFaultText, text=faultStatusString)
        self.w.itemconfig(self.guiRefActiveFaultText, font=faultFont)
        self.w.itemconfig(self.guiRefActiveFaultText, fill=faultFill)
        self.w.itemconfig(self.guiRefFaultBorder, outline=faultBorderColor)

        #Lazy Updates to user-declared text
        if(self.statusTextChanged):
            self.statusTextChanged = False
            self.w.itemconfig(self.guiRefStatusText,    text=self.statusText)
        if(self.faultTextChanged):
            self.faultTextChanged = False
            self.w.itemconfig(self.guiRefFaultUserText, text=self.faultText)


    def numToStr(self, number):
        return "{:.2f}".format(number)
