#
# The Python Imaging Library.
# $Id$
#
# EPS file handling
#
# History:
#	95-09-01 fl	Created
#	96-05-18 fl	Don't choke on "atend" fields, Ghostscript interface
#	96-08-22 fl	Don't choke on floating point BoundingBox values
#	96-08-23 fl	Handle files from Macintosh
#
# Copyright (c) Secret Labs AB 1997.
# Copyright (c) Fredrik Lundh 1995-96.
#
# See the README file for information on usage and redistribution.
#


__version__ = "0.3"


import regex, string
import Image, ImageFile

#
# --------------------------------------------------------------------

def i32(c):
    return ord(c[0]) + (ord(c[1])<<8) + (ord(c[2])<<16) + (ord(c[3])<<24)

def o32(i):
    return chr(i&255) + chr(i>>8&255) + chr(i>>16&255) + chr(i>>24&255)

split = regex.compile("^%%\([^:]*\):[ \t]*\(.*\)[ \t]*$")
field = regex.compile("^%[%!]\([^:]*\)[ \t]*$")

def Ghostscript(tile, size, fp):
    """Render an image using Ghostscript (Unix only)"""

    # Unpack decoder tile
    decoder, tile, offset, data = tile[0]
    length, bbox = data

    import tempfile, os

    file = tempfile.mktemp()

    # Build ghostscript command
    command = ["gs",
	       "-q",			# quite mode
	       "-g%dx%d" % size,	# set output geometry (pixels)
	       "-dNOPAUSE -dSAFER",	# don't pause between pages, safe mode
	       "-sDEVICE=ppmraw",	# ppm driver
	       "-sOutputFile=%s" % file,# output file
	       "- >/dev/tty 2>/dev/tty"]

    command = string.join(command)

    # push data through ghostscript
    try:
	gs = os.popen(command, "w")
	# adjust for image origin
	if bbox[0] != 0 or bbox[1] != 0:
	    gs.write("%d %d translate\n" % (-bbox[0], -bbox[1]))
	fp.seek(offset)
	while length > 0:
	    s = fp.read(8192)
	    if not s:
		break
	    length = length - len(s)
	    gs.write(s)
	gs.close()
	im = Image.core.open_ppm(file)
    finally:
	try: os.unlink(file)
	except: pass

    return im


class PSFile:
    """Wrapper that treats either CR or LF as end of line."""
    def __init__(self, fp):
	self.fp = fp
    def __getattr__(self, id):
	v = getattr(self.fp, id)
	setattr(self, id, v)
	return v
    def readline(self):
	s = ""
	c = self.fp.read(1)
	while c not in "\r\n":
	    s = s + c
	    c = self.fp.read(1)
	return s + "\n"


def _accept(prefix):
    return prefix[:4] == "%!PS" or i32(prefix) == 0xC6D3D0C5


class EpsImageFile(ImageFile.ImageFile):
    """EPS File Parser for the Python Imaging Library"""

    format = "EPS"
    format_description = "Encapsulated Postscript"

    def _open(self):

	# FIXME: should check the first 512 bytes to see if this
	# really is necessary (platform-dependent, though...)

	fp = PSFile(self.fp)

	# HEAD
	s = fp.read(512)
	if s[:4] == "%!PS":
	    offset = 0
	    fp.seek(0, 2)
	    length = fp.tell()
	elif i32(s) == 0xC6D3D0C5: 
	    offset = i32(s[4:])
	    length = i32(s[8:])
	else:
	    raise SyntaxError, "not an EPS file"

	fp.seek(offset)

	box = None

	self.mode = "RGB"
	self.size = 1, 1 # FIXME: huh?


	#
	# Load EPS header

	s = fp.readline()

	while s:

	    if len(s) > 255:
		raise SyntaxError, "not an EPS file"

	    if s[-2:] == '\r\n':
		s = s[:-2]
	    elif s[-1:] == '\n':
		s = s[:-1]

	    try:
		hit = split.match(s)
	    except regex.error, v:
		raise SyntaxError, "not an EPS file"

	    if hit >= 0:
		k, v = split.group(1, 2)
		self.info[k] = v
		if k == "BoundingBox":
		    try:
			# Note: The DSC spec says that BoundingBox
			# fields should be integers, but some drivers
			# put floating point values there anyway.
			box = map(int, map(string.atof, string.split(v)))
			self.size = box[2] - box[0], box[3] - box[1]
			self.tile = [("eps", (0,0) + self.size, offset,
				      (length, box))]
		    except:
			pass

	    elif field.match(s) >= 0:
		k = field.group(1)
		if k == "EndComments":
		    break
		if k[:8] == "PS-Adobe":
		    self.info[k[:8]] = k[9:]
		else:
		    self.info[k] = ""

	    else:
		raise IOError, "bad EPS header"

	    s = fp.readline()
	    if s[:2] != "%%":
		break

	#
	# Scan for an "ImageData" descriptor

	while s[0] == "%":

	    if len(s) > 255:
		raise SyntaxError, "not an EPS file"

	    if s[-2:] == '\r\n':
		s = s[:-2]
	    elif s[-1:] == '\n':
		s = s[:-1]

	    if s[:11] == "%ImageData:":

		s = map(string.atoi, string.split(s[11:]))
		[x, y, bi, mo, z3, z4, en, id] = s

		if en == 1:
		    decoder = "eps_binary"
		elif en == 2:
		    decoder = "eps_hex"
		else:
		    break
		if bi != 8:
		    break
		if mo == 1:
		    self.mode = "L"
		elif mo == 2:
		    self.mode = "LAB"
		elif mo == 3:
		    self.mode = "RGB"
		else:
		    break

		# Scan forward to the actual image data
		while 1:
		    s = fp.readline()
		    if not s:
			break
		    if s[:len(id)] == id:
			self.size = x, y
			self.tile2 = [(decoder,
				       (0, 0, x, y),
				       fp.tell(),
				       0)]
			return

	    s = fp.readline()
	    if not s:
		break

	if not box:
	    raise IOError, "cannot determine EPS bounding box"

    def load(self):
	# Load EPS via Ghostscript
	if not self.tile:
	    return
	self.im = Ghostscript(self.tile, self.size, self.fp)
	self.mode = self.im.mode
	self.size = self.im.size
	self.tile = []

#
# --------------------------------------------------------------------

def _save(im, fp, filename, eps=1):
    """EPS Writer for the Python Imaging Library.""" 

    #
    # make sure image data is available
    im.load()

    #
    # determine postscript image mode
    if im.mode == "L":
	operator = (8, 1, "image")
    elif im.mode == "RGB":
	operator = (8, 3, "false 3 colorimage")
    elif im.mode == "CMYK":
	operator = (8, 4, "false 4 colorimage")
    else:
	raise ValueError, "image mode is not supported"

    if eps:
	#
	# write EPS header
	fp.write("%!PS-Adobe-3.0 EPSF-3.0\n")
	fp.write("%%Creator: PIL 0.1 EpsEncode\n")
    	#fp.write("%%CreationDate: %s"...)
	fp.write("%%%%BoundingBox: 0 0 %d %d\n" % im.size)
	fp.write("%%Pages: 1\n")
	fp.write("%%EndComments\n")
	fp.write("%%Page: 1 1\n")
	fp.write("%%ImageData: %d %d " % im.size)
	fp.write("%d %d 0 1 1 \"%s\"\n" % operator)

    #
    # image header
    fp.write("gsave\n")
    fp.write("10 dict begin\n")
    fp.write("/buf %d string def\n" % (im.size[0] * operator[1]))
    fp.write("%d %d scale\n" % im.size)
    fp.write("%d %d 8\n" % im.size) # <= bits
    fp.write("[%d 0 0 -%d 0 %d]\n" % (im.size[0], im.size[1], im.size[1]))
    fp.write("{ currentfile buf readhexstring pop } bind\n")
    fp.write("%s\n" % operator[2])

    ImageFile._save(im, fp, [("eps", (0,0)+im.size, 0, None)])

    fp.write("\n%%%%EndBinary\n")
    fp.write("grestore end\n")
    fp.flush()

#
# --------------------------------------------------------------------

Image.register_open(EpsImageFile.format, EpsImageFile, _accept)

Image.register_save(EpsImageFile.format, _save)

Image.register_extension(EpsImageFile.format, ".ps")
Image.register_extension(EpsImageFile.format, ".eps")

Image.register_mime(EpsImageFile.format, "application/postscript")
