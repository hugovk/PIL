/*
 * The Python Imaging Library
 * $Id: Negative.c,v 1.2 1996/11/07 12:11:30 fredrik Exp $
 *
 * negate image
 *
 * to do:
 *      FIXME: Maybe this should be implemented using ImagingPoint()
 *
 * history:
 *	95-11-27 fl:	Created
 *
 * Copyright (c) Fredrik Lundh 1995.  All rights reserved.
 */


#include "Imaging.h"


Imaging
ImagingNegative(Imaging im)
{
    Imaging imOut;
    unsigned x, y;

    if (!im)
	return (Imaging) ImagingError_Argument(NULL);

    imOut = ImagingNew(im->mode, im->xsize, im->ysize);
    if (!imOut)
	return NULL;

    for (y = 0; y < im->ysize; y++)
	for (x = 0; x < im->linesize; x++)
	     imOut->image[y][x] = ~im->image[y][x];

    return imOut;
}

