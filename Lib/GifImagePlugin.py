#
# The Python Imaging Library.
# $Id: GifImagePlugin.py,v 1.7 1996/12/16 18:50:23 fl Exp fl $
#
# GIF file handling
#
# History:
#	95-09-01 fl	Created
#	96-12-14 fl	Added interlace support
#
# Copyright (c) Fredrik Lundh 1995.  All rights reserved.
#


__version__ = "0.2"


import array
import Image, ImageFile


# --------------------------------------------------------------------
# Helpers

def i16(c):
    return ord(c[0]) + (ord(c[1])<<8)


# --------------------------------------------------------------------
# Identify/read GIF files

class GifImageFile(ImageFile.ImageFile):

    format = "GIF"
    format_description = "Compeserve GIF"

    def data(self):
	s = self.fp.read(1)
	if s and ord(s):
	    return self.fp.read(ord(s))
	return None

    def _open(self):

        first_only = 0

	# Screen
	s = self.fp.read(13)
	if s[:6] not in ["GIF87a", "GIF89a"]:
	    raise SyntaxError, "not a GIF file"
	self.info["version"] = s[:6]

	self.size = i16(s[6:]), i16(s[8:])

	self.tile = []

	flag = ord(s[10])
	bits = (flag & 7) + 1

	# Global palette
	if ord(s[10]) & 128:
	    self.info["background"] = ord(s[11])
	    self.global_palette = self.palette = "RGB", self.fp.read(3<<bits)

	while 1:
	    s = self.fp.read(1)
	    if not s or s == ";":
		break
	    elif s == "!":
		s = self.fp.read(1)
		# print "GIF EXTENSION", ord(s)
		block = self.data()
		if ord(s) == 249:
		    # graphic control extension
		    # FIXME! extract transparency etc
		    pass
		elif ord(s) == 255:
		    # application extension
		    self.info["extension"] = block, self.fp.tell()
		    if block[:11] == "NETSCAPE2.0":
			self.info["loop"] = 1 # FIXME
		while self.data():
		    pass
	    elif s == ",":
		# Local image
		s = self.fp.read(9)
		x0, y0, x1, y1 = i16(s[0:]), i16(s[2:]), i16(s[4:]), i16(s[6:])
		#x1, y1 = x0+x1, y0+y1
		flag = ord(s[8])
		interlace = flag & 64
		if flag & 128:
		    # Local palette
		    palette = "RGB", self.fp.read(3<<bits)
		bits = ord(self.fp.read(1))
		self.tile.append("gif",
				 (x0, y0, x1, y1),
				 self.fp.tell(),
				 (bits, interlace))
		if first_only:
		    break
		while self.data():
		    pass
	    else:
		pass
		# raise IOError, "illegal GIF tag `%x`" % ord(s)

	if not self.tile:
	    raise IOError, "no images in GIF file"

	self.mode = "L"
	if self.palette:
	    self.mode = "P"

def _save_hack(im, fp, filename):
    import os
    file = im._dump()
    if im.mode != "RGB":
	os.system("ppmtogif %s >%s" % (file, filename))
    else:
	os.system("ppmquant 256 %s | ppmtogif >%s" % (file, filename))
    try: os.unlink(file)
    except: pass

# --------------------------------------------------------------------
# Registry

Image.register_open(GifImageFile.format, GifImageFile)
Image.register_save(GifImageFile.format, _save_hack)

Image.register_extension(GifImageFile.format, ".gif")

Image.register_mime(GifImageFile.format, "image/gif")
