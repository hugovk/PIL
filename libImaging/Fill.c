/*
 * The Python Imaging Library
 * $Id: Fill.c,v 1.3 1996/11/07 12:11:30 fredrik Exp $
 *
 * fill image with constant pixel value
 *
 * history:
 *	95-11-26 fl:	Moved from Imaging.c
 *	96-05-17 fl:	Added radial fill, renamed wedge to linear
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


#include "Imaging.h"

#include "math.h"


Imaging
ImagingFill(const char *mode, unsigned xsize, unsigned ysize, void *colour)
{
    Imaging imOut;
    INT32 c;
    unsigned x, y;

    imOut = ImagingNew(mode, xsize, ysize);
    if (!imOut)
	return NULL;

    c = 0L;
    memcpy(&c, colour, imOut->bands);

    if (imOut->image32 && c != 0L)
	for (y = 0; y < ysize; y++)
	    for (x = 0; x < xsize; x++)
		imOut->image32[y][x] = c;
    else
	for (y = 0; y < ysize; y++)
	    memset(imOut->image[y], (unsigned char) c, imOut->linesize);

    return imOut;
}

Imaging
ImagingFillLinearGradient(const char *mode)
{
    Imaging im;
    unsigned y;

    if (strlen(mode) != 1)
	return (Imaging) ImagingError_Argument("Bad mode for linear fill");

    im = ImagingNew(mode, 256, 256);
    if (!im)
	return NULL;

    for (y = 0; y < 256; y++)
	memset(im->image8[y], (unsigned char) y, 256);

    return im;
}

Imaging
ImagingFillRadialGradient(const char *mode)
{
    Imaging im;
    unsigned x, y;
    int d;

    if (strlen(mode) != 1)
	return (Imaging) ImagingError_Argument("Bad mode for radial fill");

    im = ImagingNew(mode, 256, 256);
    if (!im)
	return NULL;

    for (y = 0; y < 256; y++)
	for (x = 0; x < 256; x++) {
	    d = (int) (sqrt((x-128) * (x-128) + (y-128) * (y-128)) * 2.0);
	    if (d >= 255)
		im->image8[y][x] = 255;
	    else
		im->image8[y][x] = d;
	}

    return im;
}
