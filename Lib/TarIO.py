#
# The Python Imaging Library.
# $Id: TarIO.py,v 1.3 1996/11/10 17:52:14 fredrik Exp $
#
# read files from within a tar file
#
# History:
#	95-06-18 fl	Created
#	96-05-28 fl	Open files in binary mode
#
# Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
#

import ContainerIO
import string

class TarIO(ContainerIO.ContainerIO):

    def __init__(self, tarfile, file):

	fh = open(tarfile, "rb")

	while 1:

	    s = fh.read(512)
	    if len(s) != 512:
		raise IOError, "unexpected end of tar file"

	    name = s[:100]
	    i = string.find(name, chr(0))
	    if i == 0:
		raise IOError, "cannot find subfile"
	    if i > 0:
		name = name[:i]

	    size = string.atoi(s[124:136], 8)

	    if file == name:
		break

	    fh.seek((size + 511) & -512, 1)

	# Open region
	ContainerIO.ContainerIO.__init__(self, fh, fh.tell(), size)
