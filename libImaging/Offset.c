/*
 * The Python Imaging Library
 * $Id: Offset.c,v 1.1 1996/08/15 13:12:44 fredrik Exp $
 *
 * offset an image in x and y directions
 *
 * history:
 *	96-07-22 fl:	Created
 *
 * Copyright (c) Fredrik Lundh 1996.
 * Copyright (c) Secret Labs AB 1997.
 *
 * See the README file for information on usage and redistribution.
 */


#include "Imaging.h"


Imaging
ImagingOffset(Imaging im, int xoffset, int yoffset)
{
    int x, y;
    Imaging imOut;

    if (!im)
	return (Imaging) ImagingError_Argument(NULL);

    imOut = ImagingNew(im->mode, im->xsize, im->ysize);
    if (!imOut)
	return NULL;

    ImagingCopyInfo(imOut, im);

#define	OFFSET(image)\
    for (y = 0; y < im->ysize; y++)\
	for (x = 0; x < im->xsize; x++) {\
	    int yi = (y - yoffset) % im->ysize;\
	    int xi = (x - xoffset) % im->xsize;\
	    imOut->image[y][x] = im->image[yi][xi];\
	}

    if (im->image8)
	OFFSET(image8)
    else
	OFFSET(image32)

    return imOut;
}
