#
# The Python Imaging Library.
# $Id: ImageWin.py,v 1.3 1996/11/10 17:52:14 fredrik Exp $
#
# a Windows DIB display interface
#
# History:
#	96-05-20 fl	Created
#	96-09-20 fl	Fixed subregion exposure
#
# Copyright (c) Fredrik Lundh 1996.  All rights reserved.
#

import Image

#
# Class wrapper for the Windows display buffer interface
#
# Create an object of this type, paste your data into it, and call
# expose with an hDC casted to a Python int...  In PythonWin, you
# can use the GetHandleAttrib() method of the CDC class to get an
# appropriate hDC.
#

class Dib:
    def __init__(self, mode, size):
	if mode not in ["1", "L", "P", "RGB"]:
	    mode = "RGB"
	self.image = Image.core.display(mode, size)
	self.mode = mode
	self.size = size

    def expose(self, dc, dxy = (0,0,0,0), sxy = (0,0)):
	return self.image.expose(dc, dxy, sxy)

    def query_palette(self, dc):
	return self.image.query_palette(dc)

    def paste(self, im, box = None):
	# fix to handle conversion when pasting
	im.load()
	if self.mode != im.mode:
	    im = im.convert(self.mode)
	if box:
	    self.image.paste(im.im, box)
	else:
	    self.image.paste(im.im)
