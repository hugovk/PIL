#! /usr/local/bin/python
#
# The Python Imaging Library.
# $Id: pilconvert.py,v 1.2 1996/12/16 18:49:51 fl Exp fl $
#
# convert image files
#
# History:
# 0.1	96-04-20 fl	Created
# 0.2a1	96-10-04 fl	Use draft mode when converting images
#

import Image

import getopt, sys

def usage():
    print "PIL Convert 0.2a1/96-10-04 -- convert image files"
    print "Usage: pilconvert [option] infile outfile"
    print "Options:"
    print "  -c <format>  convert to format (default is given by outfile)"
    print "  -1           convert to bilevel"
    print "  -g           convert to greyscale"
    print "  -r           convert to rgb"
    print "  -d           list supported file formats"
    sys.exit(1)

if len(sys.argv) == 1:
    usage()

try:
    opt, argv = getopt.getopt(sys.argv[1:], "dc:1gr")
except getopt.error, v:
    print v
    sys.exit(1)

format = None
convert = None

for o, a in opt:
    if o == "-d":
        Image.init() # cheating
	print Image.ID
	sys.exit(1)
    elif o == "-c":
	format = a
    elif o == "-1":
	convert = "1"
    elif o == "-g":
	convert = "L"
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
	im.save(argv[1], format)
    else:
	im.save(argv[1])
except:
    print "cannot convert image",
    print "(%s:%s)" % (sys.exc_type, sys.exc_value)
