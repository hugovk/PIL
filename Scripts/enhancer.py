#
# The Python Imaging Library
# $Id$
#
# this demo script creates four windows containing an image and a slider.
# drag the slider to modify the image.
# 

from Tkinter import *
import Image, ImageTk, ImageEnhance
import sys
import os

#
# enhancer widget

class Enhance(Frame):
    def __init__(self, master, image, name, enhancer, lo, hi):
	Frame.__init__(self, master)

	# set up the image
	self.tkim = ImageTk.PhotoImage(image.mode, image.size)
	self.enhancer = enhancer(image)
	self.update("1.0") # normalize

	# image window
	Label(self, image=self.tkim).pack()

	# scale
	s = Scale(self, label=name, orient=HORIZONTAL,
		  from_=lo, to=hi, resolution=0.01,
		  command=self.update)
	s.set(self.value)
        s.pack()

    def update(self, value):
	self.value = eval(value)
	self.tkim.paste(self.enhancer.enhance(self.value))

#
# main

if os.name == 'mac' and len(sys.argv) == 1:
	import macfs
	fss, ok = macfs.StandardGetFile()
	if not ok: sys.exit(0)
	sys.argv.append(fss.as_pathname())
root = Tk()

im = Image.open(sys.argv[1])

im.thumbnail((200, 200))

Enhance(root, im, "Color", ImageEnhance.Color, 0.0, 4.0).pack()
Enhance(Toplevel(), im, "Sharpness", ImageEnhance.Sharpness, -2.0, 2.0).pack()
Enhance(Toplevel(), im, "Brightness", ImageEnhance.Brightness, -1.0, 3.0).pack()
Enhance(Toplevel(), im, "Contrast", ImageEnhance.Contrast, -1.0, 3.0).pack()

root.mainloop()
