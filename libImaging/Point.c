/*
 * The Python Imaging Library
 * $Id: Point.c,v 1.4 1996/08/15 13:12:44 fredrik Exp $
 *
 * point (pixel) translation
 *
 * history:
 *	95-11-27 fl:	Created
 *	96-03-31 fl:	Fixed colour support
 *	96-08-13 fl:	Support 8-bit to "1" thresholding
 *	97-05-31 fl:	Added floating point transform
 *
 * Copyright (c) Fredrik Lundh 1995-97.
 * Copyright (c) Secret Labs AB 1997.
 *
 * See the README file for information on usage and redistribution.
 */


#include "Imaging.h"


Imaging
ImagingPoint(Imaging imIn, const char* mode, const UINT8* table)
{
    /* integer lookup table transform */

    Imaging imOut;
    int x, y, i;
    int xx;

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


Imaging
ImagingPointTransform(Imaging imIn, double scale, double offset)
{
    /* floating point transform */

    Imaging imOut;
    int x, y;

    if (!imIn || strcmp(imIn->mode, "F") != 0)
	return (Imaging) ImagingError_Argument(NULL);

    imOut = ImagingNew(imIn->mode, imIn->xsize, imIn->ysize);
    if (!imOut)
	return NULL;

    ImagingCopyInfo(imOut, imIn);

    for (y = 0; y < imIn->ysize; y++) {
        FLOAT32* in  = (FLOAT32*) imIn->image32[y];
        FLOAT32* out = (FLOAT32*) imOut->image32[y];
        for (x = 0; x < imIn->xsize; x++)
            out[x] = in[x] * scale + offset;
    }

    return imOut;
}
