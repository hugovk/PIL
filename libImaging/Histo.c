/*
 * The Python Imaging Library
 * $Id: Histo.c,v 1.4 1996/11/07 12:11:30 fredrik Exp $
 *
 * histogram support
 *
 * history:
 *	95-06-15 fl:	Created.
 *	96-04-05 fl:	Fixed histogram for multiband images.
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
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
ImagingHisto(Imaging im)
{
    unsigned x, y;
    ImagingHistogram h;

    if (!im)
	return ImagingError_Argument(NULL);

    h = ImagingHistogramNew(im);

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

    return h;
}
