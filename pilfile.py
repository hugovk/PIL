#! /usr/local/bin/python
#
# The Python Imaging Library.
# $Id: pilfile.py,v 1.2 1996/12/16 18:49:51 fl Exp fl $
#
# a utility to identify image files
#
# this script identifies image files, extracting size and
# pixel mode information for known file formats.  Note that
# you don't need the PIL C extension to use this module.
#
# History:
# 0.0	95-09-01 fl	Created
# 0.1	96-05-18 fl	Modified options, added debugging mode
#

import Image

import getopt, sys

if len(sys.argv) == 1:
    print "PIL File 0.1/95-09-01 -- identify image files"
    print "Usage: pilfile [option] files..."
    print "Options:"
    print "  -f  list supported file formats"
    print "  -i  show associated info and tile data"
    print "  -q  quiet, don't warn for unidentified/missing files"
    sys.exit(1)

try:
    opt, argv = getopt.getopt(sys.argv[1:], "fqiD")
except getopt.error, v:
    print v
    sys.exit(1)

verbose = quiet = 0

for o, a in opt:
    if o == "-f":
        Image.init()
	id = Image.ID[:]
	id.sort()
	print "Supported formats:"
	for i in id:
	    print i, 
	sys.exit(1)
    elif o == "-i":
        verbose = 1
    elif o == "-q":
        quiet = 1
    elif o == "-D":
        Image.DEBUG = Image.DEBUG + 1

for file in argv:
    try:
	im = Image.open(file)
	print "%s:" % file, im.format, "%dx%d" % im.size, im.mode,
	if verbose:
	    print im.info, im.tile,
	print
    except IOError, v:
	if len(v) == 2:
	    x, v = v
	if not quiet:
	    print file, "failed:", v
