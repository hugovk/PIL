#
# The Python Imaging Library.
# $Id: BmpImagePlugin.py,v 1.7 1997/01/14 16:38:36 fl Exp $
#
# BMP file handler
#
# Windows (and OS/2) native bitmap storage format.
#
# History:
#	95-09-01 fl	Created
#	96-04-30 fl	Added save
#
# Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
#


__version__ = "0.1"


import array
import Image, ImageFile, ImagePalette


#
# --------------------------------------------------------------------
# Read BMP file

def i16(c):
    return ord(c[0]) + (ord(c[1])<<8)

def i32(c):
    return ord(c[0]) + (ord(c[1])<<8) + (ord(c[2])<<16) + (ord(c[3])<<24)


BIT2MODE = {
    1: ("1", "1"),
    4: ("P", "P;4"),
    8: ("P", "P"),
    24: ("RGB", "BGR")
}

def _accept(prefix):
    return prefix[:2] == "BM"

class BmpImageFile(ImageFile.ImageFile):

    format = "BMP"
    format_description = "Windows Bitmap"

    def _bitmap(self, header = 0, offset = 0):

	if header:
	    self.fp.seek(header)

	# CORE/INFO
	s = self.fp.read(4)
	s = s + self.fp.read(i32(s)-4)

	colors = 0

	if len(s) == 12:

	    # OS/2 1.0 CORE
	    bits = i16(s[10:])
	    self.size = i16(s[4:]), i16(s[6:])
	    lutsize = 3

	elif len(s) in [40, 64]:

	    # WIN 3.1 or OS/2 2.0 INFO
	    bits = i16(s[14:])
	    self.size = i32(s[4:]), i32(s[8:])
	    self.info["compression"] = i32(s[16:])
	    lutsize = 4

	else:
	    raise IOError, "Unknown BMP header type"

	# MODE
	try:
	    self.mode, rawmode = BIT2MODE[bits]
	except KeyError:
	    raise IOError, "Unsupported BMP pixel depth"

	# LUT
	if self.mode != "RGB":
	    colors = 1<<bits
	    if lutsize == 3:
		self.palette = ImagePalette.raw("BGR", self.fp.read(colors*3))
	    else:
		self.palette = ImagePalette.raw("BGRX", self.fp.read(colors*4))
	    if self.mode == "L":
		self.mode = "P"

	if offset == 0:
	    offset = self.fp.tell()
		
	self.tile = [("raw",
		     (0, 0) + self.size,
		     offset,
		     (rawmode, ((self.size[0]*bits+31)>>3)&(-4), -1))]

    def _open(self):

	# HEAD
	s = self.fp.read(14)
	if s[:2] != "BM":
	    raise SyntaxError, "Not a BMP file"
	offset = i32(s[10:])

	self._bitmap()


#
# --------------------------------------------------------------------
# Write BMP file

def o16(i):
    return chr(i&255) + chr(i>>8&255)

def o32(i):
    return chr(i&255) + chr(i>>8&255) + chr(i>>16&255) + chr(i>>24&255)

def _save(im, fp, filename):

    if im.mode == "1":
	rawmode, bits, colors = "1I", 1, 2
    elif im.mode in ["L", "P"]:
	rawmode, bits, colors = "L", 8, 256
    elif im.mode == "RGB":
	rawmode, bits, colors = "BGR", 24, 0
    else:
	raise IOError, "cannot write mode %s as BMP" % im.mode

    stride = ((im.size[0]*bits+7)/8+3)&-4
    header = 40 # or 64 for OS/2 version 2
    offset = 14 + header + colors * 4
    image  = stride * im.size[1]

    # bitmap header
    fp.write("BM" +			# file type (magic)
	     o32(offset+image) +	# file size
	     o32(0) +			# reserved
	     o32(offset))		# image data offset

    # bitmap info header
    fp.write(o32(header) +		# info header size
	     o32(im.size[0]) +		# width
	     o32(im.size[1]) +		# height
	     o16(1) +			# planes
	     o16(bits) +		# depth
	     o32(0) +			# compression (0=uncompressed)
	     o32(image) +		# size of bitmap
	     o32(1) + o32(1) +		# resolution
	     o32(colors) +		# colors used
	     o32(colors))		# colors important

    fp.write("\000" * (header - 40))	# padding (for OS/2 format)

    if im.mode == "1":
	for i in (0, 255):
	    fp.write(chr(i) * 4)
    elif im.mode == "L":
	for i in range(256):
	    fp.write(chr(i) * 4)
    elif im.mode == "P":
	fp.write(im.im.getpalette("BGRX"))

    ImageFile._save(im, fp, [("raw", (0,0)+im.size, 0, (rawmode, stride, -1))])

#
# --------------------------------------------------------------------
# Registry

Image.register_open(BmpImageFile.format, BmpImageFile, _accept)
Image.register_save(BmpImageFile.format, _save)

Image.register_extension(BmpImageFile.format, ".bmp")
