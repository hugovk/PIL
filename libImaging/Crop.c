/*
 * The Python Imaging Library
 * $Id: Crop.c,v 1.2 1996/05/09 21:20:18 fredrik Exp $
 *
 * cut region from image
 *
 * to do:
 *	FIXME: Allow regions outside the source image.
 *
 * history:
 *	95-11-27 fl:	Created
 *
 * Copyright (c) Fredrik Lundh 1995.
 * Copyright (c) Secret Labs AB 1997.
 *
 * See the README file for information on usage and redistribution.
 */


#include "Imaging.h"


Imaging
ImagingCrop(Imaging im, int x0, int y0, int x1, int y1)
{
    Imaging imOut;
    int y;
    int xsize, ysize;
    int xoff, yoff;

    if (!im)
	return (Imaging) ImagingError_Argument(NULL);

    xsize = x1 - x0;
    ysize = y1 - y0;

    imOut = ImagingNew(im->mode, x1-x0, y1-y0);
    if (!imOut)
	return NULL;

    ImagingCopyInfo(imOut, im);

    /* Determine which region to copy */
    xoff = yoff = 0;
    if (x0 < 0)
	xsize += x0, xoff = -x0, x0 = 0;
    if (x0 + xsize > imOut->xsize)
	xsize = imOut->xsize - x0;
    if (y0 < 0)
	ysize += y0, yoff = -y0, y0 = 0;
    if (y0 + ysize > imOut->ysize)
	ysize = imOut->ysize - y0;

    if (xsize <= 0 || ysize <= 0)
	return 0;



    for (y = 0; y < imOut->ysize; y++)
	memcpy(imOut->image[y], im->image[y+y0]+x0*im->pixelsize,
	       imOut->linesize);

    return imOut;
}

