#
# The Python Imaging Library.
# $Id: SunImagePlugin.py,v 1.3 1996/11/10 17:52:14 fredrik Exp $
#
# Sun image file handling
#
# History:
#	95-09-10 fl	Created
#	96-05-28 fl	Fixed 32-bit alignment
#
# Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
#


__version__ = "0.1"


import regex, string
import Image, ImageFile


def i16(c):
    return ord(c[1]) + (ord(c[0])<<8)

def i32(c):
    return ord(c[3]) + (ord(c[2])<<8) + (ord(c[1])<<16) + (ord(c[0])<<24)


class SunImageFile(ImageFile.ImageFile):

    format = "SUN"
    format_description = "Sun Raster File"

    def _open(self):

	# HEAD
	s = self.fp.read(32)
	if i32(s) != 0x59a66a95:
	    raise SyntaxError, "not an SUN raster file"

	offset = 32

	self.size = i32(s[4:8]), i32(s[8:12])

	depth = i32(s[12:16])
	if depth == 1:
	    self.mode, rawmode = "1", "1;I"
	elif depth == 8:
	    self.mode = rawmode = "L"
	elif depth == 24:
	    self.mode, rawmode = "RGB", "BGR"
	else:
	    raise SyntaxError, "unsupported mode"

	compression = i32(s[20:24])

	if i32(s[24:28]) != 0:
	    length = i32(s[28:32])
	    offset = offset + length
	    self.palette = "RGB;L", self.fp.read(length)
	    if self.mode == "L":
		self.mode = "P"

	stride = (((self.size[0] * depth + 7) / 8) + 3) & -4

	if compression == 1:
	    self.tile = [("raw", (0,0)+self.size, offset, (rawmode, stride))]
	elif compression == 2:
	    self.tile = [("sun_rle", (0,0)+self.size, offset, rawmode)]

Image.register_open("SUN", SunImageFile)

Image.register_extension("SUN", ".ras")
