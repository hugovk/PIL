# $Id: ImageFont.py,v 1.1 1996/10/04 19:40:50 fredrik Exp $
#
# THIS IS WORK IN PROGRESS.
#
# The Python Imaging Library.
#
# File:
#	ImageFont.py -- PIL raster font management
#
# History:
#	96-08-07 fl	Created (experimental)
#
# Copyright (c) Fredrik Lundh 1996.  All rights reserved.
#

import Image
import math, os, string, sys

# --------------------------------------------------------------------
# Font metrics format:
#	"PILfont" LF
#	fontdescriptor LF
#	(optional) key=value... LF
#	"DATA" LF
#	binary data: 256*10*2 bytes (dx, dy, dstbox, srcbox)
#
# To place a character, cut out srcbox and paste at dstbox,
# relative to the character position.  Then move the character
# position according to dx, dy.
# --------------------------------------------------------------------

def i16(c):
    v = (ord(c[0])<<8) + ord(c[1])
    if v >= 32768:
	v = v - 65536
    return v

class ImageFont:

    def _load_pilfont(self, filename):

	fp = open(filename, "rb")

	# read PILfont header
	if fp.readline() != "PILfont\n":
	    raise SyntaxError, "Not a PILfont file"
	d = string.split(fp.readline(), ";")
	self.ysize = string.atoi(d[6])
	self.props = [] # FIXME: should be a dictionary
	s = fp.readline()
	while s and s != "DATA\n":
	    self.props.append(s)

	# read PILfont metrics
	self.metrics = []
	for i in range(256):
	    s = fp.read(20)
	    if len(s) != 20:
		raise SyntaxError, "Broken PILfont file"
	    m = map(lambda a,s=s: i16(s[a:a+2]), range(0,20,2))
	    self.metrics.append(m)

	try:
	    self.image = Image.open(os.path.splitext(filename)[0] + ".gif")
	except:
	    self.image = Image.open(os.path.splitext(filename)[0] + ".pbm")

	self.image.load()

    def getsize(self, str):
	w = 0
	for id in map(ord, str):
	    w = w + self.metrics[id][0]
	if w == 0:
	    return 0, 0
	return w, self.ysize

    def getmask(self, str):
	# FIXME: this should be reimplemented in C
	im = Image.new("1", self.getsize(str), 0)
	x = 0
	for m in map(lambda c, m=self.metrics: m[ord(c)], str):
	    [x0, y0, x1, y1] = m[2:6]
	    im.im.paste(self.image.im.crop(tuple(m[6:10])),
			(x0+x, y0, x1+x, y1))
	    x = x + m[0]
	return im

#
# --------------------------------------------------------------------

def load(filename):
    "Load a font file."
    f = ImageFont()
    f._load_pilfont(filename)
    return f

def load_path(filename):
    "Load a font file, searching along the Python path."
    for dir in sys.path:
	try:
	    return load(os.path.join(dir, filename))
	except:
	    pass
    raise IOError, "cannot find font file"

