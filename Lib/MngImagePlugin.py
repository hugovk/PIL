#
# THIS IS WORK IN PROGRESS
#
# The Python Imaging Library.
# $Id: MngImagePlugin.py,v 1.1 1996/12/16 18:50:23 fl Exp fl $
#
# MNG support code
#
# See "MNG (Multiple-Image Network Graphics) Format Version
# 19961212", 1996-12-12, Glenn Randers-Pehrsson.
# <ftp://swrinde.nde.swri.edu/pub/mng/documents>
#
# history:
#	96-12-13 fl	Created (couldn't resist it)
#
# Copyright (c) Fredrik Lundh 1996.  All rights reserved.
#

__version__ = "0.1"

import string

import Image, ImageFile

from PngImagePlugin import i16, i32, ChunkStream, PngStream, MODES


class MngStream(ChunkStream):
    "Parser callbacks for MNG data"

    def __init__(self, im, fp):

	ChunkStream.__init__(self, fp)

	self.im = im
	self.atend = [None]

    def chunk_MHDR(self, pos, len):

	# image header
	s = self.fp.read(len)
	self.im.size = self.size = i32(s), i32(s[4:])
	try:
	    self.im.mode = "RGB"
	except:
	    pass
	self.im.info["frames"] = i32(s[16:])
	return s

    def chunk_MEND(self, pos, len):
	raise EOFError, "end of MNG file"

    def chunk_DEFI(self, pos, len):
	
	# define image
	buf = self.fp.read(len)

	s = buf + "\0"*9

	# store image_id, state, offset
	self.atend = "DEFI", i16(s), ord(s[2]), (i32(s[3:]), i32(s[7:]))

	return buf

    def chunk_IHDR(self, pos, len):
	
	# image header
	s = self.fp.read(len)
	self.im.size = i32(s), i32(s[4:])
	try:
	    self.im.mode, self.im.rawmode = MODES[(ord(s[8]), ord(s[9]))]
	except:
	    pass
	if ord(s[12]):
	    self.im.info["interlace"] = 1
	if ord(s[11]):
	    raise SyntaxError, "unknown filter category"

	return s

    def chunk_IDAT(self, pos, len):

	self.im.tile = [("zip", (0,0)+self.im.size, pos, self.im.rawmode)]
	self.im.idat = len
	raise EOFError

	# return self.fp.read(len)

    def chunk_IEND(self, pos, len):

	cid = self.atend[0]
	if cid == "DEFI":
	    cid, id, state, offset = self.atend

	return self.fp.read(len)

    def chunk_DHDR(self, pos, len):

	# delta image
	buf = self.fp.read(len)

	s = buf + "\0"*16

	delta_type = ord(s[3])

	# determine bounding box
	if delta_type != 5:
	    xsize, ysize = i32(s[4:]), i32(s[8:])
	    if delta_type != 0:
		xoffs, yoffs = i32(s[12:]), i32(s[16:])
	    else:
		xoffs, yoffs = 0
	    bbox = xoffs, yoffs, xoffs+xsize, yoffs+ysize
	else:
	    bbox = 0, 0, 0, 0

	# chunk, image_id, image_type, delta_type, bbox
	self.atend = "DHDR", i16(s), ord(s[2]), delta_type, bbox
	if Image.DEBUG:
	    print self.atend

	xy = 0, 0, bbox[2]-bbox[0], bbox[3]-bbox[1]

	# store current frame
	self.im.old_im = self.im.im
	self.im.old_size = self.im.size

	# set things up to read delta frame
	self.im.size = xy[2:]
	self.im.tile = [("zip", xy, pos, self.im.rawmode)]
	self.im.idat = len

	return buf

    def chunk_PLTE(self, pos, len):
	
	# palette
	s = self.fp.read(len)
	if self.im.mode == "P":
	    self.im.palette = "RGB", s
	return s

    def chunk_gAMA(self, pos, len):

	# gamma setting
	s = self.fp.read(len)
	self.im.info["gamma"] = i32(s) / 100000.0
	return s

    def chunk_tEXt(self, pos, len):

	# text
	s = self.fp.read(len)
	try:
	    [k, v] = string.split(s, "\0")
	    self.im.info[k] = v
	except ValueError:
	    pass
	return s

    def chunk_NEED(self, pos, len):

	# capabilities
	# FIXME: what should we support?
	s = self.fp.read(len)
	return s


class MngImageFile(ImageFile.ImageFile):

    format = "MNG"
    format_description = "Multiple-image network graphics"


    def _open(self):

	if self.fp.read(8) != "\212MNG\r\n\032\n":
	    raise SyntaxError, "not a MNG file"

	self.mng = MngStream(self, self.fp)

	self.seek(0)

    def seek(self, frame):

	self.fp = self.mng.fp

	while 1:

	    #
	    # get next chunk

	    cid, pos, len = self.mng.read()

	    try:
		s = self.mng.call(cid, pos, len)
	    except EOFError:
		break

	    except AttributeError:
		if Image.DEBUG:
		    print cid, len, "(unknown)"
		s = self.fp.read(len)

	    self.mng.fp.read(4) # skip extra CRC
	    # self.mng.crc(cid, s)

	if not self.tile:
	    raise EOFError, "no more frames"

	try:
	    self.load()
	except IOError:
	    print "PROBLEM!"
	    self.load_end()

	self.mng.fp.read(4) # skip extra CRC

    def tell(self):
	return 0

    def verify(self):
	"Verify MNG file"

	# back up to beginning of IDAT block
	self.fp.seek(self.tile[0][2] - 8)

	self.mng.verify(self)

	self.mng.close()
	self.fp = None

    def load_read(self, bytes):
	"Read more data from chunks (used by ImageFile.load)"

	while self.idat == 0:
	    # end of chunk, skip forward to next one

	    self.fp.read(4) # CRC

	    cid, pos, len = self.mng.read()

	    if cid not in ["IDAT", "DDAT"]:

		self.mng.push(cid, pos, len)

		return ""

	    self.idat = len # empty chunks are allowed

	# read more data from this chunk
	if bytes <= 0:
	    bytes = self.idat
	else:
	    bytes = min(bytes, self.idat)

	self.idat = self.idat - bytes

	return self.fp.read(bytes)

    def load_end(self):

	if self.mng.atend[0] == "DHDR":
	    cid, image_id, image_type, delta_type, bbox = self.mng.atend
	    im1 = self.im
	    self.size = self.old_size
	    self.im = self.old_im
	    im2 = self.crop(bbox).im
	    im1 = im1.chop_add_modulo(im2)
	    self.im.paste(im1, bbox)

	if 0 and self.size != self.mng.size:
	    print "EXPAND", self.size, "to", self.mng.size
	    im = self.im
	    self.im = Image.new(self.mode, self.mng.size, 0).im
	    self.im.putpalette(self.palette[0], self.palette[1])
	    self.im.paste(im, (0,0)+self.size)
	    self.size = self.mng.size


#
# --------------------------------------------------------------------

Image.register_open("MNG", MngImageFile)

Image.register_extension("MNG", ".mng")

Image.register_mime("MNG", "image/x-mng")
