#
# The Python Imaging Library
# $Id: player.py,v 1.1 1996/12/16 18:51:31 fl Exp fl $
#

from Tkinter import *
import Image, ImageTk
import sys

Image.DEBUG = 0

#
# an image animation player

class UI(Label):

    def __init__(self, master, im):
	self.im = im
	if im.mode == "1":
	    self.image = ImageTk.BitmapImage(im, foreground="white")
	else:
	    self.image = ImageTk.PhotoImage(im)
	Label.__init__(self, master, image=self.image, bg="black", bd=0)
	self.update()
	self.after(0, self.next)

    def next(self):
	try:
	    self.im.seek(self.im.tell() + 1)
	    self.image.paste(self.im)
	except EOFError:
	    return # end of file
	self.update_idletasks()
	self.after(0, self.next)


#
# script interface

if __name__ == "__main__":

    if not sys.argv[1:]:
	print "Syntax: python player.py imagefile"
	sys.exit(1)

    filename = sys.argv[1]

    root = Tk()
    root.title(filename)

    UI(root, Image.open(filename)).pack()

    root.mainloop()
