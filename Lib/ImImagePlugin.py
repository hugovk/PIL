#
# The Python Imaging Library.
# $Id: ImImagePlugin.py,v 1.4 1997/01/14 16:38:36 fl Exp $
#
# Restricted IFUNC IM file handling for PIL
#
# history:
#	95-09-01 fl	Created.  
#	97-01-03 fl	Save palette images
#	97-01-08 fl	Added sequence support
#
# Copyright (c) Fredrik Lundh 1995-97.  All rights reserved.
#


__version__ = "0.2"

import regex, string
import Image, ImageFile, ImagePalette


# --------------------------------------------------------------------
# Standard tags

COMMENT = "Comment"
DATE = "Date"
EQUIPMENT = "Digitalization equipment"
FRAMES = "File size (no of images)"
LUT = "Lut"
NAME = "Name"
SCALE = "Scale (x,y)"
SIZE = "Image size (x*y)"
MODE = "Image type"

TAGS = { COMMENT:0, DATE:0, EQUIPMENT:0, FRAMES:0, LUT:0, NAME:0,
	 SCALE:0, SIZE:0, MODE:0 }

OLDMODES = {
    "Greyscale image": "L", "Grayscale image": "L",
    "RGB image": "RGB;L", "RLB image": "RLB", "RYB image": "RLB",
    "B4 image": "P;4", "B2 image": "P;2", "B1 image": "1",
    "X 24 image": "RGB", "RYB3 image": "RYB;T"
}


# --------------------------------------------------------------------
# Read IM directory

split = regex.compile("^\([A-Za-z][^:]*\):[ \t]*\(.*\)[ \t]*$")

class ImImageFile(ImageFile.ImageFile):

    format = "IM"
    format_description = "IFUNC Image Memory"

    def _open(self):

	# Quick rejection: if there's not a LF among the first
	# 100 bytes, this is (probably) not a text header.

	if not "\n" in self.fp.read(100):
	    raise SyntaxError, "not an IM file"
	self.fp.seek(0)

	n = 0

	# Default values
	self.info[MODE] = "L"
	self.info[SIZE] = (512, 512)
	self.info[FRAMES] = 1

	while 1:

	    s = self.fp.read(1)

	    # Some drivers erronously uses \n\r instead of \r\n...
	    if s == "\r":
		continue

	    if not s or s[0] == chr(0) or s[0] == chr(26):
		break

	    # FIXME: risky; may read whole file if not a text file
	    s = s + self.fp.readline()

	    if len(s) > 100:
		raise SyntaxError, "not an IM file"

	    if s[-2:] == '\r\n':
		s = s[:-2]
	    elif s[-1:] == '\n':
		s = s[:-1]

	    try:
		hit = split.match(s)
	    except regex.error, v:
		raise SyntaxError, "not an IM file"

	    if hit >= 0:

		k, v = split.group(1,2)

		# Convert value as appropriate
		if k in [FRAMES, SCALE, SIZE]:
		    i = string.find(v, "*")
		    if i >= 0:
			v = v[:i] + "," + v[i+1:]
		    v = eval(v)
		elif k == MODE and OLDMODES.has_key(v):
		    v = OLDMODES[v]

		# Add to dictionary. Note that COMMENT tags are
		# combined into a list of strings.
		if k == COMMENT:
		    if self.info.has_key(k):
			self.info[k].append(v)
		    else:
			self.info[k] = [v]
		else:
		    self.info[k] = v

		if TAGS.has_key(k):
		    n = n + 1

	    else:

		raise SyntaxError, "Syntax error in IM header: " + s

	if not n:
	    raise SyntaxError, "Not an IM file"

	# Basic attributes
	self.size = self.info[SIZE]
	self.mode = self.info[MODE]

	# Pixel layout
	rawmode = self.mode
	if rawmode in ["RGB;L", "RGB;T", "RYB;T"]:
	    self.mode = "RGB"

	# Skip forward to start of image data
	while s and s[0] != chr(26):
	    s = self.fp.read(1)
	if not s:
	    return

	if self.info.has_key(LUT):
	    self.palette = ImagePalette.raw("RGB;L", self.fp.read(768))
	    if self.mode == "L":
		self.mode = "P"

	self.frame = 0
	self.rawmode = rawmode

	self.offset = offs = self.fp.tell()

	self.fp2 = self.fp # FIXME: hack

	if rawmode in ["RGB;T", "RYB;T"]:
	    # Old LabEye/3PC files.  Would be very surprised if anyone
	    # ever stumbled upon such a file ;-)
	    size = self.size[0] * self.size[1]
	    self.tile = [("raw", (0,0)+self.size, offs, ("G", 0, -1)),
			 ("raw", (0,0)+self.size, offs+size, ("R", 0, -1)),
			 ("raw", (0,0)+self.size, offs+2*size, ("B", 0, -1))]
	else:
	    # LabEye/IFUNC files
	    self.tile = [("raw", (0,0)+self.size, offs, (rawmode, 0, -1))]

    def seek(self, frame):
	
	if frame < 0 or frame >= self.info[FRAMES]:
	    raise EOFError, "seek outside sequence"

	if self.frame == frame:
	    return

	self.frame = frame

	if self.mode == "1":
	    bits = 1
	else:
	    bits = 8 * len(self.mode)

	size = ((self.size[0] * bits + 7) / 8) * self.size[1]
	offs = self.offset + frame * size

	self.fp = self.fp2

	self.tile = [("raw", (0,0)+self.size, offs, (self.rawmode, 0, -1))]

    def tell(self):
	
	return self.frame

#
# --------------------------------------------------------------------
# Save IM files

def _save(im, fp, filename):
    if im.mode not in ["L", "P"]:
	raise ValueError, "Cannot save %s images as IM" % im.mode

    fp.write("Image type: Greyscale image\r\n")
    fp.write("Name: %s\r\n" % filename)
    fp.write("Image size (x*y): %d*%d\r\n" % im.size)
    fp.write("File size (no of images): 1\r\n")
    if im.mode == "P":
	fp.write("Lut: 1\r\n")
    fp.write("\000" * (511-fp.tell()) + "\032")
    if im.mode == "P":
	fp.write(im.im.getpalette("RGB;L")) # 768 bytes, line-interleaved
    ImageFile._save(im, fp, [("raw", (0,0)+im.size, 0, ("L", 0, -1))])

#
# --------------------------------------------------------------------
# Registry

Image.register_open("IM", ImImageFile)
Image.register_save("IM", _save)

Image.register_extension("IM", ".im")
