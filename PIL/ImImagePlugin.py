#
# The Python Imaging Library.
# $Id: ImImagePlugin.py,v 1.2 1996/11/10 17:52:14 fredrik Exp $
#
# Restricted IFUNC IM file handling for PIL
#
# history:
#	95-09-01 fl	Created.  
#	97-01-03 fl	Save palette images
#	97-01-08 fl	Added sequence support
#	97-01-23 fl	Added P and RGB save support
#	97-05-31 fl	Read floating point images
#	97-06-22 fl	Save floating point images
#	97-08-27 fl	Read and save 1-bit images
#	98-03-05 fl	Read and save integer images
#
# Copyright (c) Secret Labs AB 1997-98.
# Copyright (c) Fredrik Lundh 1995-97.
#
# See the README file for information on usage and redistribution.
#


__version__ = "0.6"

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
    # standard IM formats
    "0 1 image": ("1", "1"),
    "L 1 image": ("1", "1"),
    "Greyscale image": ("L", "L"),
    "Grayscale image": ("L", "L"),
    "RGB image": ("RGB", "RGB;L"),
    "RLB image": ("RGB", "RLB"),
    "RYB image": ("RGB", "RLB"),
    "B1 image": ("1", "1"),
    "B2 image": ("P", "P;2"),
    "B4 image": ("P", "P;4"),
    "X 24 image": ("RGB", "RGB"),
    # P3CFUNC formats
    "RGB3 image": ("RGB", "RGB;T"),
    "RYB3 image": ("RGB", "RYB;T"),
}

# ifunc95 handlers
for i in ["8", "8S", "16", "16S", "32", "32F"]:
    OLDMODES["L %s image" % i] = ("F", "F;%s" % i)
    OLDMODES["L*%s image" % i] = ("F", "F;%s" % i)
for i in range(2, 33):
    OLDMODES["L*%s image" % i] = ("F", "F;%s" % i)

OLDMODES["L 32S image"] = ("I", "I")
OLDMODES["L*32S image"] = ("I", "I")

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

        self.rawmode = "L"

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
		    v, self.rawmode = OLDMODES[v]

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

	self.offset = offs = self.fp.tell()

	self.fp2 = self.fp # FIXME: hack

        if self.rawmode[:2] == "F;":

            # ifunc95 formats
            try:
                # use bit decoder (if necessary)
                bits = string.atoi(self.rawmode[2:])
                if bits not in [8, 16, 32]:
                    self.tile = [("bit", (0,0)+self.size, offs,
                                 (bits, 8, 3, 0, -1))]
                    return
            except ValueError:
                pass

	if self.rawmode in ["RGB;T", "RYB;T"]:
	    # Old LabEye/3PC files.  Would be very surprised if anyone
	    # ever stumbled upon such a file ;-)
	    size = self.size[0] * self.size[1]
	    self.tile = [("raw", (0,0)+self.size, offs, ("G", 0, -1)),
			 ("raw", (0,0)+self.size, offs+size, ("R", 0, -1)),
			 ("raw", (0,0)+self.size, offs+2*size, ("B", 0, -1))]
	else:
	    # LabEye/IFUNC files
	    self.tile = [("raw", (0,0)+self.size, offs, (self.rawmode, 0, -1))]

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

SAVE = {
    "1": ("0 1", "1"),
    "L": ("Greyscale", "L"),
    "P": ("Greyscale", "P"),
    "F": ("L 32F", "F"),
    "I": ("L 32S", "I"),
    "RGB": ("RGB", "RGB;L"),
    "RGBA": ("RGBA", "RGBA;L"),
    "CMYK": ("CMYK", "CMYK;L"),
}

def _save(im, fp, filename):

    try:
	type, rawmode = SAVE[im.mode]
    except KeyError:
	raise IOError, "Cannot save %s images as IM" % im.mode

    try:
	frames = im.encoderinfo["frames"]
    except KeyError:
	frames = 1

    fp.write("Image type: %s image\r\n" % type)
    if filename:
	fp.write("Name: %s\r\n" % filename)
    fp.write("Image size (x*y): %d*%d\r\n" % im.size)
    fp.write("File size (no of images): %d\r\n" % frames)
    if im.mode == "P":
	fp.write("Lut: 1\r\n")
    fp.write("\000" * (511-fp.tell()) + "\032")
    if im.mode == "P":
	# 768 bytes, line-interleaved
	fp.write(im.im.getpalette("RGB", "RGB;L"))
    ImageFile._save(im, fp, [("raw", (0,0)+im.size, 0, (rawmode, 0, -1))])

#
# --------------------------------------------------------------------
# Registry

Image.register_open("IM", ImImageFile)
Image.register_save("IM", _save)

Image.register_extension("IM", ".im")
