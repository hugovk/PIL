/* 
 * The Python Imaging Library
 * $Id: Convert.c,v 1.8 1997/01/14 16:40:04 fl Exp $
 * 
 * convert images
 *
 * to do:
 *	FIXME: Add more converters
 *
 * history:
 *	95-06-15 fl:	Created
 *	95-11-28 fl:	Added some "RGBA" and "CMYK" conversions
 *	96-04-22 fl:	Added "1" conversions (same as "L")
 *	96-05-05 fl:	Added palette conversions (hack)
 *	96-07-23 fl:	Fixed "1" conversions to zero/non-zero convention
 *	96-11-01 fl:	Fixed "P" to "L" and "RGB" to "1" conversions
 *	96-12-29 fl	Set alpha byte in RGB converters
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


#include "Imaging.h"


/* ------------------- */
/* 1 (bit) conversions */
/* ------------------- */

static void
bit2l(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++)
	out[x] = (in[x] != 0) ? 255 : 0;
}

static void
bit2rgb(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++, out += 4) {
	out[0] = out[1] = out[2] = (in[x] != 0) ? 255 : 0;
	out[3] = 255;
    }
}

static void
bit2cmyk(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++, out += 4)
	out[0] = out[1] = out[2] = 0, (in[x] != 0) ? 0 : 255;
}

/* ----------------- */
/* RGB/L conversions */
/* ----------------- */

static void
l2bit(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++)
	out[x] = (in[x] != 0) ? 255 : 0;
}

static void
l2rgb(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++, out += 4) {
	out[0] = out[1] = out[2] = in[x];
	out[3] = 255;
    }
}

static void
rgb2bit(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++, in += 4)
	/* ITU-R Recommendation 601-2 (assuming nonlinear RGB) */
	out[x] = (((INT32) in[0] * 299 +
		   (INT32) in[1] * 587 +
		   (INT32) in[2] * 114) >= 1000) ? 255 : 0;
}

static void
rgb2l(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++, in += 4)
	/* ITU-R Recommendation 601-2 (assuming nonlinear RGB) */
	out[x] = ((INT32) in[0] * 299 +
		  (INT32) in[1] * 587 +
		  (INT32) in[2] * 114) / 1000;
}


/* ---------------- */
/* RGBA conversions */
/* ---------------- */

static void
rgb2rgba(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++, in += 4, out += 4)
	out[0] = in[0], out[1] = in[1], out[2] = in[2], out[3] = 255;
}


static void
rgba2rgb(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++, in += 4, out += 4) {
	out[0] = in[0], out[1] = in[1], out[2] = in[2]; /* FIXME: memcpy? */
	out[3] = 255;
    }
}


/* ---------------- */
/* CMYK conversions */
/* ---------------- */

static void
l2cmyk(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++, out += 4)
	out[0] = out[1] = out[2] = 0, out[3] = ~in[x];
}

static void
rgb2cmyk(UINT8* out, const UINT8* in, int xsize)
{
    int i;
    for (i = 0; i < xsize; i++, in += 4, out += 4)
	/* Note: no undercolour removal */
	out[0] = ~in[0], out[1] = ~in[1], out[2] = ~in[2], out[3] = 0;
}

static void
cmyk2rgb(UINT8* out, const UINT8* in, int xsize)
{
    int i;
    for (i = 0; i < xsize; i++, in += 4, out += 4) {
	int temp = 255 - (in[0] + in[3]);
	out[0] = (temp <= 0) ? 0 : temp;
	temp = 255 - (in[1] + in[3]);
	out[1] = (temp <= 0) ? 0 : temp;
	temp = 255 - (in[2] + in[3]);
	out[2] = (temp <= 0) ? 0 : temp;
	out[3] = 255;
    }
}


/* ------------------- */
/* Palette conversions */
/* ------------------- */

static void
p2l(UINT8* out, const UINT8* in, int xsize, const UINT8* palette)
{
    int x;
    for (x = 0; x < xsize; x++)
	out[x] = palette[in[x]*4]; /* FIXME: map to RED member ???? */
}

static void
p2rgb(UINT8* out, const UINT8* in, int xsize, const UINT8* palette)
{
    int x;
    for (x = 0; x < xsize; x++, out += 4) {
	int v = in[x] * 4;
	out[0] = palette[v];
	out[1] = palette[v+1];
	out[2] = palette[v+2];
	out[3] = 255;
    }
}

static void
p2cmyk(UINT8* out, const UINT8* in, int xsize, const UINT8* palette)
{
    int x;
    for (x = 0; x < xsize; x++, out += 4) {
	int v = in[x] * 4;
	out[0] = palette[v];
	out[1] = palette[v+1];
	out[2] = palette[v+2];
	out[3] = palette[v+3];
    }
}

static struct {
    const char* from;
    const char* to;
    ImagingShuffler convert;
} converters[] = {
    { "1",    "L",    bit2l },
    { "1",    "RGB",  bit2rgb },
    { "1",    "RGBA", bit2rgb },
    { "1",    "CMYK", bit2cmyk },
    { "L",    "1",    l2bit },
    { "L",    "RGB",  l2rgb },
    { "L",    "RGBA", l2rgb },
    { "L",    "CMYK", l2cmyk },
    { "RGB",  "1",    rgb2bit },
    { "RGB",  "L",    rgb2l },
    { "RGB",  "RGBA", rgb2rgba },
    { "RGB",  "CMYK", rgb2cmyk },
    { "RGBA", "1",    rgb2bit },
    { "RGBA", "L",    rgb2l },
    { "RGBA", "RGB",  rgba2rgb },
    { "RGBA", "CMYK", rgb2cmyk },
    { "CMYK", "RGB",  cmyk2rgb },
    { "CMYK", "RGBA", cmyk2rgb },
    { NULL }
};


Imaging
ImagingConvert(Imaging im, const char *mode)
{
    Imaging imOut;
    unsigned x, y;
    ImagingShuffler convert;

    if (!im)
	return (Imaging) ImagingError_Argument(NULL);

    if (!mode) {
	/* Map palette image to full depth */
	if (!im->palette)
	    return (Imaging) ImagingError_Argument("Not a palette image");
	mode = im->palette->mode;
    } else
	/* Same mode? */
	if (!strcmp(im->mode, mode))
	    return ImagingCopy(im);

    convert = NULL;

    if (strcmp(im->mode, "P") == 0) {

	/* Map palette to L, RGB, or CMYK */

	void (*convert)(UINT8*, const UINT8*, int, const UINT8*);

	if (!im->palette)
	    return (Imaging) ImagingError_Argument("No palette");

	if (strcmp(mode, "L") == 0)
	    convert = p2l;
	else if (strcmp(mode, "RGB") == 0)
	    convert = p2rgb;
	else if (strcmp(mode, "RGBA") == 0)
	    convert = p2rgb;
	else if (strcmp(mode, "CMYK") == 0)
	    convert = p2cmyk;
	else
	    return (Imaging) ImagingError_Argument("Illegal conversion");

	imOut = ImagingNew(mode, im->xsize, im->ysize);
	if (!imOut)
	    return NULL;

	for (y = 0; y < im->ysize; y++)
	    (*convert)((UINT8*) imOut->image[y], (UINT8*) im->image[y],
		       im->xsize, im->palette->palette);

	return imOut;

    } else if (strcmp(im->mode, "RGB") == 0 && strcmp(mode, "P") == 0) {

	/* Map RGB to palette */

	if (!im->palette)
	    return (Imaging) ImagingError_Argument("No palette");

	if (ImagingPaletteCachePrepare(im->palette) < 0)
	    return NULL;

	imOut = ImagingNew("P", im->xsize, im->ysize);
	if (!imOut)
	    return NULL;

	for (y = 0; y < im->ysize; y++) {
	    UINT8* in  = (UINT8*) im->image[y];
	    UINT8* out = imOut->image8[y];
	    for (x = 0; x < im->xsize; x++, in+=4) {
		INT16* cache;
		cache = &ImagingPaletteCache(im->palette,
					     in[0], in[1], in[2]);
		if (cache[0] == 256)
		    ImagingPaletteCacheUpdate(im->palette,
					      in[0], in[1], in[2]);
		out[x] = cache[0];
	    }
	}

	ImagingPaletteCacheDelete(im->palette);

	return imOut;

    }

    /* Convert between colour spaces RGB to palette */

    for (y = 0; converters[y].from; y++)
	if (!strcmp(im->mode, converters[y].from) &&
	    !strcmp(mode, converters[y].to)) {
	    convert = converters[y].convert;
	    break;
	}

    if (!convert)
	return (Imaging) ImagingError_Argument("Illegal conversion");

    imOut = ImagingNew(mode, im->xsize, im->ysize);
    if (!imOut)
	return NULL;

    for (y = 0; y < im->ysize; y++)
	(*convert)((UINT8*) imOut->image[y], (UINT8*) im->image[y],
		   im->xsize);

    return imOut;
}
