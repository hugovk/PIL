#
# The Python Imaging Library.
# $Id: PngImagePlugin.py,v 1.4 1996/12/16 18:50:23 fl Exp fl $
#
# PNG support code
#
# See "PNG (Portable Network Graphics) Specification, version 1.0;
# W3C Recommendation", 1996-10-01, Thomas Boutell (ed.).
#
# history:
# 0.1	96-05-06 fl	Created (couldn't resist it)
# 0.2	96-12-14 fl	Upgraded, added read and verify support
#	96-12-15 fl	Separate PNG stream parser
#
# Copyright (c) Fredrik Lundh 1996.  All rights reserved.
#

__version__ = "0.2"

import string

import Image, ImageFile


def i16(c):
    return ord(c[1]) + (ord(c[0])<<8)
def i32(c):
    return ord(c[3]) + (ord(c[2])<<8) + (ord(c[1])<<16) + (ord(c[0])<<24)


MODES = {
    # supported bits/color combinations, and corresponding modes/rawmodes
    (1, 0): ("1", "1"),
    (2, 0): ("L", "L;2"),
    (4, 0): ("L", "L;4"),
    (8, 0): ("L", "L"),
    (16,0): ("L", "L;16B"),
    (8, 2): ("RGB", "RGB"),
    (16,2): ("RGB", "RGB;16B"),
    (1, 3): ("P", "P;1"),
    (2, 3): ("P", "P;2"),
    (4, 3): ("P", "P;4"),
    (8, 3): ("P", "P"),
    (8, 4): ("RGBA", "LA"),
    (16,4): ("RGBA", "LA;16B"),
    (8, 6): ("RGBA", "RGBA"),
    (16,6): ("RGBA", "RGBA;16B"),
}


# --------------------------------------------------------------------
# Support classes.  Suitable for PNG and related formats like MNG etc.

class ChunkStream:

    def __init__(self, fp):

	self.fp = fp
	self.queue = []

	if not hasattr(Image.core, "crc32"):
	    self.crc = self.crc_skip

    def read(self):
	"Fetch a new chunk. Returns header information."

	if self.queue:

	    cid, pos, len = self.queue[-1]
	    del self.queue[-1]

	    self.fp.seek(pos)

	else:

	    s = self.fp.read(8)

	    cid = s[4:]
	    pos = self.fp.tell()
	    len = i32(s)

	return cid, pos, len

    def close(self):
	del self.queue
	self.fp = None

    def push(self, cid, pos, len):

	self.queue.append((cid, pos, len))

    def call(self, cid, pos, len):
	"Call the appropriate chunk handler"

	if Image.DEBUG:
	    print cid, pos, len
	return getattr(self, "chunk_" + cid)(pos, len)

    def crc(self, cid, data):
	"Read and verify checksum"

	crc1 = Image.core.crc32(data, Image.core.crc32(cid))
	crc2 = i16(self.fp.read(2)), i16(self.fp.read(2))
	if crc1 != crc2:
	    raise SyntaxError, "broken PNG file (bad header checksum)"

    def crc_skip(self, cid, data):
	"Read checksum.  Used of the C module is not present"

	self.fp.read(4)

    def verify(self, endchunk = "IEND"):

	# Simple approach; just calculate checksum for all remaining
	# blocks.  Must be called directly after open.

	cids = []

	while 1:
	    cid, pos, len = self.read()
	    if cid == endchunk:
		break
	    self._crc(cid, self.fp.read(len))
	    cids.append(cid)

	return cids


# --------------------------------------------------------------------
# PNG image stream (IHDR/IEND)

class PngStream(ChunkStream):

    def __init__(self, im, fp):

	ChunkStream.__init__(self, fp)

	self.im = im

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

	# image data
	self.im.tile = [("zip", (0,0)+self.im.size, pos, self.im.rawmode)]
	self.im.idat = len
	raise EOFError

    def chunk_IEND(self, pos, len):

	# end of PNG image
	raise EOFError

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
	[k, v] = string.split(s, "\0")
	self.im.info[k] = v
	return s


# --------------------------------------------------------------------
# PNG reader

class PngImageFile(ImageFile.ImageFile):

    format = "PNG"
    format_description = "Portable network graphics"

    def _open(self):

	if self.fp.read(8) != "\211PNG\r\n\032\n":
	    raise SyntaxError, "not a PNG file"

	self.png = PngStream(self, self.fp)

	while 1:

	    #
	    # get next chunk

	    cid, pos, len = self.png.read()

	    try:
		s = self.png.call(cid, pos, len)
	    except EOFError:
		break

	    except AttributeError:
		if Image.DEBUG:
		    print cid, pos, len, "(unknown)"
		s = self.fp.read(len)

	    self.png.crc(cid, s)

    def verify(self):
	"Verify PNG file"

	# back up to beginning of IDAT block
	self.fp.seek(self.tile[0][2] - 8)

	self.png.verify(self)

	self.png.close()
	self.fp = None

    def load_read(self, bytes):
	"Read more data from chunks (used by ImageFile.load)"

	while self.idat == 0:
	    # end of chunk, skip forward to next one

	    self.fp.read(4) # CRC

	    cid, pos, len = self.png.read()

	    if cid not in ["IDAT", "DDAT"]:
		self.png.push(cid, pos, len)
		return ""

	    self.idat = len # empty chunks are allowed

	# read more data from this chunk
	if bytes <= 0:
	    bytes = self.idat
	else:
	    bytes = min(bytes, self.idat)

	self.idat = self.idat - bytes

	return self.fp.read(bytes)

#
# --------------------------------------------------------------------

Image.register_open("PNG", PngImageFile)

Image.register_extension("PNG", ".png")

Image.register_mime("PNG", "image/png")
