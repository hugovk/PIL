#! /usr/local/bin/python
#
# The Python Imaging Library.
# $Id: pilconvert.py,v 1.1 1996/10/04 19:40:40 fredrik Exp $
#
# convert image files
#
# History:
# 0.1	96-04-20 fl	Created
# 0.2	96-10-04 fl	Use draft mode when converting images
# 0.3	96-12-30 fl	Optimize output (PNG, JPEG)
# 0.4	97-01-18 fl	Made optimize an option (PNG, JPEG)
#

import Image

import getopt, string, sys

def usage():
    print "PIL Convert 0.4/97-01-18 -- convert image files"
    print "Usage: pilconvert [option] infile outfile"
    print
    print "Options:"
    print
    print "  -c <format>  convert to format (default is given by outfile)"
    print
    print "  -g           convert to greyscale"
    print "  -p           convert to palette image"
    print "  -r           convert to rgb"
    print
    print "  -o           optimize output (trade speed for size)"
    print "  -q <value>   set compression quality (0-100, JPEG only)"
    print
    print "  -f           list supported file formats"
    sys.exit(1)

if len(sys.argv) == 1:
    usage()

try:
    opt, argv = getopt.getopt(sys.argv[1:], "dc:gopq:r")
except getopt.error, v:
    print v
    sys.exit(1)

format = None
convert = None

options = { }

for o, a in opt:

    if o == "-f":
        Image.init()
	id.sort()
	print "Supported formats:"
	for i in id:
	    print i, 
	sys.exit(1)

    elif o == "-c":
	format = a

    if o == "-g":
	convert = "L"
    elif o == "-p":
	convert = "P"
    elif o == "-r":
	convert = "RGB"

    elif o == "-o":
	options["optimize"] = 1
    elif o == "-q":
	options["quality"] = string.atoi(a)

if len(argv) != 2:
    usage()

try:
    im = Image.open(argv[0])
    if convert and im.mode != convert:
	im.draft(convert, im.size)
	im = im.convert(convert)
    if format:
	apply(im.save, (argv[1], format), options)
    else:
	apply(im.save, (argv[1],), options)
except:
    print "cannot convert image",
    print "(%s:%s)" % (sys.exc_type, sys.exc_value)
