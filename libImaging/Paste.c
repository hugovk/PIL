/*
 * The Python Imaging Library
 * $Id: Paste.c,v 1.2 1996/10/04 19:41:10 fredrik Exp $
 *
 * paste region into image, using mask image
 *
 * history:
 *	96-03-27 fl	Created
 *	96-07-16 fl	Support "1", "L" and "RGBA" masks
 *	96-08-16 fl	Merged with opaque paste	
 *	97-01-17 fl	Faster blending, added support for RGBa images
 *	97-08-27 fl	Faster masking for 32-bit images
 *	98-02-02 fl	Fixed MULDIV255 macro for gcc
 *
 * Copyright (c) Fredrik Lundh 1996-97.
 * Copyright (c) Secret Labs AB 1997.
 *
 * See the README file for information on usage and redistribution.
 */

#include "Imaging.h"

/* like (a * b + 127) / 255), but much faster on most platforms */
#define	MULDIV255NEW(a, b, tmp)\
     	(tmp = (a) * (b) + 128, ((((tmp) >> 8) + (tmp)) >> 8))

#define	MULDIV255OLD(a, b, tmp)\
        (((a) * (b) + 127) / 255)

#define MULDIV255 MULDIV255NEW

#define	BLEND(mask, in1, in2, tmp1, tmp2)\
	(MULDIV255(in1, 255 - mask, tmp1) + MULDIV255(in2, mask, tmp2))

#define	COMPOSE(mask, in1, in2, tmp1, tmp2)\
	(MULDIV255(in1, 255 - mask, tmp1) + in2)

#define	OFFSET(im, x, y)\
	((UINT8*) (im)->image[(y)]) + (x)*((im)->pixelsize)

#define	OFFSET32(im, x, y)\
	((UINT32*) (im)->image32[(y)]) + (x)

#define PASTEMASK(operation, step)\
    for (y = 0; y < ysize; y++) {\
	UINT8* out = OFFSET(imOut, x0, y+y0);\
	UINT8* in = OFFSET(imIn, xoff, y+yoff);\
	UINT8* mask = OFFSET(imMask, xoff, y+yoff);\
	xx = 0;\
	xm = step-1;\
	for (x = 0; x < xsize; x++) {\
	    for (i = 0; i < imOut->pixelsize; i++)\
		out[xx+i] = operation;\
	    xx += imOut->pixelsize;\
	    xm += step;\
	}\
    }

#define PASTEMASK32(operation)\
    for (y = 0; y < ysize; y++) {\
	UINT32* out = OFFSET32(imOut, x0, y+y0);\
	UINT32* in = OFFSET32(imIn, xoff, y+yoff);\
	UINT8* mask = OFFSET(imMask, xoff, y+yoff);\
	for (x = 0; x < xsize; x++) {\
            out[x] = operation;\
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
    unsigned int tmp1, tmp2;

    if (imOut == NULL || imIn == NULL) {
	ImagingError_Argument(NULL);
	return -1;
    }

    /* Check sizes */

    xsize = x1 - x0;
    ysize = y1 - y0;

    if (xsize != imIn->xsize || ysize != imIn->ysize) {
	ImagingError_Mismatch();
	return -1;
    }

    if (imMask &&
	(xsize != imMask->xsize || ysize != imMask->ysize)) {
	ImagingError_Mismatch();
	return -1;
    }

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

    if (imMask == NULL) {

        /* opaque */
	for (y = 0; y < ysize; y++)
	    memcpy(imOut->image[y+y0]+x0*imIn->pixelsize,
		   imIn->image[y+yoff]+xoff*imIn->pixelsize,
		   xsize*imIn->pixelsize);

    } else if (strcmp(imMask->mode, "1") == 0) {

	/* "1" (1-bit) mask */
        if (imIn->image8) {
            PASTEMASK((mask[xm] == 0 ? out[xx+i] : in[xx+i]), 1);
        } else {
            PASTEMASK32((mask[x] == 0 ? out[x] : in[x]));
        }

    } else if (strcmp(imMask->mode, "L") == 0) {

	/* "L" (8-bit) matte layer. */
	PASTEMASK(BLEND(mask[xm], out[xx+i], in[xx+i], tmp1, tmp2), 1);

    } else if (strcmp(imMask->mode, "RGBA") == 0) {

	/* "A" matte layer from "RGBA" image. */
	PASTEMASK(BLEND(mask[xm], out[xx+i], in[xx+i], tmp1, tmp2), 4);

    } else if (strcmp(imMask->mode, "RGBa") == 0) {

	/* "a" matte layer from "RGBa" image. */
	PASTEMASK(COMPOSE(mask[xm], out[xx+i], in[xx+i], tmp1, tmp2), 4);

    } else {
	ImagingError_Argument("bad transparency mask");
	return -1;
    }

    return 0;
}
