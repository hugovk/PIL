#
# The Python Imaging Library.
# $Id: ImageTk.py,v 1.5 1996/11/10 17:52:14 fredrik Exp $
#
# a Tk display interface
#
# History:
#	96-04-08 fl	Created
#	96-09-06 fl	Added getimage method
#	96-11-01 fl	Rewritten, removed image attribute and crop method
#
# Copyright (c) Fredrik Lundh 1996.  All rights reserved.
#


import Image, Tkinter


# --------------------------------------------------------------------
# PhotoImage

class PhotoImage:

    def __init__(self, mode, size = None):
	if hasattr(mode, "mode") and hasattr(mode, "size"):
	    # got an image
	    image = mode
	    mode = image.mode
	    size = image.size
	else:
	    image = None
	if mode not in ["1", "L"]:
	    mode = "RGB"
	self.__mode = mode
	self.__size = size
	self.__tk = Tkinter.PhotoImage()
	self.image = self.__tk # compatibility
	if image:
	    self.paste(image)

    def __str__(self):
	return str(self.__tk)

    def paste(self, im, box = None):

	# convert to blittable 
	block = Image.core.new_block(self.__mode, self.__size)
	if block.mode != im.mode:
	    im = im.convert(block.mode)
	im.load()
	block.paste(im.im, (0,0)+im.size)

	tk = Tkinter.PhotoImage(data=block.id, format="PyImagingCore")

	# paste into existing image
	if not box:
	    tk.tk.call(self.__tk, 'copy', tk)
	else:
	    x0, y0, x1, y1 = box
	    # FIXME: doesn't work properly if x0 or y0 is less than 0;
	    # needs to set the -from attribute properly
	    x0, y0 = max(x0, 0), max(y0, 0)
	    x1, y1 = min(x1, self.__size[0]), min(y1, self.__size[1])
	    tk.tk.call(self.__tk, 'copy', tk, "-to", x0, y0, x1, y1)


# --------------------------------------------------------------------
# BitmapImage

class BitmapImage:

    def __init__(self, image, **kw):
	kw["data"] = image.tobitmap()
	self.__tk = apply(Tkinter.BitmapImage, (), kw)

    def __str__(self):
	return str(self.__tk)


# --------------------------------------------------------------------
# Helper for the Image.show method.

def _show(image, title):

    import Tkinter

    class UI(Tkinter.Label):
	def __init__(self, master, im):
	    if im.mode == "1":
		self.image = BitmapImage(im, foreground="white")
	    else:
		self.image = PhotoImage(im)
	    Tkinter.Label.__init__(self, master, image=self.image,
		bg="black", bd=0)

    if not Tkinter._default_root:
	raise IOError, "tkinter not initialized"
    top = Tkinter.Toplevel()
    if title:
	top.title(title)
    UI(top, image).pack()
