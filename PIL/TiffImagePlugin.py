#
# The Python Imaging Library.
# $Id: TiffImagePlugin.py,v 1.4 1996/11/10 17:52:14 fredrik Exp $
#
# TIFF file handling
#
# TIFF is a flexible, if somewhat aged, image file format
# originally defined by Aldus.  Although TIFF supports a wide
# variety of pixel layouts and compression methods, the name
# doesn't really stand for "thousands of incompatible file
# formats," it just feels that way.
#
# To read TIFF data from a stream, the stream must be seekable.
# For progressive decoding, make sure to use TIFF files where
# the tag directory is placed first in the file.
#
# Notes:
#	save(tiff) really needs more work:
#	- data is currently written in a single strip
#	- the "bits per sample" tag is not correctly written (?)
#	  when "samples per pixel" is not 1
#	- the colormap is not written for palette images
#
# History:
#	95-09-01 fl	Created
#	96-05-04 fl	Handle JpegTables tag (tag 347, experimental)
#	96-05-18 fl	Fixed palette support (tag 320)
#	97-01-05 fl	Fixed predictor support (tag 317)
#	97-08-27 fl	Added support for rational tags (from Perry Stoll)
#	98-01-10 fl	Fixed seek/tell (from Jan Blom)
#
# Copyright (c) Secret Labs AB 1997-98.
# Copyright (c) Fredrik Lundh 1995-97.
#
# See the README file for information on usage and redistribution.
#

__version__ = "0.5"

import Image, ImageFile, ImagePalette

import string

#
# --------------------------------------------------------------------
# Read TIFF files

def l16(c):
    return ord(c[0]) + (ord(c[1])<<8)
def l32(c):
    return ord(c[0]) + (ord(c[1])<<8) + (ord(c[2])<<16) + (ord(c[3])<<24)

def b16(c):
    return ord(c[1]) + (ord(c[0])<<8)
def b32(c):
    return ord(c[3]) + (ord(c[2])<<8) + (ord(c[1])<<16) + (ord(c[0])<<24)


COMPRESSION = {
    # mapped from Compression (tag 259)
    (1,): "raw",
    (2,): "tiff_ccitt",
    (3,): "group3",
    (4,): "group4",
    (5,): "tiff_lzw",
    (6,): "tiff_jpeg", # obsolete
    (7,): "jpeg",
    (32771,): "tiff_raw_16", # 16-bit padding
    (32773,): "packbits"
}

OPEN = {
    # mapped from Photometric Interpretation (262) and BitsPerSample (258)
    (0, (1,)): ("1", "1;I"),
    (0, (8,)): ("L", "L;I"),
    (1, (1,)): ("1", "1"),
    (1, (8,)): ("L", "L"),
    (1, (16,)): ("L", "L;16B"),
    (2, (8,8,8)): ("RGB", "RGB"),
    (2, (8,8,8,8)): ("RGBA", "RGBA"),
    (2, (16,16,16)): ("RGB", "RGB;16B"),
    (2, (16,16,16,16)): ("RGB", "RGBA;16B"),
    (3, (1,)): ("P", "P;1"),
    (3, (2,)): ("P", "P;2"),
    (3, (4,)): ("P", "P;4"),
    (3, (8,)): ("P", "P"),
    (5, (8,8,8,8)): ("CMYK", "CMYK"),
    (6, (8,8,8)): ("RGB", "RGB"), # YCC, actually
}


def _accept(prefix):
    return prefix[:2] in ["MM", "II"]


class TiffImageFile(ImageFile.ImageFile):

    format = "TIFF"
    format_description = "Aldus TIFF"

    def _open(self):
	"Open the first image in a TIFF file"

	# Header
	s = self.fp.read(8)
	if s[:2] == "MM":
	    self.i16, self.i32 = b16, b32
	elif s[:2] == "II":
	    self.i16, self.i32 = l16, l32
	else:
	    raise SyntaxError, "not a TIFF file"
	if self.i16(s[2:]) != 42:
	    raise SyntaxError, "unknown TIFF version"

	self._first = self._next = self.i32(s[4:])
	self._frame = 0

	self.fp2 = self.fp # FIXME: hack

	self._seek(0)

    def seek(self, frame):
	"Select a given frame as current image"

	self._seek(frame)
	
    def tell(self):
	"Return the current frame number"

	return self._tell()

    def _seek(self, frame):

	self.fp = self.fp2
	if frame < self._frame:
	    # rewind file
	    self._frame = 0
	    self._next = self._first
	while self._frame < frame:
	    self._gettags()
	    self._frame = self._frame + 1
	self._gettags()
	self._setup()

    def _tell(self):

	return self._frame

    def _decoder(self, rawmode, layer):
	"Setup decoder contexts"

	args = None
	if rawmode == "P":
	    rawmode = "L"
	if rawmode == "RGB" and self._planar_configuration == 2:
	    rawmode = rawmode[layer]
	if self._compression == "raw":
	    args = (rawmode, 0, 1)
	if self._compression in ["packbits", "tiff_lzw", "jpeg"]:
	    args = rawmode
	    if self._compression == "jpeg" and self.tag.has_key(347):
		# Hack to handle abbreviated JPEG headers
		self.tile_prefix = self.tag[347]
	    elif self._compression == "tiff_lzw" and self.tag.has_key(317):
		# Section 14: Differencing Predictor
		self.decoderconfig = (self.tag[317][0],)

	return args

    def _setup(self):
	"Setup this image object based on current tags"

	# extract relevant tags
	self._compression = COMPRESSION[self.tag[259]]
	self._photometric_interpretation = self.tag[262][0]
	self._planar_configuration = self.tag[284][0]

	if Image.DEBUG:
	    print "*** Summary ***"
	    print "- compression:", self._compression
	    print "- photometric_interpretation:", self._photometric_interpretation
	    print "- planar_configuration:", self._planar_configuration

	# size
	xsize, ysize = self.tag[256][0], self.tag[257][0]
	self.size = xsize, ysize

	if Image.DEBUG:
	    print "- size:", self.size

	# mode: check photometric interpretation and bits per pixel
	try:
	    self.mode, rawmode = OPEN[(self._photometric_interpretation, 
					self.tag[258])]
	except KeyError:
	    if Image.DEBUG:
		print "- unknown photometric interpretation"
	    raise SyntaxError, "unknown pixel mode"

	if Image.DEBUG:
	    print "- raw mode:", rawmode
	    print "- pil mode:", self.mode

	self.info["compression"] = self._compression

	# build tile descriptors
	x = y = l = 0
	self.tile = []
	if self.tag.has_key(273):
	    # striped image
	    h = self.tag[278][0]
	    w = self.size[0]
	    a = None
	    for o in self.tag[273]:
		if not a:
		    a = self._decoder(rawmode, l)
		self.tile.append(self._compression,
				 (0, min(y, ysize), w, min(y+h, ysize)),
				 o, a)
		y = y + h
		if y >= self.size[1]:
		    x = y = 0
		    l = l + 1
		    a = None
	elif self.tag.has_key(324):
	    # tiled image
	    w = self.tag[322][0]
	    h = self.tag[323][0]
	    a = None
	    for o in self.tag[324]:
		if not a:
		    a = self._decoder(rawmode, l)
		# FIXME: this doesn't work if the image size
		# is not a multiple of the tile size...
		self.tile.append(self._compression,
				 (x, y, x+w, y+h),
				 o, a)
		x = x + w
		if x >= self.size[0]:
		    x, y = 0, y + h
		    if y >= self.size[1]:
			x = y = 0
			l = l + 1
			a = None
	else:
	    if Image.DEBUG:
		print "- unknown data organization"
	    raise SyntaxError, "unknown data organization"

	# fixup palette descriptor
	if self.mode == "P":
	    palette = map(lambda a: chr(a / 256), self.tag[320])
	    self.palette = ImagePalette.raw("RGB;L", string.join(palette, ""))

    # ----------------------------------------------------------------
    # TIFF tag parser

    TYPES = {}

    def unpackString(self, data):
	if data[-1:] == '\0':
	    data = data[:-1]
	return data
    TYPES[2] = (1, unpackString)

    def unpackShort(self, data):
	l = []
        for i in range(0, len(data), 2):
	    l.append(self.i16(data[i:i+2]))
	return tuple(l)
    TYPES[3] = (2, unpackShort)

    def unpackLong(self, data):
	l = []
        for i in range(0, len(data), 4):
	    l.append(self.i32(data[i:i+4]))
	return tuple(l)
    TYPES[4] = (4, unpackLong)

    def unpackRational(self, data):
        l = []
        for i in range(0, len(data), 8):
            num   = self.i32(data[i:i+4])
            denom = self.i32(data[i+4:i+8])
        if denom == 0:
            l.append(1.0)
        else:
            l.append(float(num)/float(denom))
        return tuple(l)
    TYPES[5] = (8, unpackRational)

    def unpackUndefined(self, data):
	# Untyped data
	return data
    TYPES[7] = (1, unpackUndefined)

    def _gettags(self):
	"Load a tag directory"

	if not self._next:
	    raise EofError, "no more TIFF directories"

	self.fp.seek(self._next)

	# Setup default tag values
	self.tag = {
	    259: (1,),
	    284: (1,),
	}

	# Load dictionary
	for i in range(self.i16(self.fp.read(2))):

	    s = self.fp.read(12)
	    tag, type = self.i16(s), self.i16(s[2:])

	    if Image.DEBUG:
		print "found", tag, type

	    try:
	        size = self.TYPES[type][0] * self.i32(s[4:])
	    except KeyError:
		if Image.DEBUG:
		    print "- unknown type", type
	        continue # ignore unknown type

	    # Get and expand tag value
	    if size > 4:
		here = self.fp.tell()
	        self.fp.seek(self.i32(s[8:]))
		data = self.fp.read(size)
		self.fp.seek(here)
	    else:
	        data = s[8:8+size]

	    self.tag[tag] = self.TYPES[type][1](self, data)

	s = self.fp.read(4)
	self._next = self.i32(s[0:])


#
# --------------------------------------------------------------------
# Write TIFF files

# little endian is default

def o16(i):
    return chr(i&255) + chr(i>>8&255)

def o32(i):
    return chr(i&255) + chr(i>>8&255) + chr(i>>16&255) + chr(i>>24&255)

SAVE = {
    # mode: rawmode, bits per pixel, photometric interpretation
    "1": ("1", 1, 1),
    "L": ("L", 8, 1),
    "RGB": ("RGB", 24, 2),
    "P": ("P", 8, 3),
    "A": ("L", 8, 4),
    "CMYK": ("CMYK", 32, 5),
    "YCC": ("YCC", 24, 6),
    "LAB": ("LAB", 24, 8),
}

def _save(im, fp, filename):

    try:
	rawmode, bits, photo = SAVE[im.mode]
    except KeyError:
	raise IOError, "cannot write mode %s as TIFF" % im.mode

    # tiff header
    fp.write("II" +			# intel byte order
	     o16(42) +			# version number
	     o32(8))			# offset to first tag directory

    tags = []

    # size
    tags.append(256, im.size[0])	# width
    tags.append(257, im.size[1])	# length

    # mode
    if bits == 24:
	tags.append(258, 8)		# bits per sample
	tags.append(277, 3)		# samples per pixel
    elif bits == 32:
	tags.append(258, 8)		# bits per sample
	tags.append(277, 4)		# samples per pixel
    else:
	tags.append(258, bits)
	tags.append(277, 1)
    if photo != 1:
	tags.append(262, photo)		# photometric interpretation
    #if im.mode == "P":
    #	tags.append(320, None)		# colormap

    # data orientation
    stride = (im.size[0]*bits+7)/8
    tags.append(278, im.size[1])        # rows per strip (full image)
    tags.append(279, stride*im.size[1]) # strip byte count

    # strip offset (must be appended last)
    tags.append(273, 8 + 2 + len(tags) * 12 + 12 + 4)

    fp.write(o16(len(tags)))

    tags.sort() # write tags in correct order
    for id, value in tags:
	if value < 65536:
	    fp.write(o16(id) + o16(3) + o32(1) + o32(value))
	else:
	    fp.write(o16(id) + o16(4) + o32(1) + o32(value))

    # end of IFD
    fp.write("\000" * 4)

    ImageFile._save(im, fp, [("raw", (0,0)+im.size, 0, (rawmode, 0, 1))])

#
# --------------------------------------------------------------------
# Register

Image.register_open("TIFF", TiffImageFile, _accept)
Image.register_save("TIFF", _save)

Image.register_extension("TIFF", ".tif")
Image.register_extension("TIFF", ".tiff")

Image.register_mime("TIFF", "image/tiff")
