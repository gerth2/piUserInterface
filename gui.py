from tkinter import *
from stripChart import StripChart

master = Tk()

chartTest1 = StripChart(master, "Test 1", "kPa", 600, 200, "yellow", 15.0, 0, 6)

chartTest1.addNewValue(-5, 22)
chartTest1.addNewValue(0,   1)
chartTest1.addNewValue(1,   3)
chartTest1.addNewValue(2,   2)
chartTest1.addNewValue(3,   4)
chartTest1.addNewValue(4,   3)
chartTest1.addNewValue(15,  0)
chartTest1.update(15)


mainloop()