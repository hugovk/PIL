#
# The Python Imaging Library.
# $Id: XbmImagePlugin.py,v 1.3 1996/11/10 17:52:14 fredrik Exp $
#
# XBM File handling
#
# History:
#	95-09-08 fl	Created
#	96-11-01 fl	Added save support
#
# Copyright (c) Fredrik Lundh 1996.  All rights reserved.
#


__version__ = "0.1"


import regex, string
import Image, ImageFile

# XBM header
xbm_head = regex.compile("^#define [^_]*_width \([0-9]*\)\n"
			 "#define [^_]*_height \([0-9]*\)\n"
			 ".*char [^_]*_bits\[\]")

class XbmImageFile(ImageFile.ImageFile):

    format = "XBM"
    format_description = "X11 Bitmap"

    def _open(self):

	s = xbm_head.match(self.fp.read(512))

	if s > 0:

	    xsize = string.atoi(xbm_head.group(1))
	    ysize = string.atoi(xbm_head.group(2))

	    self.mode = "1"
	    self.size = xsize, ysize

	    self.tile = [("xbm", (0, 0)+self.size, s, None)]


def _save(im, fp, filename):

    if im.mode != "1":
	raise IOError, "cannot write mode %s as BMP" % im.mode

    fp.write("#define im_width %d\n" % im.size[0])
    fp.write("#define im_height %d\n" % im.size[1])
    fp.write("static char im_bits[] = {\n")

    ImageFile._save(im, fp, [("xbm", (0,0)+im.size, 0, None)])

    fp.write("};\n")


Image.register_open("XBM", XbmImageFile)
Image.register_save("XBM", _save)

Image.register_extension("XBM", ".xbm")

Image.register_mime("XBM", "image/xbm")
