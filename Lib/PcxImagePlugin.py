#
# The Python Imaging Library.
# $Id: PcxImagePlugin.py,v 1.5 1996/12/16 18:50:23 fl Exp fl $
#
# PCX file handling
#
# This format was used by the popular PaintBrush applications, from
# ZSoft, Inc.
#
# History:
#	95-09-01 fl	Created
#	96-05-20 fl	Fixed RGB support
#
# Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
#


__version__ = "0.1"


import array
import Image, ImageFile


def i16(c):
    return ord(c[0]) + (ord(c[1])<<8)


class PcxImageFile(ImageFile.ImageFile):

    format = "PCX"
    format_description = "Paintbrush"

    def _open(self):

	# Header
	s = self.fp.read(128)
	if ord(s[0]) != 10 or ord(s[1]) not in [0, 2, 3, 5]:
	    raise SyntaxError, "not a PCX file"
 
	# Image
	box = i16(s[4:]), i16(s[6:]), i16(s[8:])+1, i16(s[10:])+1

	# Format
	bits, planes, stride = ord(s[3]), ord(s[65]), i16(s[66:])
	if bits == 1 and planes == 1:
	    self.mode = rawmode = "1"
	elif bits == 1 and planes == 4:
	    self.mode = "P"
	    rawmode = "L;4"
	    self.palette = "RGB", s[16:64]
	elif bits == 8 and planes == 1:
	    self.mode = rawmode = "L"
	    # FIXME: hey, this doesn't work with the incremental loader !!!
	    self.fp.seek(-769, 2)
	    s = self.fp.read(769)
	    if ord(s[0]) == 12:
		self.palette = "RGB", s[1:]
		self.mode = "P"
	elif bits == 8 and planes == 3:
	    self.mode = "RGB"
	    rawmode = "RGB;L"
	else:
	    raise IOError, "unknown PCX mode"

	self.size = box[2]-box[0], box[3]-box[1]

	self.tile = [("pcx", box, 128, rawmode)]


Image.register_open("PCX", PcxImageFile)

Image.register_extension("PCX", ".pcx")
