#
# The Python Imaging Library
# $Id$
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

    im = Image.open(filename)

    #
    # handle transparent images

    r, g, b = root.winfo_rgb("systembuttonface")
    rgb = r / 256, g / 256, b / 256

    if im.mode == "P" and im.info.has_key("transparency"):

	# palette + transparency property (GIF, PNG, XPM).  Create
	# a transparency mask, and paste the image onto a solid back-
	# ground through that mask.

	lut = [255] * 256
	lut[im.info["transparency"]] = 0

	i = Image.new("RGB", im.size, rgb)
	i.paste(im, None, im.point(lut, "1"))

	im = i

    elif im.mode == "RGBA":

	# true colour w. transparency (PNG, TIFF).  simply paste it
	# onto a background image

	i = Image.new("RGB", im.size, rgb)
	i.paste(im, None, im)
	
	im = i
	
    UI(root, im).pack()

    root.mainloop()
