#
# The Python Imaging Library.
# $Id: ImageFile.py,v 1.10 1997/01/14 16:38:36 fl Exp $
#
# base class for image file handlers
#
# History:
#	95-09-09 fl	Created
#	96-03-11 fl	Fixed load mechanism.
#	96-04-15 fl	Added pcx/xbm decoders.
#	96-04-30 fl	Added encoders.
#	96-10-04 fl	Added update_idletasks hook.
#	96-12-14 fl	Added load helpers
#	97-01-11 fl	Added save speedup (via encode_to_file)
#
# Copyright (c) Fredrik Lundh 1995-97.  All rights reserved.
#


import Image
import traceback, sys

MAXBLOCK = 65536

# --------------------------------------------------------------------
# EXPERIMENTAL: Global idletasks callback (to be overloaded)

def update_idletasks(self, bytes):
    # FIXME: argument is not valid for tiled files
    pass

#
# --------------------------------------------------------------------
# Helpers

def _getdecoder(d, e, a, ac):

    # tweak arguments
    if a == None:
	a = ()
    elif type(a) != type(()):
	a = (a,)

    try:
	# get decoder
	decoder = getattr(Image.core, d + "_decoder")
	return apply(decoder, a + ac)
    except AttributeError:
	raise IOError, "decoder %s not available" % d

def _getencoder(d, e, a, ac):
    # tweak arguments
    if a == None:
	a = ()
    elif type(a) != type(()):
	a = (a,)

    try:
	# get encoder
	encoder = getattr(Image.core, d + "_encoder")
	return apply(encoder, a + ac)
    except AttributeError:
	raise IOError, "encoder %s not available" % d

def _tilesort(t1, t2):
    # sort on offset
    return cmp(t1[2], t2[2])

#
# --------------------------------------------------------------------
# ImageFile base class

class ImageFile(Image.Image):
    "Base class for image file format handlers."

    def __init__(self, fp = None, filename = None):

	self.im = None
	self.mode = ""
	self.size = (0, 0)
	self.palette = None
	self.tile = None
	self.info = {}

	self.decoderconfig = ()
	self.decodermaxblock = MAXBLOCK

        if type(fp) == type(""):
	    self.fp = open(fp, "rb")
	    self.filename = fp
	else:
	    self.fp = fp
	    self.filename = filename

	try:
	    self._open()
	except IndexError, v: # end of data
	    if Image.DEBUG > 1:
		traceback.print_exc()
	    raise SyntaxError, v
	except TypeError, v: # end of data (ord)
	    if Image.DEBUG > 1:
		traceback.print_exc()
	    raise SyntaxError, v
	except KeyError, v: # unsupported mode
	    if Image.DEBUG > 1:
		traceback.print_exc()
	    raise SyntaxError, v

	if not self.mode or self.size[0] <= 0:
	    raise SyntaxError, "not identified by this driver"

    def draft(self, mode, size):
	"Set draft mode"

	pass

    def verify(self):
	"Check file integrity"

	# raise exception if something's wrong.  must be called
	# directly after open, and closes file when finished.
	self.fp = None

    def load(self):
	"Load image data based on tile list"

	Image.Image.load(self)

	if self.tile == None:
	    raise IOError, "cannot load this image"
	if not self.tile:
	    return

	# create image (but only if necessary)
	if not self.im or\
	    self.im.mode != self.mode or self.im.size != self.size:
	    self.im = Image.core.new(self.mode, self.size)

	# create palette (optional)
	if self.mode == "P" and self.palette:
	    self.im.putpalette(self.palette.rawmode, self.palette.data)

	# process tiles in file order
	self.tile.sort(_tilesort)

	for d, e, o, a in self.tile:
	    d = _getdecoder(d, e, a, self.decoderconfig)
	    self.load_seek(o)
	    d.setimage(self.im, e)
	    b = ""
	    if hasattr(self, "tile_prefix"):
		# FIXME: This is a hack to handle TIFF's JpegTables tag.
		b = self.tile_prefix
	    t = len(b)
	    while 1:
		s = self.load_read(self.decodermaxblock)
		if not s:
		    self.tile = []
		    raise IOError, "image file is truncated, %d bytes left" % len(b)
	        b = b + s
		n, e = d.decode(b)
		if n < 0:
		    break
		b = b[n:]
		t = t + n
		update_idletasks(self, t)
	self.tile = []
	self.fp = None # might be shared
	if e < 0:
	    raise IOError, "decoder error %d when reading image file" % e

	# post processing
	if hasattr(self, "tile_post_rotate"):
	    # FIXME: This is a hack to handle rotated PCD's
	    self.im = self.im.rotate(self.tile_post_rotate)
	    self.size = self.im.size

	self.load_end()

    #
    # FIXME: don't like these.  make them go away...

    def load_end(self):
	# may be overridden
	pass

    def load_seek(self, pos):
	# may be overridden for contained formats
	self.fp.seek(pos)

    def load_read(self, bytes):
	# may be overridden for blocked formats (e.g. PNG)
	return self.fp.read(bytes)


#
# --------------------------------------------------------------------
# Save image body

def _save(im, fp, tile):
    "Helper to save image based on tile list"

    im.load()
    if not hasattr(im, "encoderconfig"):
	im.encoderconfig = ()
    tile.sort(_tilesort)
    bufsize = max(MAXBLOCK, im.size[0] * 4) # see RawEncode.c
    try:
	fh = fp.fileno()
	fp.flush()
    except AttributeError:
	# compress to Python file-compatible object
	for e, b, o, a in tile:
	    e = _getencoder(e, b, a, im.encoderconfig)
	    if o > 0:
		fp.seek(o, 0)
	    e.setimage(im.im, b)
	    while 1:
		l, s, d = e.encode(bufsize)
		fp.write(d)
		if s:
		    break
	    if s < 0:
		raise IOError, "encoder error %d when writing image file" % s
    else:
	# slight speedup: compress to real file object
	for e, b, o, a in tile:
	    e = _getencoder(e, b, a, im.encoderconfig)
	    if o > 0:
		fp.seek(o, 0)
	    e.setimage(im.im, b)
	    s = e.encode_to_file(fh, bufsize)
	    if s < 0:
		raise IOError, "encoder error %d when writing image file" % s
