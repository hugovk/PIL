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
# 0.3	96-12-30 fl	Optimize output (currently works with PNG only)
#

import Image

import getopt, sys

def usage():
    print "PIL Convert 0.3/96-12-30 -- convert image files"
    print "Usage: pilconvert [option] infile outfile"
    print "Options:"
    print "  -c <format>  convert to format (default is given by outfile)"
    print "  -1           convert to bilevel"
    print "  -g           convert to greyscale"
    print "  -r           convert to rgb"
    print "  -p           progressive output (JPEG only)"
    print "  -f           list supported file formats"
    sys.exit(1)

if len(sys.argv) == 1:
    usage()

try:
    opt, argv = getopt.getopt(sys.argv[1:], "dc:1gpr")
except getopt.error, v:
    print v
    sys.exit(1)

format = None
convert = None

options = {
    "optimize": 1
}

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
    elif o == "-1":
	convert = "1"
    elif o == "-g":
	convert = "L"
    elif o == "-p":
	options["progression"] = 1
    elif o == "-r":
	convert = "RGB"

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
