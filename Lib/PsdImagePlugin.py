#
# The Python Imaging Library.
# $Id: PsdImagePlugin.py,v 1.4 1997/01/14 16:38:36 fl Exp $
#
# Adobe PSD 2.5/3.0 file handling
#
# History:
#	95-09-01 fl	Created
#	97-01-03 fl	Read (some) PSD images
#
# Copyright (c) Fredrik Lundh 1995-97.  All rights reserved.
#

__version__ = "0.1"

import string
import Image, ImageFile, ImagePalette


MODES = {
    0:"1",
    1:"L",
    2:"P",
    3:"RGB",
    4:"CMYK",
    7:"MLS",
    8:"L",
    9:"LAB"
}

#
# helpers

def i16(c):
    return ord(c[1]) + (ord(c[0])<<8)

def i32(c):
    return ord(c[3]) + (ord(c[2])<<8) + (ord(c[1])<<16) + (ord(c[0])<<24)

# --------------------------------------------------------------------.
# read PSD images

def _accept(prefix):
    return prefix[:4] == "8BPS"

class PsdImageFile(ImageFile.ImageFile):

    format = "PSD"
    format_description = "Adobe Photoshop"

    def _open(self):

	#
	# header

	s = self.fp.read(26)
	if s[:4] != "8BPS" or i16(s[4:]) != 1:
	    raise SyntaxError, "not a PSD file"

	bits, layers = i16(s[22:]), i16(s[12:])

	self.mode = MODES[i16(s[24:])]
	if self.mode == "MLS":
	    self.mode = string.uppercase[:layers] + "*8"

	self.size = i32(s[18:]), i32(s[14:])

	#
	# mode data

	size = i32(self.fp.read(4))
	if size:
	    data = self.fp.read(size)
	    if self.mode == "P" and size == 768:
		self.palette = ImagePalette.raw("RGB", data)

	#
	# resources

	size = i32(self.fp.read(4))
	if size:
	    self.fp.seek(size, 1) # ignored

	#
	# reserved block (overlays?)

	size = i32(self.fp.read(4))
	if size:
	    self.fp.seek(size, 1) # ignored

	#
	# image descriptor

	self.tile = []

	compression = i16(self.fp.read(2))

	if compression == 0:

	    #
	    # raw compression

	    offset = self.fp.tell()
	    for layer in self.mode:
		self.tile.append(("raw", (0,0)+self.size, offset, layer))
		offset = offset + self.size[0]*self.size[1]

	elif compression == 1:

	    #
	    # packbits compression

	    i = 0
	    s = self.fp.read(layers * self.size[1] * 2) # byte counts
	    offset = self.fp.tell()
	    for layer in self.mode:
		self.tile.append(("packbits", (0,0)+self.size, offset, layer))
		for y in range(self.size[1]):
		    offset = offset + i16(s[i:i+2])
		    i = i + 2

# --------------------------------------------------------------------
# registry

Image.register_open("PSD", PsdImageFile, _accept)

Image.register_extension("PSD", ".psd")
