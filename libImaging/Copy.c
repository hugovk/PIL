/* 
 * The Python Imaging Library
 * $Id: Copy.c,v 1.3 1996/11/07 12:11:30 fredrik Exp $
 *
 * copy image
 *
 * history:
 *	95-11-26 fl:	Moved from Imaging.c
 *
 * Copyright (c) Fredrik Lundh 1995.  All rights reserved.
 */


#include "Imaging.h"


Imaging
ImagingCopy(Imaging im)
{
    Imaging imOut;
    unsigned y;

    if (!im)
	return (Imaging) ImagingError_Argument(NULL);

    imOut = ImagingNew(im->mode, im->xsize, im->ysize);
    if (!imOut)
	return NULL;

    ImagingCopyInfo(imOut, im);

    for (y = 0; y < im->ysize; y++)
	memcpy(imOut->image[y], im->image[y], im->linesize);

    return imOut;
}

