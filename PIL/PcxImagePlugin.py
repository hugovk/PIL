#
# The Python Imaging Library.
# $Id: PcxImagePlugin.py,v 1.4 1996/11/10 17:52:14 fredrik Exp $
#
# PCX file handling
#
# This format was used by the popular PaintBrush applications, from
# ZSoft, Inc.  It's also support by many MS-DOS and Windows applications,
# including the PaintBrush program in Windows 3.
#
# History:
#	95-09-01 fl	Created
#	96-05-20 fl	Fixed RGB support
#	97-01-03 fl	Fixed 2-bit and 4-bit support
#
# Copyright (c) Secret Labs AB 1997.
# Copyright (c) Fredrik Lundh 1995-97.
#
# See the README file for information on usage and redistribution.
#


__version__ = "0.1"


import array
import Image, ImageFile, ImagePalette


def i16(c):
    return ord(c[0]) + (ord(c[1])<<8)

def _accept(prefix):
    return ord(prefix[0]) == 10 and ord(prefix[1]) in [0, 2, 3, 5]

class PcxImageFile(ImageFile.ImageFile):

    format = "PCX"
    format_description = "Paintbrush"

    def _open(self):

	# Header
	s = self.fp.read(128)
	if not _accept(s):
	    raise SyntaxError, "not a PCX file"

	# Image
	box = i16(s[4:]), i16(s[6:]), i16(s[8:])+1, i16(s[10:])+1
	if box[2] <= box[0] or box[3] <= box[1]:
	    raise SyntaxError, "bad PCX image size"

	# Format
	bits, planes, stride = ord(s[3]), ord(s[65]), i16(s[66:])
	if bits == 1 and planes == 1:
	    self.mode = rawmode = "1"
	elif bits == 1 and planes == 2:
	    self.mode = "P"
	    rawmode = "P;2L"
	    self.palette = ImagePalette.raw("RGB", s[16:64])
	elif bits == 1 and planes == 4:
	    self.mode = "P"
	    rawmode = "P;4L"
	    self.palette = ImagePalette.raw("RGB", s[16:64])
	elif bits == 8 and planes == 1:
	    self.mode = rawmode = "L"
	    # FIXME: hey, this doesn't work with the incremental loader !!!
	    # FIXME: should perhaps check version before looking for a palette
	    self.fp.seek(-769, 2)
	    s = self.fp.read(769)
	    if ord(s[0]) == 12:
		self.palette = ImagePalette.raw("RGB", s[1:])
		self.mode = "P"
	elif bits == 8 and planes == 3:
	    self.mode = "RGB"
	    rawmode = "RGB;L"
	else:
	    raise IOError, "unknown PCX mode"

	self.size = box[2]-box[0], box[3]-box[1]

	self.tile = [("pcx", box, 128, rawmode)]

#
# registry

Image.register_open("PCX", PcxImageFile, _accept)

Image.register_extension("PCX", ".pcx")
