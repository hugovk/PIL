/*
 * The Python Imaging Library
 * $Id: Histo.c,v 1.3 1996/04/26 23:32:15 fredrik Exp $
 *
 * histogram support
 *
 * history:
 *	95-06-15 fl:	Created.
 *	96-04-05 fl:	Fixed histogram for multiband images.
 *	97-02-23 fl:	Added mask support
 *
 * Copyright (c) Fredrik Lundh 1995-97.
 * Copyright (c) Secret Labs AB 1997.
 *
 * See the README file for information on usage and redistribution.
 */


#include "Imaging.h"


/* HISTOGRAM */
/* --------------------------------------------------------------------
 * Take a histogram of an image. Returns a histogram object containing
 * 256 slots per band in the input image.
 */

void
ImagingHistogramDelete(ImagingHistogram h)
{
    if (h->histogram)
	free(h->histogram);
    free(h);
}

ImagingHistogram
ImagingHistogramNew(Imaging im)
{
    ImagingHistogram h;

    /* Create histogram descriptor */
    h = calloc(1, sizeof(struct ImagingHistogramInstance));
    strcpy(h->mode, im->mode);
    h->bands = im->bands;
    h->histogram = calloc(im->pixelsize, 256 * sizeof(long));

    return h;
}

ImagingHistogram
ImagingGetHistogram(Imaging im, Imaging imMask)
{
    int x, y;
    ImagingHistogram h;

    if (!im || im->type != 0 || im->depth != 8)
	return ImagingError_Argument(NULL);

    if (imMask) {
	/* Validate mask */
	if (im->xsize != imMask->xsize || im->ysize != imMask->ysize)
	    return ImagingError_Mismatch();
	if (strcmp(imMask->mode, "1") != 0 && strcmp(imMask->mode, "L") != 0)
	    return ImagingError_Argument("bad transparency mask");
    }

    h = ImagingHistogramNew(im);

    if (imMask) {
	/* mask */
	if (im->image8) {
	    for (y = 0; y < im->ysize; y++)
		for (x = 0; x < im->xsize; x++)
		    if (imMask->image8[y][x] != 0)
			h->histogram[im->image8[y][x]]++;
	} else { /* yes, we need the braces. C isn't Python! */
	    for (y = 0; y < im->ysize; y++) {
		UINT8* in = (UINT8*) im->image[y];
		for (x = 0; x < im->xsize; x++)
		    if (imMask->image8[y][x] != 0) {
			h->histogram[(*in++)]++;
			h->histogram[(*in++)+256]++;
			h->histogram[(*in++)+512]++;
			h->histogram[(*in++)+768]++;
		    } else
			in += 4;
	    }
	}
    } else {
	/* mask not given; process pixels in image */
	if (im->image8)
	    for (y = 0; y < im->ysize; y++)
		for (x = 0; x < im->xsize; x++)
		    h->histogram[im->image8[y][x]]++;
	else
	    for (y = 0; y < im->ysize; y++) {
		UINT8* in = (UINT8*) im->image[y];
		for (x = 0; x < im->xsize; x++) {
		    h->histogram[(*in++)]++;
		    h->histogram[(*in++)+256]++;
		    h->histogram[(*in++)+512]++;
		    h->histogram[(*in++)+768]++;
		}
	    }
    }

    return h;
}
