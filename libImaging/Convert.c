/* 
 * The Python Imaging Library
 * $Id: Convert.c,v 1.6 1996/08/15 13:12:44 fredrik Exp $
 * 
 * convert images
 *
 * history:
 *	95-06-15 fl:	Created
 *	95-11-28 fl:	Added some "RGBA" and "CMYK" conversions
 *	96-04-22 fl:	Added "1" conversions (same as "L")
 *	96-05-05 fl:	Added palette conversions (hack)
 *	96-07-23 fl:	Fixed "1" conversions to zero/non-zero convention
 *	96-11-01 fl:	Fixed "P" to "L" and "RGB" to "1" conversions
 *	96-12-29 fl:	Set alpha byte in RGB converters
 *	97-05-12 fl:	Added ImagingConvert2
 *	97-05-30 fl:	Added floating point support
 *	97-08-27 fl:	Added "P" to "1" and "P" to "F" conversions
 *	98-01-11 fl:	Added integer support
 *
 * Copyright (c) Fredrik Lundh 1995-97.
 * Copyright (c) Secret Labs AB 1997-98.
 *
 * See the README file for details on usage and redistribution.
 */


#include "Imaging.h"

/* ITU-R Recommendation 601-2 (assuming nonlinear RGB) */
#define L(rgb)\
    ((INT32) (rgb)[0] * 299 + (INT32) (rgb)[1] * 587 + (INT32) (rgb)[2] * 114)

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
	out[x] = (L(in) >= 1000) ? 255 : 0;
}

static void
rgb2l(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++, in += 4)
	/* ITU-R Recommendation 601-2 (assuming nonlinear RGB) */
	out[x] = L(in) / 1000;
}

static void
rgb2i(UINT8* out_, const UINT8* in, int xsize)
{
    int x;
    INT32* out = (INT32*) out_;
    for (x = 0; x < xsize; x++, in += 4)
	out[x] = L(in) / 1000;
}

static void
rgb2f(UINT8* out_, const UINT8* in, int xsize)
{
    int x;
    FLOAT32* out = (FLOAT32*) out_;
    for (x = 0; x < xsize; x++, in += 4)
	out[x] = L(in) / 1000.0F;
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

/* ------------- */
/* I conversions */
/* ------------- */

static void
bit2i(UINT8* out_, const UINT8* in, int xsize)
{
    int x;
    INT32* out = (INT32*) out_;
    for (x = 0; x < xsize; x++)
	out[x] = (in[x] != 0) ? 255 : 0;
}

static void
l2i(UINT8* out_, const UINT8* in, int xsize)
{
    int x;
    INT32* out = (INT32*) out_;
    for (x = 0; x < xsize; x++)
	out[x] = (INT32) in[x];
}

static void
i2l(UINT8* out, const UINT8* in_, int xsize)
{
    int x;
    INT32* in = (INT32*) in_;
    for (x = 0; x < xsize; x++) {
        if (in[x] <= 0)
            out[x] = 0;
        else if (in[x] >= 255)
            out[x] = 255;
        else
            out[x] = (UINT8) in[x];
    }
}

static void
i2f(UINT8* out_, const UINT8* in_, int xsize)
{
    int x;
    INT32* in = (INT32*) in_;
    FLOAT32* out = (FLOAT32*) out_;
    for (x = 0; x < xsize; x++)
	out[x] = (FLOAT32) in[x];
}

/* ------------- */
/* F conversions */
/* ------------- */

static void
bit2f(UINT8* out_, const UINT8* in, int xsize)
{
    int x;
    FLOAT32* out = (FLOAT32*) out_;
    for (x = 0; x < xsize; x++)
	out[x] = (in[x] != 0) ? 255.0 : 0.0;
}

static void
l2f(UINT8* out_, const UINT8* in, int xsize)
{
    int x;
    FLOAT32* out = (FLOAT32*) out_;
    for (x = 0; x < xsize; x++)
	out[x] = (FLOAT32) in[x];
}

static void
f2l(UINT8* out, const UINT8* in_, int xsize)
{
    int x;
    FLOAT32* in = (FLOAT32*) in_;
    for (x = 0; x < xsize; x++) {
        if (in[x] <= 0.0)
            out[x] = 0;
        else if (in[x] >= 255.0)
            out[x] = 255;
        else
            out[x] = (UINT8) in[x];
    }
}

static void
f2i(UINT8* out_, const UINT8* in_, int xsize)
{
    int x;
    FLOAT32* in = (FLOAT32*) in_;
    INT32* out = (INT32*) out_;
    for (x = 0; x < xsize; x++)
	out[x] = (INT32) in[x];
}


static struct {
    const char* from;
    const char* to;
    ImagingShuffler convert;
} converters[] = {
    { "1",    "L",    bit2l },
    { "1",    "I",    bit2i },
    { "1",    "F",    bit2f },
    { "1",    "RGB",  bit2rgb },
    { "1",    "RGBA", bit2rgb },
    { "1",    "CMYK", bit2cmyk },

    { "L",    "1",    l2bit },
    { "L",    "I",    l2i },
    { "L",    "F",    l2f },
    { "L",    "RGB",  l2rgb },
    { "L",    "RGBA", l2rgb },
    { "L",    "CMYK", l2cmyk },

    { "I",    "L",    i2l },
    { "I",    "F",    i2f },

    { "F",    "L",    f2l },
    { "F",    "I",    f2i },

    { "RGB",  "1",    rgb2bit },
    { "RGB",  "L",    rgb2l },
    { "RGB",  "I",    rgb2i },
    { "RGB",  "F",    rgb2f },
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


/* ------------------- */
/* Palette conversions */
/* ------------------- */

static void
p2bit(UINT8* out, const UINT8* in, int xsize, const UINT8* palette)
{
    int x;
    /* FIXME: precalculate greyscale palette? */
    for (x = 0; x < xsize; x++)
	out[x] = (L(&palette[in[x]*4]) >= 1000) ? 255 : 0;
}

static void
p2l(UINT8* out, const UINT8* in, int xsize, const UINT8* palette)
{
    int x;
    /* FIXME: precalculate greyscale palette? */
    for (x = 0; x < xsize; x++)
	out[x] = L(&palette[in[x]*4]) / 1000;
}

static void
p2i(UINT8* out_, const UINT8* in, int xsize, const UINT8* palette)
{
    int x;
    INT32* out = (INT32*) out_;
    for (x = 0; x < xsize; x++)
	out[x] = L(&palette[in[x]*4]) / 1000;
}

static void
p2f(UINT8* out_, const UINT8* in, int xsize, const UINT8* palette)
{
    int x;
    FLOAT32* out = (FLOAT32*) out_;
    for (x = 0; x < xsize; x++)
	out[x] = L(&palette[in[x]*4]) / 1000.0F;
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
p2rgba(UINT8* out, const UINT8* in, int xsize, const UINT8* palette)
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

static Imaging
frompalette(Imaging imOut, Imaging imIn, const char *mode)
{
    int y;
    void (*convert)(UINT8*, const UINT8*, int, const UINT8*);

    /* Map palette image to L, RGB, RGBA, or CMYK */

    if (!imIn->palette)
	return (Imaging) ImagingError_Argument("No palette");

    if (strcmp(mode, "1") == 0)
	convert = p2bit;
    else if (strcmp(mode, "L") == 0)
	convert = p2l;
    else if (strcmp(mode, "I") == 0)
	convert = p2i;
    else if (strcmp(mode, "F") == 0)
	convert = p2f;
    else if (strcmp(mode, "RGB") == 0)
	convert = p2rgb;
    else if (strcmp(mode, "RGBA") == 0)
	convert = p2rgba;
    else if (strcmp(mode, "CMYK") == 0)
	convert = p2rgba;
    else
	return (Imaging) ImagingError_Argument("Illegal conversion");

    imOut = ImagingNew2(mode, imOut, imIn);
    if (!imOut)
        return NULL;

    for (y = 0; y < imIn->ysize; y++)
	(*convert)((UINT8*) imOut->image[y], (UINT8*) imIn->image[y],
		   imIn->xsize, imIn->palette->palette);

    return imOut;
}

static Imaging
topalette(Imaging imOut, Imaging imIn, ImagingPalette palette)
{
    int x, y;

    /* Map L or RGB to palette image */

    if (strcmp(imIn->mode, "L") != 0 && strcmp(imIn->mode, "RGB") != 0)
	return (Imaging) ImagingError_Argument("Illegal conversion");
	
    if (imIn->bands == 1)
	palette = ImagingPaletteNew("RGB"); /* Initialised to grey ramp */
    else
	palette = ImagingPaletteNewBrowser(); /* Standard colour cube */

    if (!palette)
	return (Imaging) ImagingError_Argument("No palette");

    imOut = ImagingNew2("P", imOut, imIn);
    if (!imOut) {
	ImagingPaletteDelete(palette);
        return NULL;
    }

    imOut->palette = ImagingPaletteDuplicate(palette);

    if (imIn->bands == 1) {

	/* Greyscale image */

	/* Copy as is */
	for (y = 0; y < imIn->ysize; y++)
	    memcpy(imOut->image[y], imIn->image[y], imIn->linesize);

    } else {

	/* True colour image */

	/* Create mapping cache */
	if (ImagingPaletteCachePrepare(palette) < 0) {
	    ImagingDelete(imOut);
	    ImagingPaletteDelete(palette);
	    return NULL;
	}

	/* Map each pixel to the nearest palette entry */
	for (y = 0; y < imIn->ysize; y++) {
	    UINT8* in  = (UINT8*) imIn->image[y];
	    UINT8* out = imOut->image8[y];
	    for (x = 0; x < imIn->xsize; x++, in+=4) {
		INT16* cache;
		cache = &ImagingPaletteCache(palette, in[0], in[1], in[2]);
		if (cache[0] == 0x100)
		    ImagingPaletteCacheUpdate(palette, in[0], in[1], in[2]);
		out[x] = cache[0];
	    }
	}

	ImagingPaletteCacheDelete(palette);

    }

    ImagingPaletteDelete(palette);

    return imOut;
}


Imaging
_convert(Imaging imOut, Imaging imIn, const char *mode, ImagingPalette palette)
{
    int y;
    ImagingShuffler convert;

    if (!imIn)
	return (Imaging) ImagingError_Argument(NULL);

    if (!mode) {
	/* Map palette image to full depth */
	if (!imIn->palette)
	    return (Imaging) ImagingError_Argument("Not a palette image");
	mode = imIn->palette->mode;
    } else
	/* Same mode? */
	if (!strcmp(imIn->mode, mode))
	    return ImagingCopy2(imOut, imIn);

    /* palette conversions are special */

    if (strcmp(imIn->mode, "P") == 0)
	return frompalette(imOut, imIn, mode);

    if (strcmp(mode, "P") == 0)
	return topalette(imOut, imIn, NULL);

    /* Convert between colour spaces RGB to palette */

    convert = NULL;

    for (y = 0; converters[y].from; y++)
	if (!strcmp(imIn->mode, converters[y].from) &&
	    !strcmp(mode, converters[y].to)) {
	    convert = converters[y].convert;
	    break;
	}

    if (!convert)
	return (Imaging) ImagingError_Argument("Illegal conversion");

    imOut = ImagingNew2(mode, imOut, imIn);
    if (!imOut)
        return NULL;

    for (y = 0; y < imIn->ysize; y++)
	(*convert)((UINT8*) imOut->image[y], (UINT8*) imIn->image[y],
		   imIn->xsize);

    return imOut;
}

Imaging
ImagingConvert(Imaging imIn, const char *mode, ImagingPalette palette)
{
    return _convert(NULL, imIn, mode, palette);
}

Imaging
ImagingConvert2(Imaging imOut, Imaging imIn)
{
    return _convert(imOut, imIn, imOut->mode, NULL);
}
