#
# The Python Imaging Library.
# $Id: FliImagePlugin.py,v 1.2 1996/11/10 17:52:14 fredrik Exp $
#
# FLI/FLC file handling.
#
# History:
#	95-09-01 fl	Created
#
# Copyright (c) Fredrik Lundh 1995.  All rights reserved.
#


__version__ = "0.1"

import Image, ImageFile


def i16(c):
    return ord(c[0]) + (ord(c[1])<<8)


class FliImageFile(ImageFile.ImageFile):

    format = "FLI"
    format_description = "Autodesk FLI/FLC Animation"

    def _open(self):

	# HEAD
	s = self.fp.read(128)
	if i16(s[4:]) not in [0xAF11, 0xAF12]:
	    raise SyntaxError, "not an FLI/FLC file"

	self.mode = "L"
	self.size = i16(s[8:]), i16(s[10:]) 
	self["frames"] = i16(s[6:])


Image.register_open("FLI", FliImageFile)

Image.register_extension("FLI", ".fli")
Image.register_extension("FLI", ".flc")
