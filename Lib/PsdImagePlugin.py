#
# The Python Imaging Library.
# $Id: PsdImagePlugin.py,v 1.3 1996/11/10 17:52:14 fredrik Exp $
#
# Adobe PSD 2.5/3.0 file handling
#
# History:
#	95-09-01 fl	Created
#
# Copyright (c) Fredrik Lundh 1995.  All rights reserved.
#


__version__ = "0.1"

import string
import Image, ImageFile


MODES = {
    0:"1",
    1:"L",
    2:"L",
    3:"RGB",
    4:"CMYK",
    7:"MLS",
    9:"LAB"
}


def i16(c):
    return ord(c[1]) + (ord(c[0])<<8)

def i32(c):
    return ord(c[3]) + (ord(c[2])<<8) + (ord(c[1])<<16) + (ord(c[0])<<24)


class PsdImageFile(ImageFile.ImageFile):

    format = "PSD"
    format_description = "Adobe Photoshop"

    def _open(self):

	# Header
	s = self.fp.read(34)
	if s[:4] != "8BPS" or i16(s[4:]) != 1:
	    raise SyntaxError, "not a PSD file"

	bits, layers = i16(s[22:]), i32(s[10:])

	self.mode = MODES[i16(s[24:])]
	if self.mode == "MLS":
	    self.mode = string.uppercase[:layers] + "*8"

	self.size = i32(s[14:]), i32(s[18:])


Image.register_open("PSD", PsdImageFile)

Image.register_extension("PSD", ".psd")
