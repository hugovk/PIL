#
# The Python Imaging Library.
# $Id: PdfImagePlugin.py,v 1.4 1996/12/16 18:50:23 fl Exp fl $
#
# PDF (Acrobat) file handling
#
# History:
#	96-07-16 fl	Created: save works with L and RGB only
#
# Copyright (c) Fredrik Lundh 1996.  All rights reserved.
#


__version__ = "0.1"

import Image, ImageFile


#
# --------------------------------------------------------------------

# object ids:
#  1. catalogue
#  2. pages
#  3. image
#  4. image length
#  5. page
#  6. page contents
#  7. page content length

def _obj(fp, obj, **dict):
    fp.write("%d 0 obj\n" % obj)
    if dict:
	fp.write("<<\n")
	for k, v in dict.items():
	    fp.write("/%s %s\n" % (k, v))
	fp.write(">>\n")

def _endobj(fp):
    fp.write("endobj\n")

def _save(im, fp, filename):

    #
    # make sure image data is available
    im.load()

    xref = [0]*(7+1) # placeholders

    fp.write("%%PDF-1.1\n")
    fp.write("%% created by PIL PDF driver %s\n" % __version__)

    #
    # Get image characteristics

    width, height = im.size

    # FIXME: Should replace ASCIIHexDecode with RunLengthDecode (packbits)
    # or LZWDecode (tiff/lzw compression).  Note that PDF 1.2 also supports
    # DeflateDecode (zip compression).

    if im.mode == "1":
	c = "/ASCIIHexDecode", "/DeviceGray", "/ImageB", 1
    elif im.mode == "L":
	c = "/DCTDecode", "/DeviceGray", "/ImageB", 8
    elif im.mode == "P":
	c = "/ASCIIHexDecode", "/Indexed", "/ImageI", 8
    elif im.mode == "RGB":
	c = "/DCTDecode", "/DeviceRGB", "/ImageC", 8
    elif im.mode == "CMYK":
	c = "/DCTDecode", "/DeviceRGB", "/ImageC", 8
    else:
	raise ValueError, "illegal mode"

    filter, colorspace, proc, bits = c

    #
    # catalogue

    xref[1] = fp.tell()
    _obj(fp, 1, Type = "/Catalog",
		Pages = "2 0 R")
    _endobj(fp)

    #
    # pages

    xref[2] = fp.tell()
    _obj(fp, 2, Type = "/Pages",
		Count = 1,
		Kids = "[5 0 R]")
    _endobj(fp)

    #
    # image

    xref[3] = fp.tell()
    _obj(fp, 3, Type = "/XObject",
		Subtype = "/Image",
		Width = width,
		Height = height,
		Length = "4 0 R",
		Filter = filter,
		BitsPerComponent = bits,
		ColorSpace = colorspace)

    fp.write("stream\n")

    length = fp.tell()
    if filter == "/ASCIIHexDecode":
	ImageFile._save(im, fp, [("eps", (0,0)+im.size, 0, None)])
    elif filter == "/DCTDecode":
	ImageFile._save(im, fp, [("jpeg", (0,0)+im.size, 0, im.mode)])
    elif filter == "/RunLengthDecode":
	ImageFile._save(im, fp, [("packbits", (0,0)+im.size, 0, im.mode)])
    else:
	raise ValueError, "unsupported PDF filter"
    length = fp.tell() - length
    fp.write("\n")
    fp.write("endstream\n")

    _endobj(fp)

    xref[4] = fp.tell() # length of stream
    _obj(fp, 4)
    fp.write("%d\n" % length)
    _endobj(fp)

    #
    # page

    xref[5] = fp.tell()
    fp.write("5 0 obj\n<<\n/Type /Page\n/Parent 2 0 R\n"\
             "/Resources <<\n/ProcSet [ /PDF %s ]\n"\
             "/XObject << /image 3 0 R >>\n>>\n"\
	     "/MediaBox [ 0 0 %d %d ]\n/Contents 6 0 R\n>>\n" %\
	     (proc, width, height))

    #
    # page contents

    xref[6] = fp.tell()
    fp.write("6 0 obj\n<< /Length 7 0 R >>\nstream\n")
    length = fp.tell()
    fp.write("q %d 0 0 %d 0 0 cm /image Do Q\n" % (width, height))
    length = fp.tell() - length
    fp.write("endstream\nendobj\n")

    xref[7] = fp.tell() # length of stream
    fp.write("7 0 obj\n%d\nendobj\n" % length)

    #
    # trailer
    startxref = fp.tell()
    fp.write("xref\n0 %d\n0000000000 65535 f \n" % len(xref))
    for x in xref[1:]:
	fp.write("%010d 00000 n \n" % x)
    fp.write("trailer\n<<\n/Size %d\n/Root 1 0 R\n>>\n" % len(xref))
    fp.write("startxref\n%d\n%%%%EOF\n" % startxref)
    fp.flush()

#
# --------------------------------------------------------------------

Image.register_save("PDF", _save)

Image.register_extension("PDF", ".pdf")

Image.register_mime("PDF", "application/pdf")
