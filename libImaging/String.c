/*
 * The Python Imaging Library
 * $Id: String.c,v 1.3 1996/11/07 12:11:30 fredrik Exp $
 *
 * store image in string
 *
 * history:
 *	96-03-23 fl:	ToString
 *	96-03-25 fl:	Added FromString
 *
 * Copyright (c) Fredrik Lundh 1996.  All rights reserved.
 */


#include "Imaging.h"


int
ImagingFromString(Imaging im, int lineno, char *buffer)
{
    unsigned y;

    /* Assume there's enough data in the buffer */
    if (!im || !buffer) {
	ImagingError_Argument(NULL);
	return -1;
    }

    if (lineno < 0)
	/* Backwards */
	for (y = 0; y < im->ysize; y++) {
	    memcpy(im->image[im->ysize-y-1], buffer, im->linesize);
	    buffer += im->linesize;
	}
    else
	memcpy(im->image[lineno], buffer, im->linesize);

    return 0;
}


int
ImagingToString(Imaging im, int lineno, char *buffer)
{
    unsigned y;

    if (!im) {
	ImagingError_Argument(NULL);
	return -1;
    }

    if (lineno < 0)
	/* Backwards */
	for (y = 0; y < im->ysize; y++) {
	    memcpy(buffer, im->image[im->ysize-y-1], im->linesize);
	    buffer += im->linesize;
	}
    else
	memcpy(buffer, im->image[lineno], im->linesize);

    return 0;
}
