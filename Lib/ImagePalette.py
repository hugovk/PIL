#
# The Python Imaging Library.
# $Id: ImagePalette.py,v 1.5 1997/01/14 16:38:36 fl Exp $
#
# image palette object
#
# History:
#	96-03-11 fl	Rewritten.
#	97-01-03 fl	Up and running.
#
# Copyright (c) Fredrik Lundh 1996-97.  All rights reserved.
#

import array
import Image

class ImagePalette:
    "Colour palette for palette mapped images"

    def __init__(self, mode = "RGB", palette = None):
        self.mode = mode
        self.palette = palette or range(256)*len(self.mode)
	if len(self.mode)*256 != len(self.palette):
	    raise ValueError, "wrong palette size"

    def tostring(self):
        return array.array("b", self.palette).tostring()

    def save(self, fp):
        if type(fp) == type(""):
	    fp = open(fp, "w")
	fp.write("# Palette\n")
	fp.write("# Mode: %s\n" % self.mode)
	for i in range(256):
	    fp.write("%d" % i)
	    for j in range(i, len(self.palette), 256):
	        fp.write(" %d" % self.palette[j])
	    fp.write("\n")
        fp.close()

# --------------------------------------------------------------------
# Internal

class raw:
    def __init__(self, rawmode, data):
	self.rawmode = rawmode
	self.data = data

# --------------------------------------------------------------------
# Factories

def new(mode, data):
    Image.core.new_palette(mode, data)

def negative(mode = "RGB"):
    palette = range(256)
    palette.reverse()
    return ImagePalette(mode, palette * len(mode))

def random(mode = "RGB"):
    from whrandom import randint
    palette = map(lambda a: randint(0, 255), [0]*256*len(mode))
    return ImagePalette(mode, palette)

def wedge(mode = "RGB"):
    return ImagePalette(mode, range(256) * len(mode))

# add some psuedocolour palettes as well
