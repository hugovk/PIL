#
# The Python Imaging Library.
# $Id: ImageTk.py,v 1.5 1996/11/10 17:52:14 fredrik Exp $
#
# a Tk display interface
#
# History:
# 96-04-08 fl	Created
# 96-09-06 fl	Added getimage method
# 96-11-01 fl	Rewritten, removed image attribute and crop method
# 97-05-09 fl	Use PyImagingPaste method instead of image type
# 97-05-12 fl	Minor tweaks to match the IFUNC95 interface
# 97-05-17 fl	Support the "pilbitmap" booster patch
# 97-06-05 fl	Added file= and data= argument to image constructors
# 98-03-09 fl	Added width and height methods to Image classes
#
# Copyright (c) Secret Labs AB 1997-98.
# Copyright (c) Fredrik Lundh 1996-97.
#
# See the README file for information on usage and redistribution.
#

import Image, Tkinter

# --------------------------------------------------------------------
# Check for optional "pilbitmap" booster patch

_pilbitmap_ok = None

def _pilbitmap_check():
    global _pilbitmap_ok
    if _pilbitmap_ok is None:
        try:
            im = Image.new("1", (1,1))
            Tkinter.BitmapImage(data="PIL:%d" % im.im.id)
            _pilbitmap_ok = 1
        except:
            _pilbitmap_ok = 0
    return _pilbitmap_ok

# --------------------------------------------------------------------
# PhotoImage

class PhotoImage:

    def __init__(self, image = None, size = None, **kw):

        # Tk compatibility: file or data
	if image is None:
	    if kw.has_key("file"):
		image = Image.open(kw["file"])
		del kw["file"]
	    elif kw.has_key("data"):
		from StringIO import StringIO
		image = Image.open(StringIO(kw["data"]))
		del kw["data"]

	if hasattr(image, "mode") and hasattr(image, "size"):
	    # got an image instead of a mode
	    mode = image.mode
            if mode == "P":
                # palette mapped data
                image.load()
                mode = image.palette.mode
	    size = image.size
	    kw["width"], kw["height"] = size
	else:
            mode = image
	    image = None

	if mode not in ["1", "L", "RGB", "RGBA"]:
            if mode == "F":
                mode = "L"
            else:
                mode = "RGB"

	self.__mode = mode
	self.__size = size
	self.__photo = apply(Tkinter.PhotoImage, (), kw)
	if image:
	    self.paste(image)

    def __str__(self):
	return str(self.__photo)

    def width(self):
	return self.__size[0]

    def height(self):
	return self.__size[1]

    def paste(self, im, box = None):

	# convert to blittable
        im.load()
        image = im.im
        if image.isblock() and im.mode == self.__mode:
            block = image
        else:
            block = image.new_block(self.__mode, im.size)
            image.convert2(block, image) # convert directly between buffers

        self.__photo.tk.call("PyImagingPhoto", self.__photo, block.id)


# --------------------------------------------------------------------
# BitmapImage

class BitmapImage:

    def __init__(self, image = None, **kw):

        # Tk compatibility: file or data
	if image is None:
            if kw.has_key("file"):
                image = Image.open(kw["file"])
                del kw["file"]
            elif kw.has_key("data"):
                from StringIO import StringIO
                image = Image.open(StringIO(kw["data"]))
                del kw["data"]

	self.__mode = image.mode
	self.__size = image.size

        if _pilbitmap_check():
            # fast way (requires the pilbitmap booster patch)
            image.load()
            kw["data"] = "PIL:%d" % image.im.id
            self.__im = image # must keep a reference
        else:
            # slow but safe way
            kw["data"] = image.tobitmap()
        self.__photo = apply(Tkinter.BitmapImage, (), kw)

    def width(self):
	return self.__size[0]

    def height(self):
	return self.__size[1]

    def __str__(self):
	return str(self.__photo)


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
