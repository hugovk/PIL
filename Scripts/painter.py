#
# The Python Imaging Library
# $Id: painter.py,v 1.1 1996/12/16 18:51:31 fl Exp fl $
#
# this demo script illustrates pasting into an already displayed
# photoimage.  note that the current version of Tk updates the whole
# image everytime we paste, so to get decent performance, we create
# a set of tiles.
#
# to be fixed:
# - the tiling code assumes that the image size is a multiple of
#   the tile size
#

from Tkinter import *
import Image, ImageTk
import sys

#
# painter widget

class PaintCanvas(Canvas):
    def __init__(self, master, image):
	Canvas.__init__(self, master, width=image.size[0], height=image.size[1])

	# fill the canvas
	self.tile = {}
	self.tilesize = tilesize = 32
	for x in range(0, image.size[0], tilesize):
	    for y in range(0, image.size[1], tilesize):
		box = x, y, x+tilesize, y+tilesize
		tile = ImageTk.PhotoImage(image.crop(box))
		self.create_image(x, y, image=tile, anchor=NW)
		self.tile[(x,y)] = box, tile

	self.image = image

	self.bind("<B1-Motion>", self.paint)

    def paint(self, event):
	xy = event.x - 10, event.y - 10, event.x + 10, event.y + 10
	#
	# NOTES:
	# im = self.image.crop(xy) may crash if the region extends
	# outside the image.  the transform(EXTENT) method provides
	# a workaround:
	im = self.image.transform((xy[2]-xy[0], xy[3]-xy[1]),
			          Image.EXTENT, xy)

	# process the image in some fashion
        im = im.convert("L")

	self.image.paste(im, xy)
	self.repair(xy)

    def repair(self, box):
	# update canvas
	dx = box[0] % self.tilesize
	dy = box[1] % self.tilesize
	for x in range(box[0]-dx, box[2]+1, self.tilesize):
	    for y in range(box[1]-dy, box[3]+1, self.tilesize):
		try:
		    xy, tile = self.tile[(x, y)]
		    tile.paste(self.image.crop(xy))
		except KeyError:
		    pass # outside the image
	self.update_idletasks()

#
# main

root = Tk()

im = Image.open(sys.argv[1])

if im.mode != "RGB":
    im = im.convert("RGB")

PaintCanvas(root, im).pack()

root.mainloop()
