/*
 * The Python Imaging Library
 * $Id: Paste.c,v 1.4 1996/12/16 18:51:03 fl Exp fl $
 *
 * paste region into image, using mask image
 *
 * history:
 *	96-03-27 fl	Created
 *	96-07-16 fl	Support "1", "L" and "RGBA" masks
 *	96-08-16 fl	Merged with opaque paste	
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


#include "Imaging.h"

#define	BLEND(mask, in1, in2)\
	(mask == 0) ? in1 : (mask == 255) ? in2 :\
	in1 * (255 - mask) / 255 + in2 * mask / 255

#define	OFFSET(im, x, y)\
	(UINT8*) (im)->image[(y)] + (x)*(im)->pixelsize

#define PASTEMASK(operation, step)\
    for (y = 0; y < ysize; y++) {\
	UINT8* out = (UINT8*) OFFSET(imOut, x0, y+y0);\
	UINT8* in = (UINT8*) OFFSET(imIn, xoff, y+yoff);\
	UINT8* mask = (UINT8*) OFFSET(imMask, xoff, y+yoff);\
	xx = 0;\
	xm = step-1;\
	for (x = 0; x < xsize; x++) {\
	    for (i = 0; i < imOut->bands; i++)\
		out[xx+i] = operation;\
	    xx += imOut->pixelsize;\
	    xm += step;\
	}\
    }

int
ImagingPaste(Imaging imOut, Imaging imIn, Imaging imMask,
	     int x0, int y0, int x1, int y1)
{
    int x, y, i;
    int xm;
    int xx;
    int xsize, ysize;
    int xoff, yoff;

    if (imOut == NULL || imIn == NULL) {
	ImagingError_Argument(NULL);
	return -1;
    }

    /* Validate sizes */

    xsize = x1 - x0;
    ysize = y1 - y0;

    if (xsize != (int) imIn->xsize || ysize != (int) imIn->ysize) {
	ImagingError_Mismatch();
	return -1;
    }

    if (imMask &&
	(xsize != (int) imMask->xsize || ysize != (int) imMask->ysize)) {
	ImagingError_Mismatch();
	return -1;
    }

    /* Determine which region to copy */
    xoff = yoff = 0;
    if (x0 < 0)
	xsize += x0, xoff = -x0, x0 = 0;
    if (x0 + xsize > (int) imOut->xsize)
	xsize = imOut->xsize - x0;
    if (y0 < 0)
	ysize += y0, yoff = -y0, y0 = 0;
    if (y0 + ysize > (int) imOut->ysize)
	ysize = imOut->ysize - y0;

    if (xsize <= 0 || ysize <= 0)
	return 0;

    if (imMask == NULL) {

	for (y = 0; y < ysize; y++)
	    memcpy(imOut->image[y+y0]+x0*imIn->pixelsize,
		   imIn->image[y+yoff]+xoff*imIn->pixelsize,
		   xsize*imIn->pixelsize);

    } else if (strcmp(imMask->mode, "1") == 0) {

	/* "1" (1-bit) image. */
	PASTEMASK((mask[xm] == 0 ? out[xx+i] : in[xx+i]), 1);

    } else if (strcmp(imMask->mode, "L") == 0) {

	/* "L" (8-bit) alpha layer. */
	PASTEMASK(BLEND(mask[xm], out[xx+i], in[xx+i]), 1);

    } else if (strcmp(imMask->mode, "RGBA") == 0) {

	/* "A" alpha layer from "RGBA" image. */
	PASTEMASK(BLEND(mask[xm], out[xx+i], in[xx+i]), 4);

    } else {
	ImagingError_Argument("bad transparency mask");
	return -1;
    }

    return 0;
}
