#
# The Python Imaging Library
# $Id$
#

from Tkinter import *
import Image, ImageTk
import sys
import os

#
# an image viewer

class UI(Label):

    def __init__(self, master, im):

	if im.mode == "1":
            # bitmap image
	    self.image = ImageTk.BitmapImage(im, foreground="white")
            Label.__init__(self, master, image=self.image, bg="black", bd=0)

        else:
            # photo image
            self.image = ImageTk.PhotoImage(im)
            Label.__init__(self, master, image=self.image, bd=0)

#
# script interface

def main():

    if os.name == 'mac' and not sys.argv[1:]:
    	import macfs
    	fss, ok = macfs.StandardGetFile()
    	if not ok: sys.exit(0)
    	sys.argv[1:] = [fss.as_pathname()]
    if not sys.argv[1:]:
	print "Syntax: python viewer.py imagefile"
	sys.exit(1)

    filename = sys.argv[1]

    root = Tk()
    root.title(filename)

    try:
	root.config(bd=2, relief=SUNKEN)
    except AttributeError:
    	# workaround for Python 1.4
	root.tk.call(root, "config", "-bd", 2, "-relief", SUNKEN)

    im = Image.open(filename)

    UI(root, im).pack()

    root.mainloop()


main()
