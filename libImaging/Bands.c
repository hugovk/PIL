/* 
 * The Python Imaging Library
 * $Id: Bands.c,v 1.3 1996/11/07 12:11:30 fredrik Exp $
 * 
 * stuff to extract and paste back individual bands
 *
 * history:
 *	96-03-20 fl:	Created
 *
 * Copyright (c) Fredrik Lundh 1996.  All rights reserved.
 */


#include "Imaging.h"


Imaging
ImagingGetBand(Imaging imIn, int band)
{
    Imaging imOut;
    unsigned x, y;

    /* Check arguments */
    if (!imIn || band < 0 || band >= imIn->bands)
	return (Imaging) ImagingError_Mismatch();

    /* Shortcuts */
    if (imIn->bands == 1)
	return ImagingCopy(imIn);

    imOut = ImagingNew("L", imIn->xsize, imIn->ysize);
    if (!imOut)
	return NULL;

    /* Extract band from image */
    for (y = 0; y < imIn->ysize; y++) {
	UINT8* in = (UINT8*) imIn->image[y] + band;
	UINT8* out = imOut->image8[y];
	for (x = 0; x < imIn->xsize; x++) {
	    *out++ = *in;
	    in += 4;
	}
    }

    return imOut;
}

int
ImagingPutBand(Imaging imOut, Imaging imIn, int band)
{
    unsigned x, y;

    /* Check arguments */
    if (!imIn || imIn->bands != 1 ||
	!imOut || band < 0 || band >= imOut->bands) {
	ImagingError_Argument(NULL);
	return -1;
    }
    if (imIn->type  != imOut->type  ||
	imIn->depth != imOut->depth ||
	imIn->xsize != imOut->xsize ||
	imIn->ysize != imOut->ysize) {
	ImagingError_Mismatch();
	return -1;
    }

    /* Extract band from image */
    for (y = 0; y < imIn->ysize; y++) {
	UINT8* in = imIn->image8[y];
	UINT8* out = (UINT8*) imOut->image[y] + band;
	for (x = 0; x < imIn->xsize; x++) {
	    *out = *in++;
	    out += 4;
	}
    }
    return 0;
}
