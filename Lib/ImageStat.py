#
# The Python Imaging Library.
# $Id: ImageStat.py,v 1.2 1996/11/10 17:52:14 fredrik Exp $
#
# global statistics
#
# History:
#	96-04-05 fl	Created
#
# Copyright (c) Fredrik Lundh 1996.  All rights reserved.
#

import Image

class Global:
    "Global image statistics"

    def __init__(self, image):
	self.h = image.histogram()
	self.bands = range(len(self.h) / 256)

    def __getattr__(self, id):
	"Calculate missing attribute"

	if id[:6] == "_calc_":
	    raise AttributeError, id
	v = getattr(self, "_calc_" + id)()
	setattr(self, id, v)
	return v

    def _calc_extrema(self):
	"Get min/max values for each band in the image"

	def minmax(histogram):
	    n = 255
	    x = 0
	    for i in range(256):
		if histogram[i]:
		    n = min(n, i)
		    x = max(x, i)
	    return n, x # returns (255, 0) if there's no data in the histogram

	e = []
	for i in range(0, len(self.h), 256):
	    e.append(minmax(self.h[i:]))
	return e

    def _calc_count(self):
	"Get total number of pixels in each layer"

	n = []
	for i in range(0, len(self.h), 256):
	    n.append(reduce(lambda a, b: a+b, self.h[i:i+256]))
	return n

    def _calc_sum(self):
	"Get sum of all pixels in each layer"

	s = []
	for i in range(0, len(self.h), 256):
	    sum = 0
	    for j in range(256):
		sum = sum + j * self.h[i+j]
	    s.append(sum)
	return s

    def _calc_mean(self):
	"Get average pixel level for each layer"

	m = []
	for i in self.bands:
	    m.append(self.sum[i] / self.count[i])
	return m

	# ...to be continued...
