#
# The Python Imaging Library
# $Id: ImageDraw.py,v 1.4 1996/11/10 17:52:14 fredrik Exp $
#
# drawing interface operations
#
# History:
#	96-04-13 fl	Created (experimental)
#	96-08-07 fl	Filled polygons, ellipses.
#	96-08-13 fl	Added text support
#
# Copyright (c) Secret Labs AB 1997.
# Copyright (c) Fredrik Lundh 1996.
#
# See the README file for information on usage and redistribution.
#

import Image
import math

class ImageDraw:

    def __init__(self, im):
	im.load()
	self.im = im.im
	self.ink = 255
	self.fill = 0
	self.font = None

    def setink(self, ink):
	self.ink = ink

    def setfill(self, onoff):
	self.fill = onoff

    def setfont(self, font):
	self.font = font

    def line(self, xy, xy1 = None):
	if xy1:
	    self.im.draw_line(xy, xy1, self.ink)
	else:
	    self.im.draw_lines(xy, self.ink)

    def point(self, xy):
	self.im.draw_points(xy, self.ink)

    def polygon(self, xy):
	self.im.draw_polygon(xy, self.ink, self.fill)

    def rectangle(self, xy):
	self.im.draw_rectangle(xy, self.ink, self.fill)

    def ellipse(self, xy):
	x1, y1, x2, y2 = xy
	xc, yc = (x1 + x2) / 2, (y1 + y2) / 2
	xr, yr = (x1 - x2) / 2, (y1 - y2) / 2
	# FIXME: rough; should use the Path module instead
	xy = []
	for i in range(0, 360, 5):
	    a = i * math.pi / 180.0
	    xy.append(xc+xr*math.cos(a), yc+yr*math.sin(a))
	self.im.draw_polygon(xy, self.ink, self.fill)

    def text(self, xy, text):
	import ImageFont
	if not self.font:
	    # FIXME: add font repository
	    self.font = ImageFont.load_path("BDF/courR14.pil")
	x, y = xy
	m = self.font.getmask(text)
	# FIXME: "paste ink" mechanism should be added
	self.im.paste(Image.core.fill("L", m.size, self.ink),
		      (x, y, x + m.size[0], y + m.size[1]), m.im)
