#
# The Python Imaging Library.
# $Id: Image.py,v 1.16 1997/01/14 18:01:12 fl Exp $
#
# the Image class wrapper
#
# history:
#	95-09-09 fl	Created
#	96-03-11 fl	PIL release 0.0
#	96-04-30 fl	PIL release 0.1b1
#	96-05-27 fl	PIL release 0.1b2
#	96-10-04 fl	PIL release 0.2a1
#	96-11-04 fl	PIL release 0.2b1
#	96-12-08 fl	PIL release 0.2b2
#	96-12-16 fl	PIL release 0.2b3
#	97-01-14 fl	PIL release 0.2b4
#
# Copyright (c) Fredrik Lundh 1995-97.  All rights reserved.
#

class _imaging_not_installed:
    def __getattr__(self, id):
	raise ImportError, "The _imaging C module is not installed"

try:
    # If the _imaging C module is not present, you can only use the
    # "open" function to identify files.  Most other operations will
    # fail.  Note that other modules should not refer to _imaging
    # directly; import Image and use the core variable instead.
    import _imaging
    core = _imaging
    del _imaging
except ImportError:
    core = _imaging_not_installed()

import os, string

#
# Debug level

DEBUG = 0

#
# Constants (defined in _imagingmodule.c)

# transpose
FLIP_LEFT_RIGHT = 0
FLIP_TOP_BOTTOM = 1
ROTATE_90 = 2
ROTATE_180 = 3
ROTATE_270 = 4

# transform
AFFINE = 0
EXTENT = 1
PERSPECTIVE = 2 # Not yet implemented

#
# Registries

ID = []
OPEN = {}
MIME = {}
SAVE = {}
EXTENSION = {}

#
# Helpers

_initialized = 0

def init():
    "Load file format drivers.  Called automatically by open and save."

    global _initialized
    if _initialized:
	return

    import os, sys
    for p in filter(os.path.isabs, filter(os.path.isdir, sys.path)):
	for f in os.listdir(p):
	    if f[-14:] == "ImagePlugin.py":
		p, f = os.path.split(f)
		f, e = os.path.splitext(f)
		try:
		    sys.path.insert(0, p)
		    try:
			__import__(f)
		    finally:
			del sys.path[0]
		except ImportError:
		    if DEBUG:
			print "Image: failed to import",
			print f, ":", sys.exc_value

    if OPEN or SAVE:
	_initialized = 1

# --------------------------------------------------------------------
# Implementation wrapper

import os

class Image:

    format = None
    format_description = None

    def __init__(self):
	self.im = None
	self.mode = ""
	self.size = (0,0)
	self.palette = None
	self.info = {}

    def __setattr__(self, id, value):
	if id == "palette":
	    pass # print "set", id, value
	self.__dict__[id] = value

    def _makeself(self, im):
	new = Image()
	new.im = im
	new.mode = im.mode
	new.size = im.size
	new.palette = self.palette
	new.info = self.info
	return new

    def _dump(self, file = None):
        import tempfile
	if not file:
	    file = tempfile.mktemp()
	self.load()
	self.im.save_ppm(file)
	return file

    def tostring(self, fmt = None):
	"Return image as a binary string"

	self.load()
	return self.im.tostring()

    def tobitmap(self, name = "image"):
	"Return image as an XBM bitmap"

	self.load()
	if self.mode != "1":
	    raise ValueError, "not a bitmap"
	e = core.xbm_encoder()
	e.setimage(self.im, (0,0)+self.size)
	l, s, d = e.encode(self.size[0]*self.size[1])
	if s <= 0:
	    raise ValueError, "failed to create bitmap"
	return string.join(["#define %s_width %d\n" % (name, self.size[0]),
		"#define %s_height %d\n"% (name, self.size[1]),
		"static char %s_bits[] = {\n" % name, d[:l], "};"], "")

    def fromstring(self, buf, fmt = None):
	self.im.fromstring(buf)

    def load(self):
	if self.im and self.palette:
	    self.im.putpalette(self.palette.rawmode, self.palette.data)

    #
    # function wrappers

    def convert(self, mode = None, matrix = None):
        "Convert to other pixel format"

	if not mode:
	    if self.mode == "P":
		mode = self.palette.mode
	    else:
		return self.copy()
	self.load()
	if matrix:
	    im = self.im.convert_matrix(mode, matrix)
	else:
	    im = self.im.convert(mode)
	return self._makeself(im)

    def copy(self):
        "Copy raster data"

	self.load()
	im = self.im.copy()
	return self._makeself(im)

    def crop(self, box = None):
        "Crop region from image"

	self.load()
	if box == None:
	    return self.copy()

	# the C implementation of crop is broken, so we implement
	# it by pasting into empty image instead.

	size = box[2]-box[0], box[3]-box[1]

	im = core.new(self.mode, size)
	im.paste(self.im, (-box[0], -box[1],
		 self.size[0]-box[0],
		 self.size[1]-box[1]))

	# im = self.im.crop(box)

	return self._makeself(im)

    def draft(self, mode, size):
        "Configure image decoder"

	pass

    def filter(self, kernel):
        "Apply environment filter to image"

	self.load()
	id = kernel.id
	if len(self.mode) == 1:
	    return self._makeself(self.im.filter(id))
	# fix to handle multiband images since _imaging doesn't
	ims = []
	for c in range(self.im.bands):
	    ims.append(self._makeself(self.im.getband(c).filter(id)))
	return merge(self.mode, ims)

    def getbbox(self):
	"Get bounding box of actual data (non-zero pixels) in image"

    	self.load()
	return self.im.getbbox()

    def getdata(self, band = None):
	"Get image data as sequence object."

    	self.load()
	if band != None:
	    return self.im.getband(band)
	return self.im # could be misused

    def getpixel(self, (x, y)):
	"Get pixel value"

    	self.load()
	return self.im[int(x + y * self.size[0])]

    def getprojection(self):
	"Get projection to x and y axes"

    	self.load()
	x, y = self.im.getprojection()
	return map(ord, x), map(ord, y)

    def histogram(self):
        "Take histogram of image"

	self.load()
	return self.im.histogram()

    def offset(self, xoffset, yoffset = None):
        "Offset image in horizontal and/or vertical direction"

	if yoffset == None:
	    yoffset = xoffset
	self.load()
	return self._makeself(self.im.offset(xoffset, yoffset))

    def paste(self, im, box = None, mask = None):
        "Paste other image into region"

	im.load()
	self.load()
	if box == None:
	    box = (0, 0) + self.size
	# fix to handle conversion when pasting
	if self.mode != im.mode:
	    im = im.convert(self.mode)
	if mask:
	    mask.load()
	    self.im.paste(im.im, box, mask.im)
	else:
	    self.im.paste(im.im, box)

    def point(self, lut, mode = None):
        "Map image through lookup table"

	if type(lut) != type([]):
	    # if it isn't a list, it should be a function
	    lut = map(lut, range(256)) * len(self.mode)

	self.load()
	im = self.im.point(lut, mode)
	return self._makeself(im)

    def putalpha(self, im):
        "Set alpha layer"

	if self.mode != "RGBA" or im.mode not in ["1", "L"]:
	    raise ValueError, "illegal image mode"

	im.load()
	self.load()

	if im.mode == "1":
	    im = im.convert("L")

	self.im.putband(im.im, 3)

    def putdata(self, data, scale = 1.0, offset = 0.0):
	"Put data from a sequence object into an image."

	self.load() # hmm...
	self.im.putdata(data, scale, offset)

    def resize(self, size):
        "Resize image"

	self.load()
	im = self.im.resize(size)
	return self._makeself(im)

    def rotate(self, angle):
        "Rotate image.  Angle given as degrees counter-clockwise."

	self.load()
	im = self.im.rotate(angle)
	return self._makeself(im)

    def save(self, fp, format = None, **params):
        "Save image to file or stream"

	if type(fp) == type(""):
	    import __builtin__
	    filename = fp
            fp = __builtin__.open(fp, "wb")
	else:
	    filename = ""

	init()

        if not format:
	    try:
	        format = EXTENSION[string.lower(os.path.splitext(filename)[1])]
	    except KeyError, v:
	        raise IOError, "cannot determine format for %s files" % v

	self.encoderinfo = params
	self.encoderconfig = ()

	self.load()

	try:
	    SAVE[string.upper(format)](self, fp, filename)
	except KeyError, v:
	    raise IOError, "cannot save %s files" % v

    def show(self, title = None):
        "Display image (for debug purposes only)"

	try:
	    import ImageTk
	    ImageTk._show(self, title)
	    # note: caller must enter mainloop
	except:
	    _showxv(self, title)

    def split(self):
        "Split image into bands"

        ims = []
	self.load()
	for i in range(self.im.bands):
	    ims.append(self._makeself(self.im.getband(i)))
	return tuple(ims)

    def thumbnail(self, size):
	"Create thumbnail representation (modifies image in place)"

	# preserve aspect ratio
	x, y = self.size
	if x > size[0]: y = y * size[0] / x; x = size[0]
	if y > size[1]: x = x * size[1] / y; y = size[1]
	size = x, y

	if size == self.size:
	    return

	self.draft(None, size)

	im = self.resize(size)

	self.im = im.im
	self.mode = im.mode
	self.size = size

    def transform(self, size, method, data):
        "Transform image"

	if method == EXTENT:
	    x0, y0, x1, y1 = data
	    xs = float(x1 - x0) / size[0]
	    ys = float(y1 - y0) / size[1]
	    data = (xs, 0, x0 + xs/2, 0, ys, y0 + ys/2)
	elif method != AFFINE:
	    raise ValueError, "unknown transformation method"

	self.load()
	im = self.im.transform(size, data)
	return self._makeself(im)

    def transpose(self, method):
        "Transpose image (flip or rotate in 90 degree steps)"

	self.load()
	im = self.im.transpose(method)
	return self._makeself(im)


# --------------------------------------------------------------------
# Factories

#
# Debugging

def _wedge():
    "Create greyscale wedge (for debugging only)"

    return Image()._makeself(core.wedge("L"))

#
# Create/open images.

def new(mode, size, color = None):
    "Create a new image"

    if color == None:
	return Image()._makeself(core.new(mode, size))

    if type(color) == type(()):
	# convert colour tuple to integer value
	if len(color) == 3:
	    color = color[0] + (color[1]<<8) + (color[2]<<16)
	elif len(color) == 4:
	    color = color[0] + (color[1]<<8) + (color[2]<<16) + (color[3]<<24)
	else:
	    color = color[0]

    return Image()._makeself(core.fill(mode, size, color))

def open(fp, mode = "r"):
    "Open an image file, without loading the raster data"

    if mode != "r":
        raise ValueError, "bad mode"

    if type(fp) == type(""):
        import __builtin__
	filename = fp
        fp = __builtin__.open(fp, "rb")
    else:
        filename = ""

    init()

    prefix = fp.read(16)

    for i in ID:
	try:
	    factory, accept = OPEN[i]
	    if not accept or accept(prefix):
		fp.seek(0)
		return factory(fp, filename)
	except SyntaxError:
	    pass

    raise IOError, "cannot identify image file"

#
# Image processing.

def blend(im1, im2, alpha):
    "Interpolate between images."

    if alpha == 0.0:
	return im1
    elif alpha == 1.0:
	return im2
    return Image()._makeself(core.blend(im1.im, im2.im, alpha))

def composite(image1, image2, mask):
    "Create composite image by blending images using a transparency mask"

    image = image2.copy()
    image.paste(image1, None, mask)
    return image

def eval(image, *args):
    "Evaluate image expression"

    return image.point(args[0])

def merge(mode, bands):
    "Merge a set of single band images into a new multiband image."

    if len(mode) != len(bands) or "*" in mode:
        raise ValueError, "wrong number of bands"
    for im in bands[1:]:
        if len(im.mode) != 1 or im.size != bands[0].size:
            raise ValueError, "wrong number of bands"
    im = core.new(mode, bands[0].size)
    for i in range(len(mode)):
	bands[i].load()
	im.putband(bands[i].im, i)
    return Image()._makeself(im)


# --------------------------------------------------------------------
# Plugin registry

def register_open(id, factory, accept = None):
    id = string.upper(id)
    ID.append(id)
    OPEN[id] = factory, accept

def register_mime(id, mimetype):
    MIME[string.upper(id)] = mimetype

def register_save(id, driver):
    SAVE[string.upper(id)] = driver

def register_extension(id, extension):
    EXTENSION[string.lower(extension)] = string.upper(id)


# --------------------------------------------------------------------
# Unix display support

def _showxv(self, title = None):

    if self.mode == "P":
	file = self.convert("RGB")._dump()
    else:
	file = self._dump()

    if title:
	opt = "-name \"%s\"" % title
    else:
	opt = ""

    os.system("(xv %s %s; rm -f %s)&" % (opt, file, file))
