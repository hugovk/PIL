#
# The Python Imaging Library.
# $Id: MspImagePlugin.py,v 1.3 1996/12/16 18:50:23 fl Exp fl $
#
# MSP file handling
#
# This is the format used by the Paint program in Windows 1 and 2.
#
# History:
#	95-09-05 fl	Created
#
# Copyright (c) Fredrik Lundh 1995.  All rights reserved.
#


__version__ = "0.1"

import Image, ImageFile


def i16(c):
    return ord(c[0]) + (ord(c[1])<<8)


class MspImageFile(ImageFile.ImageFile):

    format = "MSP"
    format_description = "Windows Paint"

    def _open(self):

	# HEADER
	s = self.fp.read(32)
	if s[:4] not in ["DanM", "LinS"]:
	    raise SyntaxError, "not a MSP file"

	self.mode = "0 1"
	self.size = i16(s[4:]), i16(s[6:])


Image.register_open("MSP", MspImageFile)

Image.register_extension("MSP", ".msp")
