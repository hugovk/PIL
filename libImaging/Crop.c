/*
 * The Python Imaging Library
 * $Id: Crop.c,v 1.3 1996/11/07 12:11:30 fredrik Exp $
 *
 * cut region from image
 *
 * to do:
 *	FIXME: Allow regions outside the source image.
 *
 * history:
 *	95-11-27 fl:	Created
 *
 * Copyright (c) Fredrik Lundh 1995.  All rights reserved.
 */


#include "Imaging.h"


Imaging
ImagingCrop(Imaging im, int x0, int y0, int x1, int y1)
{
    Imaging imOut;
    unsigned y;

    if (!im)
	return (Imaging) ImagingError_Argument(NULL);

    imOut = ImagingNew(im->mode, x1-x0, y1-y0);
    if (!imOut)
	return NULL;

    ImagingCopyInfo(imOut, im);

    for (y = 0; y < imOut->ysize; y++)
	memcpy(imOut->image[y], im->image[y+y0]+x0*im->pixelsize,
	       imOut->linesize);

    return imOut;
}

