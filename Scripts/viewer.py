#
# The Python Imaging Library
# $Id: viewer.py,v 1.1 1996/12/16 18:51:31 fl Exp fl $
#

from Tkinter import *
import Image, ImageTk
import sys

#
# an image viewer

class UI(Label):
    def __init__(self, master, im):
	if im.mode == "1":
	    self.image = ImageTk.BitmapImage(im, foreground="white")
	else:
	    self.image = ImageTk.PhotoImage(im)
	Label.__init__(self, master, image=self.image, bg="black", bd=0)


#
# script interface

if __name__ == "__main__":

    if not sys.argv[1:]:
	print "Syntax: python viewer.py imagefile"
	sys.exit(1)

    filename = sys.argv[1]

    root = Tk()
    root.title(filename)

    UI(root, Image.open(filename)).pack()

    root.mainloop()
