#
# The Python Imaging Library.
# $Id: ImtImagePlugin.py,v 1.1 1996/12/16 18:50:23 fl Exp fl $
#
# IM Tools support for PIL
#
# history:
#	96-05-27 fl	Created (read 8-bit images only)
#
# Copyright (c) Fredrik Lundh 1996.  All rights reserved.
#


__version__ = "0.1"

import regex, string

import Image, ImageFile

#
# --------------------------------------------------------------------

field = regex.compile("\([a-z]*\) \([^ \r\n]*\)")

class ImtImageFile(ImageFile.ImageFile):

    format = "IMT"
    format_description = "IM Tools"

    def _open(self):

	xsize = ysize = 0

	while 1:

	    s = self.fp.read(1)
	    if not s:
		break

	    if s == chr(12):

		# image data begins
	        self.tile = [("raw", (0,0)+self.size,
			     self.fp.tell(),
		             (self.mode, 0, 1))]

		break

	    else:

		# read key/value pair
		s = s + self.fp.readline()
		if len(s) == 1 or len(s) > 100:
		    break
		if s[0] == "*":
		    continue # comment

		if field.match(s) < 0:
		    break
		k, v = field.group(1,2)
		if k == "width":
		    xsize = string.atoi(v)
		    self.size = xsize, ysize
		elif k == "height":
		    ysize = string.atoi(v)
		    self.size = xsize, ysize
		elif k == "pixel" and v == "n8":
		    self.mode = "L"
		    

#
# --------------------------------------------------------------------

Image.register_open("IMT", ImtImageFile)

#
# no extension registered (".im" is simply too common)
