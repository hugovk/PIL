#
# The Python Imaging Library.
# $Id$
#
# TIFF file handling
#
# TIFF is a flexible, if somewhat aged, image file format originally
# defined by Aldus.  Although TIFF supports a wide variety of pixel
# layouts and compression methods, the name doesn't really stand for
# "thousands of incompatible file formats," it just feels that way.
#
# To read TIFF data from a stream, the stream must be seekable.  For
# progressive decoding, make sure to use TIFF files where the tag
# directory is placed first in the file.
#
# History:
# 95-09-01 fl	Created
# 96-05-04 fl	Handle JPEGTABLES tag
# 96-05-18 fl	Fixed COLORMAP support
# 97-01-05 fl	Fixed PREDICTOR support
# 97-08-27 fl	Added support for rational tags (from Perry Stoll)
# 98-01-10 fl	Fixed seek/tell (from Jan Blom)
# 98-07-15 fl	Use private names for internal variables
# 99-06-13 fl	Rewritten for PIL 1.0
# 00-10-11 fl	Additional fixes for Python 2.0
#
# Copyright (c) Secret Labs AB 1997-2000
# Copyright (c) Fredrik Lundh 1995-2000
#
# See the README file for information on usage and redistribution.
#

__version__ = "1.0"

import Image, ImageFile
import ImagePalette

import string

#
# --------------------------------------------------------------------
# Read TIFF files

def il16(c,o=0):
    return ord(c[o]) + (ord(c[o+1])<<8)
def il32(c,o=0):
    return ord(c[o]) + (ord(c[o+1])<<8) + (ord(c[o+2])<<16) + (ord(c[o+3])<<24)
def ol16(i):
    return chr(i&255) + chr(i>>8&255)
def ol32(i):
    return chr(i&255) + chr(i>>8&255) + chr(i>>16&255) + chr(i>>24&255)

def ib16(c,o=0):
    return ord(c[o+1]) + (ord(c[o])<<8)
def ib32(c,o=0):
    return ord(c[o+3]) + (ord(c[o+2])<<8) + (ord(c[o+1])<<16) + (ord(c[o])<<24)

# a few tag names, just to make the code below a bit more readable
IMAGEWIDTH = 256
IMAGELENGTH = 257
BITSPERSAMPLE = 258
COMPRESSION = 259
PHOTOMETRIC_INTERPRETATION = 262
IMAGEDESCRIPTION = 270
STRIPOFFSETS = 273
SAMPLESPERPIXEL = 277
ROWSPERSTRIP = 278
STRIPBYTECOUNTS = 279
PLANAR_CONFIGURATION = 284
PREDICTOR = 317
COLORMAP = 320
EXTRASAMPLES = 338
SAMPLEFORMAT = 339
JPEGTABLES = 347
IPTC_NAA_CHUNK = 33723 # newsphoto properties
PHOTOSHOP_CHUNK = 34377 # photoshop properties

COMPRESSION_INFO = {
    # Compression => pil compression name
    1: "raw",
    2: "tiff_ccitt",
    3: "group3",
    4: "group4",
    5: "tiff_lzw",
    6: "tiff_jpeg", # obsolete
    7: "jpeg",
    32771: "tiff_raw_16", # 16-bit padding
    32773: "packbits"
}

OPEN_INFO = {
    # Photometric Interpretation, SampleFormat, BitsPerSample, ExtraSamples =>
    # mode, rawmode
    (0, 1, (1,), ()): ("1", "1;I"),
    (0, 1, (8,), ()): ("L", "L;I"),
    (1, 1, (1,), ()): ("1", "1"),
    (1, 1, (8,), ()): ("L", "L"),
    (1, 2, (16,), ()): ("I;16", "I;16"),
    (1, 2, (32,), ()): ("I", "I;32S"),
    (1, 3, (32,), ()): ("F", "F;32F"),
    (2, 1, (8,8,8), ()): ("RGB", "RGB"),
    (2, 1, (8,8,8,8), (0,)): ("RGBX", "RGBX"),
    (2, 1, (8,8,8,8), (2,)): ("RGBA", "RGBA"),
    (3, 1, (1,), ()): ("P", "P;1"),
    (3, 1, (2,), ()): ("P", "P;2"),
    (3, 1, (4,), ()): ("P", "P;4"),
    (3, 1, (8,), ()): ("P", "P"),
    (5, 1, (8,8,8,8), ()): ("CMYK", "CMYK"),
    (6, 1, (8,8,8), ()): ("YCbCr", "YCbCr"),
    (8, 1, (8,8,8), ()): ("LAB", "LAB"),
}

PREFIXES = ["MM\000\052", "II\052\000"]

def _accept(prefix):
    return prefix[:4] in PREFIXES


class ImageFileDirectory:

    # represents a TIFF tag directory.  to speed things up,
    # we don't decode tags unless they're asked for.

    def __init__(self, prefix="II"):
	assert prefix in ("MM", "II")
	self.prefix = prefix
	if prefix == "MM":
	    self.i16, self.i32 = ib16, ib32
	    # FIXME: save doesn't yet support big-endian mode...
	else:
	    self.i16, self.i32 = il16, il32
	    self.o16, self.o32 = ol16, ol32
	self.reset()

    def reset(self):
	self.tags = {}
	self.tagdata = {}
	self.next = None

    # dictionary API (sort of)

    def __len__(self):
	return max(len(self.tagdata), len(self.tags))

    def __getitem__(self, tag):
	try:
	    return self.tags[tag]
	except KeyError:
	    type, data = self.tagdata[tag] # unpack on the fly
	    size, handler = self.load_dispatch[type]
	    self.tags[tag] = data = handler(self, data)
	    del self.tagdata[tag]
	    return data

    def get(self, tag, default=None):
	try:
	    return self[tag]
	except KeyError:
	    return default

    def getscalar(self, tag, default=None):
	try:
	    value = self[tag]
	    if len(value) != 1:
		raise ValueError, "not a scalar"
	    return value[0]
	except KeyError:
	    if default is None:
		raise
	    return default

    def has_key(self, tag):
	return self.tags.has_key(tag) or self.tagdata.has_key(tag)

    def __setitem__(self, tag, value):
	if type(value) is not type(()):
	    value = (value,)
	self.tags[tag] = value

    # load primitives

    load_dispatch = {}

    def load_byte(self, data):
	l = []
        for i in range(len(data)):
	    l.append(ord(data[i]))
	return tuple(l)
    load_dispatch[1] = (1, load_byte)

    def load_string(self, data):
	if data[-1:] == '\0':
	    data = data[:-1]
	return data
    load_dispatch[2] = (1, load_string)

    def load_short(self, data):
	l = []
        for i in range(0, len(data), 2):
	    l.append(self.i16(data, i))
	return tuple(l)
    load_dispatch[3] = (2, load_short)

    def load_long(self, data):
	l = []
        for i in range(0, len(data), 4):
	    l.append(self.i32(data, i))
	return tuple(l)
    load_dispatch[4] = (4, load_long)

    def load_rational(self, data):
        l = []
        for i in range(0, len(data), 8):
            l.append((self.i32(data, i), self.i32(data, i+4)))
        return tuple(l)
    load_dispatch[5] = (8, load_rational)

    def load_undefined(self, data):
	# Untyped data
	return data
    load_dispatch[7] = (1, load_undefined)

    def load(self, fp):
	# load tag dictionary

	self.reset()

	i16 = self.i16
	i32 = self.i32

	for i in range(i16(fp.read(2))):

	    ifd = fp.read(12)

	    tag, typ = i16(ifd), i16(ifd, 2)

	    if Image.DEBUG:
		import TiffTags
		tagname = TiffTags.TAGS.get(tag, "unknown")
		typname = TiffTags.TYPES.get(typ, "unknown")
		print "tag: %s (%d)" % (tagname, tag),
		print "- type: %s (%d)" % (typname, typ),

	    try:
		dispatch = self.load_dispatch[typ]
	    except KeyError:
		if Image.DEBUG:
		    print "- unsupported type", typ
	        continue # ignore unsupported type

	    size, handler = dispatch

	    size = size * i32(ifd, 4)

	    # Get and expand tag value
	    if size > 4:
		here = fp.tell()
	        fp.seek(i32(ifd, 8))
		data = fp.read(size)
		fp.seek(here)
	    else:
	        data = ifd[8:8+size]

	    if len(data) != size:
		raise IOError, "not enough data"

	    self.tagdata[tag] = typ, data

	    if Image.DEBUG:
		if tag in (COLORMAP, IPTC_NAA_CHUNK, PHOTOSHOP_CHUNK):
		    print "- value: <table: %d bytes>" % size
		else:
		    print "- value:", self[tag]

	self.next = i32(fp.read(4))

    # save primitives

    def save(self, fp):
	
	o16 = self.o16
	o32 = self.o32

	fp.write(o16(len(self.tags)))

	# always write in ascending tag order
	tags = self.tags.items()
	tags.sort()

	directory = []
	append = directory.append

	offset = fp.tell() + len(self.tags) * 12 + 4

	stripoffsets = None

	# pass 1: convert tags to binary format
	for tag, value in tags:

	    if Image.DEBUG:
		import TiffTags
		tagname = TiffTags.TAGS.get(tag, "unknown")
		print "save: %s (%d)" % (tagname, tag),
		print "- value:", value

	    if type(value[0]) is type(""):
		# string data
		typ = 2
		data = value = string.join(value, "\0") + "\0"

	    else:
		# integer data
		if tag == STRIPOFFSETS:
		    stripoffsets = len(directory)
		    typ = 4 # to avoid catch-22
		else:
		    typ = 3
		    for v in value:
			if v >= 65536:
			    typ = 4
		if typ == 3:
		    data = string.join(map(o16, value), "")
		else:
		    data = string.join(map(o32, value), "")

	    # figure out if data fits into the directory
	    if len(data) == 4:
		append((tag, typ, len(value), data, ""))
	    elif len(data) < 4:
		append((tag, typ, len(value), data + (4-len(data))*"\0", ""))
	    else:
		append((tag, typ, len(value), o32(offset), data))
		offset = offset + len(data)
		if offset & 1:
		    offset = offset + 1 # word padding

	# update strip offset data to point beyond auxiliary data
	if stripoffsets is not None:
	    tag, typ, count, value, data = directory[stripoffsets]
	    assert not data, "multistrip support not yet implemented"
	    value = o32(self.i32(value) + offset)
	    directory[stripoffsets] = tag, typ, count, value, data

	# pass 2: write directory to file
	for tag, typ, count, value, data in directory:
	    if Image.DEBUG > 1:
		print tag, typ, count, repr(value), repr(data)
	    fp.write(o16(tag) + o16(typ) + o32(count) + value)
	fp.write("\0\0\0\0") # end of directory

	# pass 3: write auxiliary data to file
	for tag, typ, count, value, data in directory:
	    fp.write(data)
	    if len(data) & 1:
		fp.write("\0")

	return offset

class TiffImageFile(ImageFile.ImageFile):

    format = "TIFF"
    format_description = "Adobe TIFF"

    def _open(self):
	"Open the first image in a TIFF file"

	# Header
	ifh = self.fp.read(8)

	if ifh[:4] not in PREFIXES:
	    raise SyntaxError, "not a TIFF file"

	# image file directory (tag dictionary)
	self.tag = self.ifd = ImageFileDirectory(ifh[:2])

	# setup frame pointers
	self.__first = self.__next = self.ifd.i32(ifh, 4)
	self.__frame = -1
	self.__fp = self.fp

	# and load the first frame
	self._seek(0)

    def seek(self, frame):
	"Select a given frame as current image"

	self._seek(frame)
	
    def tell(self):
	"Return the current frame number"

	return self._tell()

    def _seek(self, frame):

	self.fp = self.__fp
	if frame < self.__frame:
	    # rewind file
	    self.__frame = 0
	    self.__next = self.__first
	while self.__frame < frame:
	    self.fp.seek(self.__next)
	    self.tag.load(self.fp)
	    self.__next = self.tag.next
	    self.__frame = self.__frame + 1
	self._setup()

    def _tell(self):

	return self.__frame

    def _decoder(self, rawmode, layer):
	"Setup decoder contexts"

	args = None
	if rawmode == "RGB" and self._planar_configuration == 2:
	    rawmode = rawmode[layer]
	if self._compression == "raw":
	    args = (rawmode, 0, 1)
	if self._compression in ["packbits", "tiff_lzw", "jpeg"]:
	    args = rawmode
	    if self._compression == "jpeg" and self.tag.has_key(JPEGTABLES):
		# Hack to handle abbreviated JPEG headers
		self.tile_prefix = self.tag[JPEGTABLES]
	    elif self._compression == "tiff_lzw" and self.tag.has_key(317):
		# Section 14: Differencing Predictor
		self.decoderconfig = (self.tag[PREDICTOR][0],)

	return args

    def _setup(self):
	"Setup this image object based on current tags"

	# extract relevant tags
	self._compression = COMPRESSION_INFO[self.tag.getscalar(
	    COMPRESSION, 1
	    )]
	self._planar_configuration = self.tag.getscalar(
	    PLANAR_CONFIGURATION, 1
	    )

	photo = self.tag.getscalar(PHOTOMETRIC_INTERPRETATION)

	if Image.DEBUG:
	    print "*** Summary ***"
	    print "- compression:", self._compression
	    print "- photometric_interpretation:", photo
	    print "- planar_configuration:", self._planar_configuration

	# size
	xsize = self.tag.getscalar(IMAGEWIDTH)
	ysize = self.tag.getscalar(IMAGELENGTH)
	self.size = xsize, ysize

	if Image.DEBUG:
	    print "- size:", self.size

	format = self.tag.getscalar(SAMPLEFORMAT, 1)

	# mode: check photometric interpretation and bits per pixel
	key = (
	    photo, format,
	    self.tag.get(BITSPERSAMPLE, (1,)),
	    self.tag.get(EXTRASAMPLES, ())
	    )
	if Image.DEBUG:
	    print "format key:", key
	try:
	    self.mode, rawmode = OPEN_INFO[key]
	except KeyError:
	    if Image.DEBUG:
		print "- unsupported format"
	    raise SyntaxError, "unknown pixel mode"

	if Image.DEBUG:
	    print "- raw mode:", rawmode
	    print "- pil mode:", self.mode

	self.info["compression"] = self._compression

	# build tile descriptors
	x = y = l = 0
	self.tile = []
	if self.tag.has_key(STRIPOFFSETS):
	    # striped image
	    h = self.tag.getscalar(ROWSPERSTRIP, ysize)
	    w = self.size[0]
	    a = None
	    for o in self.tag[STRIPOFFSETS]:
		if not a:
		    a = self._decoder(rawmode, l)
		self.tile.append(
		    (self._compression,
		    (0, min(y, ysize), w, min(y+h, ysize)),
		    o, a))
		y = y + h
		if y >= self.size[1]:
		    x = y = 0
		    l = l + 1
		    a = None
	elif self.tag.has_key(324):
	    # tiled image
	    w = self.tag.getscalar(322)
	    h = self.tag.getscalar(323)
	    a = None
	    for o in self.tag[324]:
		if not a:
		    a = self._decoder(rawmode, l)
		# FIXME: this doesn't work if the image size
		# is not a multiple of the tile size...
		self.tile.append(
		    (self._compression,
		    (x, y, x+w, y+h),
		    o, a))
		x = x + w
		if x >= self.size[0]:
		    x, y = 0, y + h
		    if y >= self.size[1]:
			x = y = 0
			l = l + 1
			a = None
	else:
	    if Image.DEBUG:
		print "- unsupported data organization"
	    raise SyntaxError, "unknown data organization"

	# fixup palette descriptor
	if self.mode == "P":
	    palette = map(lambda a: chr(a / 256), self.tag[COLORMAP])
	    self.palette = ImagePalette.raw("RGB;L", string.join(palette, ""))

#
# --------------------------------------------------------------------
# Write TIFF files

# little endian is default

SAVE_INFO = {
    # mode => rawmode, photometrics, sampleformat, bitspersample, extra
    "1": ("1", 1, 1, (1,), None),
    "L": ("L", 1, 1, (8,), None),
    "P": ("P", 3, 1, (8,), None),
    "I": ("I;32S", 1, 2, (32,), None),
    "I;16": ("I;16", 1, 2, (16,), None),
    "F": ("F;32F", 1, 3, (32,), None),
    "RGB": ("RGB", 2, 1, (8,8,8), None),
    "RGBX": ("RGBX", 2, 1, (8,8,8,8), 0),
    "RGBA": ("RGBA", 2, 1, (8,8,8,8), 2),
    "CMYK": ("CMYK", 5, 1, (8,8,8,8), None),
    "YCbCr": ("YCbCr", 6, 1, (8,8,8), None),
    "LAB": ("LAB", 8, 1, (8,8,8), None),
}

def _save(im, fp, filename):

    try:
	rawmode, photo, format, bits, extra = SAVE_INFO[im.mode]
    except KeyError:
	raise IOError, "cannot write mode %s as TIFF" % im.mode

    ifd = ImageFileDirectory()

    # tiff header (write via IFD to get everything right)
    fp.write(ifd.prefix + ifd.o16(42) + ifd.o32(8))

    ifd[IMAGEWIDTH] = im.size[0]
    ifd[IMAGELENGTH] = im.size[1]

    if im.encoderinfo.has_key("description"):
	ifd[IMAGEDESCRIPTION] = im.encoderinfo["description"]

    if bits != (1,):
	ifd[BITSPERSAMPLE] = bits
	if len(bits) != 1:
	    ifd[SAMPLESPERPIXEL] = len(bits)
    if extra is not None:
	ifd[EXTRASAMPLES] = extra
    if format != 1:
	ifd[SAMPLEFORMAT] = format

    ifd[PHOTOMETRIC_INTERPRETATION] = photo

    if im.mode == "P":
	lut = im.im.getpalette("RGB", "RGB;L")
	ifd[COLORMAP] = tuple(map(lambda v: ord(v) * 256, lut))

    # data orientation
    stride = len(bits) * ((im.size[0]*bits[0]+7)/8)
    ifd[ROWSPERSTRIP] = im.size[1]
    ifd[STRIPBYTECOUNTS] = stride * im.size[1]
    ifd[STRIPOFFSETS] = 0 # this is adjusted by IFD writer

    offset = ifd.save(fp)

    ImageFile._save(im, fp, [
	("raw", (0,0)+im.size, offset, (rawmode, stride, 1))
	])

#
# --------------------------------------------------------------------
# Register

Image.register_open("TIFF", TiffImageFile, _accept)
Image.register_save("TIFF", _save)

Image.register_extension("TIFF", ".tif")
Image.register_extension("TIFF", ".tiff")

Image.register_mime("TIFF", "image/tiff")
