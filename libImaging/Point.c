/*
 * The Python Imaging Library
 * $Id: Point.c,v 1.5 1996/11/07 12:11:30 fredrik Exp $
 *
 * point (pixel) translation
 *
 * history:
 *	95-11-27 fl:	Created
 *	96-03-31 fl:	Fixed colour support
 *	96-08-13 fl:	Support 8-bit to "1" thresholding
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


#include "Imaging.h"


Imaging
ImagingPoint(Imaging imIn, const char* mode, const UINT8* table)
{
    Imaging imOut;
    unsigned x, y, i;
    unsigned xx;

    if (!imIn || (mode && strcmp(mode, "1") != 0))
	return (Imaging) ImagingError_Argument(NULL);

    if (mode && !imIn->image8)
	return (Imaging) ImagingError_Mismatch();

    imOut = ImagingNew((mode) ? mode : imIn->mode, imIn->xsize, imIn->ysize);
    if (!imOut)
	return NULL;

    ImagingCopyInfo(imOut, imIn);

    if (imIn->image8)
	for (y = 0; y < imIn->ysize; y++)
	    for (x = 0; x < imIn->xsize; x++)
		imOut->image[y][x] = table[(UINT8)imIn->image[y][x]];
    else {
	for (y = 0; y < imIn->ysize; y++) {
	    UINT8* in  = (UINT8*) imIn->image[y];
	    UINT8* out = (UINT8*) imOut->image[y];
	    xx = 0;
	    for (x = 0; x < imIn->xsize; x++) {
		for (i = 0; i < imOut->bands; i++)
		    out[xx+i] = table[in[xx+i] + i * 256];
		xx += 4;
	    }
	}
    }

    return imOut;
}
