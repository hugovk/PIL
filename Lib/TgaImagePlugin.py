#
# The Python Imaging Library.
# $Id: TgaImagePlugin.py,v 1.4 1996/11/10 17:52:14 fredrik Exp $
#
# TGA file handling
#
# History:
#	95-09-01 fl	Created (reads 24-bit files only)
#
# Copyright (c) Fredrik Lundh 1995.  All rights reserved.
#


__version__ = "0.1"

import Image, ImageFile


def i16(c):
    return ord(c[0]) + (ord(c[1])<<8)

def i32(c):
    return ord(c[0]) + (ord(c[1])<<8) + (ord(c[2])<<16) + (ord(c[3])<<24)


class TgaImageFile(ImageFile.ImageFile):

    format = "TGA"
    format_description = "Targa"

    def _open(self):

	# Header
	s = self.fp.read(18)

	self.mode = "RGB"
	self.size = i16(s[12:]), i16(s[14:])

	bits = ord(s[16])

	# Validation (we only handle uncompressed truecolour)
	if i32(s[0:]) != 0x20000 or\
	   self.size[0] <= 0 or self.size[1] <= 0 or\
	   bits not in [16, 24, 32]:
	    raise SyntaxError, "not a TGA file"

	if bits == 24:
	    self.tile = [("raw", (0, 0) + self.size, 18, ("BGR", 0, 1))]
	if bits == 32:
	    self.tile = [("raw", (0, 0) + self.size, 18, ("BGRX", 0, 1))]

Image.register_open("TGA", TgaImageFile)

Image.register_extension("TGA", ".tga")
