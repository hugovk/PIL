/*
 * The Python Imaging Library
 * $Id: Geometry.c,v 1.9 1996/12/08 15:32:31 fl Exp $
 *
 * the imaging geometry methods
 *
 * history:
 *	95-06-15 fl:	Created
 *	96-04-15 fl:	Changed origin
 *	96-05-18 fl:	Fixed rotate90/270 for rectangular images
 *	96-05-27 fl:	Added general purpose transform
 *	96-11-22 fl:	Don't crash when resizing from outside source image
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


#include "Imaging.h"


/* -------------------------------------------------------------------- */
/* Transpose operations							*/

Imaging
ImagingFlipLeftRight(Imaging im)
{
    Imaging imOut;
    unsigned x, y, xr;

    if (!im)
	return (Imaging) ImagingError_Argument(NULL);

    imOut = ImagingNew(im->mode, im->xsize, im->ysize);
    if (!imOut)
	return NULL;

    ImagingCopyInfo(imOut, im);

#define	FLIP_HORIZ(image)\
    for (y = 0; y < im->ysize; y++) {\
	xr = im->xsize-1;\
	for (x = 0; x < im->xsize; x++, xr--)\
	    imOut->image[y][x] = im->image[y][xr];\
    }

    if (im->image8)
	FLIP_HORIZ(image8)
    else
	FLIP_HORIZ(image32)

    return imOut;
}


Imaging
ImagingFlipTopBottom(Imaging im)
{
    Imaging imOut;
    unsigned y, yr;

    if (!im)
	return (Imaging) ImagingError_Argument(NULL);

    imOut = ImagingNew(im->mode, im->xsize, im->ysize);
    if (!imOut)
	return NULL;

    ImagingCopyInfo(imOut, im);

    yr = im->ysize-1;
    for (y = 0; y < im->ysize; y++, yr--)
	memcpy(imOut->image[yr], im->image[y], im->linesize);

    return imOut;
}


Imaging
ImagingRotate90(Imaging im)
{
    Imaging imOut;
    unsigned x, y, xr;

    if (!im)
	return (Imaging) ImagingError_Argument(NULL);

    imOut = ImagingNew(im->mode, im->ysize, im->xsize);
    if (!imOut)
	return NULL;

    ImagingCopyInfo(imOut, im);

#define	ROTATE_90(image)\
    for (y = 0; y < im->ysize; y++) {\
	xr = im->xsize-1;\
	for (x = 0; x < im->xsize; x++, xr--)\
	    imOut->image[xr][y] = im->image[y][x];\
    }

    if (im->image8)
	ROTATE_90(image8)
    else
	ROTATE_90(image32)

    return imOut;
}


Imaging
ImagingRotate180(Imaging im)
{
    Imaging imOut;
    unsigned x, y, xr, yr;

    if (!im)
	return (Imaging) ImagingError_Argument(NULL);

    imOut = ImagingNew(im->mode, im->xsize, im->ysize);
    if (!imOut)
	return NULL;

    ImagingCopyInfo(imOut, im);

    yr = im->ysize-1;

#define	ROTATE_180(image)\
    for (y = 0; y < im->ysize; y++, yr--) {\
	xr = im->xsize-1;\
	for (x = 0; x < im->xsize; x++, xr--)\
	    imOut->image[y][x] = im->image[yr][xr];\
    }

    if (im->image8)
	ROTATE_180(image8)
    else
	ROTATE_180(image32)

    return imOut;
}


Imaging
ImagingRotate270(Imaging im)
{
    Imaging imOut;
    unsigned x, y, yr;

    if (!im)
	return (Imaging) ImagingError_Argument(NULL);

    imOut = ImagingNew(im->mode, im->ysize, im->xsize);
    if (!imOut)
	return NULL;

    ImagingCopyInfo(imOut, im);

    yr = im->ysize - 1;

#define	ROTATE_270(image)\
    for (y = 0; y < im->ysize; y++, yr--)\
	for (x = 0; x < im->xsize; x++)\
	    imOut->image[x][y] = im->image[yr][x];

    if (im->image8)
	ROTATE_270(image8)
    else
	ROTATE_270(image32)

    return imOut;
}


/* -------------------------------------------------------------------- */
/* Transforms								*/

static Imaging
ImagingScaleAffine(Imaging im, unsigned xsize, unsigned ysize,
		   double a[6], int fill)
{
    Imaging imOut;
    unsigned x, y;
    int xin;
    double xo, yo;
    unsigned xmin, xmax;
    int *xinp;

    if (!im)
	return (Imaging) ImagingError_Argument(NULL);

    imOut = ImagingNew(im->mode, xsize, ysize);
    if (!imOut)
	return NULL;

    ImagingCopyInfo(imOut, im);

    xinp = (int *) malloc(xsize * sizeof(int));
    if (!xinp) {
	ImagingDelete(imOut);
	return (Imaging) ImagingError_NoMemory();
    }

    xmin = xsize;
    xmax = 0;

    xo = a[0];
    yo = a[3];

    /* Pretabulate horizontal pixel positions */
    for (x = 0; x < xsize; x++) {
	xin = (int) xo;
	if (xin >= 0 && xin < (int) im->xsize) {
	    xmax = x+1;
	    if (x < xmin)
		xmin = x;
	    xinp[x] = xin;
	}
	xo += a[1];
    }

#define	AFFINE_SCALE(pixel, image)\
    for (y = 0; y < ysize; y++) {\
	int yi = (int) yo;\
	pixel *in, *out;\
	out = imOut->image[y];\
	if (yi >= 0 && yi < im->ysize) {\
	    in = im->image[(int) yo];\
	    if (fill) {\
	        for (x = 0; x < xmin; x++)\
		    out[x] = 0;\
	        for (; x < xmax; x++)\
		    out[x] = in[xinp[x]];\
	        for (; x < xsize; x++)\
		    out[x] = 0;\
	    } else\
	        for (x = xmin; x < xmax; x++)\
		    out[x] = in[xinp[x]];\
	} else if (fill)\
	    for (x = 0; x < xsize; x++)\
		out[x] = 0;\
	yo += a[5];\
    }

    if (im->image8)
	AFFINE_SCALE(UINT8, image8)
    else
	AFFINE_SCALE(INT32, image32)

    free(xinp);

    return imOut;
}


Imaging
ImagingTransformAffine(Imaging im, unsigned xsize, unsigned ysize,
		       double a[6], int fill)
{
    Imaging imOut;
    unsigned x, y;
    int xin, yin;
    double xx, yy;
    double xo, yo;

    if (a[2] == 0 && a[4] == 0)
	/* Scaling */
	return ImagingScaleAffine(im, xsize, ysize, a, fill);

    imOut = ImagingNew(im->mode, xsize, ysize);
    if (!imOut)
	return NULL;

    ImagingCopyInfo(imOut, im);

    xo = a[0];
    yo = a[3];

#define	AFFINE_TRANSFORM(pixel, image)\
    for (y = 0; y < ysize; y++) {\
	pixel *out;\
	xx = xo;\
	yy = yo;\
	out = imOut->image[y];\
	for (x = 0; x < xsize; x++) {\
	    xin = (int) xx;\
	    yin = (int) yy;\
	    if (xin >= 0 && xin < (int) im->xsize &&\
		yin >= 0 && yin < (int) im->ysize)\
		out[x] = im->image[yin][xin];\
	    else if (fill)\
		out[x] = 0;\
	    xx += a[1];\
	    yy += a[4];\
	}\
	xo += a[2];\
	yo += a[5];\
    }

    if (im->image8)
	AFFINE_TRANSFORM(UINT8, image8)
    else
	AFFINE_TRANSFORM(INT32, image32)

    return imOut;
}


Imaging
ImagingTransform(Imaging im, unsigned xsize, unsigned ysize,
		 void (*transform)(double* X, double* Y,
				   int x, int y, void* data),
		 void* transform_data,
		 int fill)
{
    Imaging imOut;
    unsigned x, y;

    imOut = ImagingNew(im->mode, xsize, ysize);
    if (!imOut)
	return NULL;

    ImagingCopyInfo(imOut, im);

#define	TRANSFORM(pixel, image)\
    for (y = 0; y < ysize; y++) {\
	pixel *out;\
	out = imOut->image[y];\
	for (x = 0; x < xsize; x++) {\
	    double xx, yy;\
	    int xin, yin;\
	    (*transform)(&xx, &yy, x, y, transform_data);\
	    xin = (int) xx;\
	    yin = (int) yy;\
	    if (xin >= 0 && xin < (int) im->xsize &&\
		yin >= 0 && yin < (int) im->ysize)\
		out[x] = im->image[yin][xin];\
	    else if (fill)\
		out[x] = 0;\
	}\
    }

    if (im->image8)
	TRANSFORM(UINT8, image8)
    else
	TRANSFORM(INT32, image32)

    return imOut;
}


/* Convenience transforms */

Imaging
ImagingResize(Imaging im, unsigned xsize, unsigned ysize)
{
    double a[6];

    if (xsize == im->xsize && ysize == im->ysize)
	return ImagingCopy(im);

    a[0] = a[3] = 0;
    a[1] = (double) im->xsize / xsize;
    a[5] = (double) im->ysize / ysize;

    return ImagingScaleAffine(im, xsize, ysize, a, 1);
}


Imaging
ImagingRotate(Imaging im, double theta)
{
    unsigned xsize, ysize;
    double sintheta, costheta;
    double a[6];

    theta = fmod(theta, 360.0);
    if (theta < 0.0)
	theta += 360;

    /* Check if we can use a fast version */
    if (theta == 0.0)
	return ImagingCopy(im);
    else if (theta == 90.0)
	return ImagingRotate90(im);
    else if (theta == 180.0)
	return ImagingRotate180(im);
    else if (theta == 270.0)
	return ImagingRotate270(im);

    /* Setup an affine transform to rotate around the image center */
    theta = -theta * M_PI / 180.0;
    sintheta = sin(theta);
    costheta = cos(theta);

    xsize = im->xsize;
    ysize = im->ysize;

    a[0] = -costheta * xsize/2 - sintheta * ysize/2 + xsize/2;
    a[1] = costheta;
    a[2] = sintheta;
    a[3] = sintheta * xsize/2 - costheta * ysize/2 + ysize/2;
    a[4] = -sintheta;
    a[5] = costheta;

    return ImagingTransformAffine(im, xsize, ysize, a, 1);
}
