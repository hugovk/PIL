#
# The Python Imaging Library
# $Id$
#
# split an animation into a number of frame files
#

import Image
import os, sys

if not sys.argv[2:]:
    print "Syntax: python explode.py infile template"
    print "The template argument is used to construct the names of"
    print "the individual frame files.  Use %d to insert the frame"
    print "number (1..n)."
    sys.exit(1)

infile = sys.argv[1]
outfile = sys.argv[2]

try:
    outfile % 1
except TypeError:
    file, ext = os.path.splitext(outfile)
    outfile = file + "%03d" + ext

ix = 1

im = Image.open(infile)

while 1:

    im.save(outfile % ix)
    print outfile % ix

    try:
	im.seek(ix)
    except EOFError:
	break

    ix = ix + 1
