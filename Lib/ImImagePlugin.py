#
# The Python Imaging Library.
# $Id: ImImagePlugin.py,v 1.3 1996/12/16 18:50:23 fl Exp fl $
#
# Restricted IFUNC IM file handling for PIL
#
# history:
#	95-09-01 fl	Created.  
#
# Copyright (c) Fredrik Lundh 1995.  All rights reserved.
#


__version__ = "0.1"

import regex, string
import Image, ImageFile


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
	    self.palette = "RGB;L", self.fp.read(768)
	    if self.mode == "L":
		self.mode = "P"

	offset = self.fp.tell()

	if rawmode in ["RGB;T", "RYB;T"]:
	    # Old LabEye files.  Would be very surprised if anyone
	    # would ever stumble upon such a file...
	    size = self.size[0] * self.size[1]
	    self.tile = [("raw", (0,0)+self.size, offset, ("G", 0, -1)),
			 ("raw", (0,0)+self.size, offset+size, ("R", 0, -1)),
			 ("raw", (0,0)+self.size, offset+2*size, ("B", 0, -1))]
	else:
	    self.tile = [("raw", (0,0)+self.size, offset, (rawmode, 0, -1))]

#
# --------------------------------------------------------------------
# Save IM files

def _save(im, fp, filename):
    if im.mode != "L":
	raise ValueError, "Can only save greyscale images"

    fp.write("Image type: Greyscale image\r\n")
    fp.write("Name: %s\r\n" % filename)
    fp.write("Image size (x*y): %d*%d\r\n" % im.size)
    fp.write("File size (no of images): 1\r\n")
    fp.write("\000" * (511-fp.tell()) + "\032")
    ImageFile._save(im, fp, [("raw", (0,0)+im.size, 0, ("L", 0, -1))])

#
# --------------------------------------------------------------------
# Registry

Image.register_open("IM", ImImageFile)
Image.register_save("IM", _save)

Image.register_extension("IM", ".im")
