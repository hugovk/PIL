#
# The Python Imaging Library.
# $Id: MpegImagePlugin.py,v 1.4 1997/01/14 16:38:36 fl Exp $
#
# MPEG file handling
#
# History:
#	95-09-09 fl	Created
#
# Copyright (c) Fredrik Lundh 1995.  All rights reserved.
#

__version__ = "0.1"

import array, string
import Image, ImageFile

#
# Bitstream parser

class BitStream:

    def __init__(self, fp):
	self.fp = fp
	self.bits = 0
	self.bitbuffer = 0

    def next(self):
	return ord(self.fp.read(1))

    def peek(self, bits):
	while self.bits < bits:
	    c = self.next()
	    if c < 0:
		self.bits = 0
		continue
	    self.bitbuffer = (self.bitbuffer << 8) + c
	    self.bits = self.bits + 8
	return self.bitbuffer >> (self.bits - bits) & (1 << bits) - 1

    def skip(self, bits):
	while self.bits < bits:
	    self.bitbuffer = (self.bitbuffer << 8) + ord(self.fp.read(1))
	    self.bits = self.bits + 8
	self.bits = self.bits - bits

    def read(self, bits):
	v = self.peek(bits)
	self.bits = self.bits - bits
	return v


class MpegImageFile(ImageFile.ImageFile):

    format = "MPEG"
    format_description = "MPEG"

    def _open(self):

	s = BitStream(self.fp)

	if s.read(32) != 0x1B3:
	    raise SyntaxError, "not an MPEG file"

	self.mode = "RGB"
	self.size = s.read(12), s.read(12)


# --------------------------------------------------------------------
# Registry stuff

Image.register_open("MPEG", MpegImageFile)

Image.register_extension("MPEG", ".mpg")
Image.register_extension("MPEG", ".mpeg")

Image.register_mime("MPEG", "video/mpeg")
