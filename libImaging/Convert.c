/* 
 * The Python Imaging Library
 * $Id$
 * 
 * convert images
 *
 * history:
 * 95-06-15 fl	created
 * 95-11-28 fl	added some "RGBA" and "CMYK" conversions
 * 96-04-22 fl	added "1" conversions (same as "L")
 * 96-05-05 fl	added palette conversions (hack)
 * 96-07-23 fl	fixed "1" conversions to zero/non-zero convention
 * 96-11-01 fl	fixed "P" to "L" and "RGB" to "1" conversions
 * 96-12-29 fl	set alpha byte in RGB converters
 * 97-05-12 fl	added ImagingConvert2
 * 97-05-30 fl	added floating point support
 * 97-08-27 fl	added "P" to "1" and "P" to "F" conversions
 * 98-01-11 fl	added integer support
 * 98-07-01 fl	added "YCbCr" support
 * 98-07-02 fl	added "RGBX" conversions (sort of)
 * 98-07-04 fl	added floyd-steinberg dithering
 * 98-07-12 fl	changed "YCrCb" to "YCbCr" (!)
 * 98-12-29 fl	added basic "I;16" and "I;16B" conversions
 * 99-02-03 fl	added "RGBa", and "BGR" conversions (experimental)
 *
 * Copyright (c) Secret Labs AB 1997-99.
 * Copyright (c) Fredrik Lundh 1995-97.
 *
 * See the README file for details on usage and redistribution.
 */


#include "Imaging.h"

#define CLIP(v) ((v) <= 0 ? 0 : (v) >= 255 ? 255 : (v))
#define CLIP16(v) ((v) <= -32768 ? -32768 : (v) >= 32767 ? 32767 : (v))

/* like (a * b + 127) / 255), but much faster on most platforms */
#define	MULDIV255(a, b, tmp)\
     	(tmp = (a) * (b) + 128, ((((tmp) >> 8) + (tmp)) >> 8))

/* ITU-R Recommendation 601-2 (assuming nonlinear RGB) */
#define L(rgb)\
    ((INT32) (rgb)[0]*299 + (INT32) (rgb)[1]*587 + (INT32) (rgb)[2]*114)

/* ------------------- */
/* 1 (bit) conversions */
/* ------------------- */

static void
bit2l(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++)
	*out++ = (*in++ != 0) ? 255 : 0;
}

static void
bit2rgb(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++) {
        UINT8 v = (*in++ != 0) ? 255 : 0;
	*out++ = v;
	*out++ = v;
	*out++ = v;
	*out++ = 255;
    }
}

static void
bit2cmyk(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++) {
	*out++ = 0;
	*out++ = 0;
	*out++ = 0;
	*out++ = (*in++ != 0) ? 0 : 255;
    }
}

static void
bit2ycbcr(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++) {
	*out++ = (*in++ != 0) ? 255 : 0;
        *out++ = 128;
        *out++ = 128;
        *out++ = 255;
    }
}

/* ----------------- */
/* RGB/L conversions */
/* ----------------- */

static void
l2bit(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++)
	*out++ = (*in++ >= 128) ? 255 : 0;
}

static void
l2rgb(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++) {
        UINT8 v = *in++;
	*out++ = v;
	*out++ = v;
	*out++ = v;
	*out++ = 255;
    }
}

static void
rgb2bit(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++, in += 4)
	/* ITU-R Recommendation 601-2 (assuming nonlinear RGB) */
	*out++ = (L(in) >= 128000) ? 255 : 0;
}

static void
rgb2l(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++, in += 4)
	/* ITU-R Recommendation 601-2 (assuming nonlinear RGB) */
	*out++ = L(in) / 1000;
}

static void
rgb2i(UINT8* out_, const UINT8* in, int xsize)
{
    int x;
    INT32* out = (INT32*) out_;
    for (x = 0; x < xsize; x++, in += 4)
	*out++ = L(in) / 1000;
}

static void
rgb2f(UINT8* out_, const UINT8* in, int xsize)
{
    int x;
    FLOAT32* out = (FLOAT32*) out_;
    for (x = 0; x < xsize; x++, in += 4)
	*out++ = L(in) / 1000.0F;
}

static void
rgb2bgr15(UINT8* out_, const UINT8* in, int xsize)
{
    int x;
    UINT16* out = (UINT16*) out_;
    for (x = 0; x < xsize; x++, in += 4)
	*out++ =
            ((((UINT16)in[0])<<8)&0x7c00) +
            ((((UINT16)in[1])<<2)&0x03e0) +
            ((((UINT16)in[2])>>3)&0x001f);
}

static void
rgb2bgr16(UINT8* out_, const UINT8* in, int xsize)
{
    int x;
    UINT16* out = (UINT16*) out_;
    for (x = 0; x < xsize; x++, in += 4)
	*out++ =
            ((((UINT16)in[0])<<8)&0xf800) +
            ((((UINT16)in[1])<<3)&0x07e0) +
            ((((UINT16)in[2])>>3)&0x001f);
}

static void
rgb2bgr24(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++, in += 4) {
	*out++ = in[2];
	*out++ = in[1];
	*out++ = in[0];
    }
}

/* ---------------- */
/* RGBA conversions */
/* ---------------- */

static void
rgb2rgba(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = 255; in++;
    }
}

static void
rgba2rgb(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = 255; in++;
    }
}

static void
rgba2rgba(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    unsigned int alpha, tmp;
    for (x = 0; x < xsize; x++) {
        alpha = in[3];
        *out++ = MULDIV255(*in++, alpha, tmp);
        *out++ = MULDIV255(*in++, alpha, tmp);
        *out++ = MULDIV255(*in++, alpha, tmp);
        *out++ = *in++;
    }
}

/* ---------------- */
/* CMYK conversions */
/* ---------------- */

static void
l2cmyk(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++) {
        *out++ = 0;
        *out++ = 0;
        *out++ = 0;
        *out++ = ~(*in++);
    }
}

static void
rgb2cmyk(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++) {
	/* Note: no undercolour removal */
        *out++ = ~(*in++);
        *out++ = ~(*in++);
        *out++ = ~(*in++);
        *out++ = 0; in++;
    }
}

static void
cmyk2rgb(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++, in += 4) {
        *out++ = CLIP(255 - (in[0] + in[3]));
	*out++ = CLIP(255 - (in[1] + in[3]));
	*out++ = CLIP(255 - (in[2] + in[3]));
	*out++ = 255;
    }
}

/* FIXME: translate indexed versions to pointer versions below this line */

/* ------------- */
/* I conversions */
/* ------------- */

static void
bit2i(UINT8* out_, const UINT8* in, int xsize)
{
    int x;
    INT32* out = (INT32*) out_;
    for (x = 0; x < xsize; x++)
	*out++ = (*in++ != 0) ? 255 : 0;
}

static void
l2i(UINT8* out_, const UINT8* in, int xsize)
{
    int x;
    INT32* out = (INT32*) out_;
    for (x = 0; x < xsize; x++)
	*out++ = (INT32) *in++;
}

static void
i2l(UINT8* out, const UINT8* in_, int xsize)
{
    int x;
    INT32* in = (INT32*) in_;
    for (x = 0; x < xsize; x++) {
        if (in[x] <= 0)
            *out++ = 0;
        else if (in[x] >= 255)
            *out++ = 255;
        else
            *out++ = (UINT8) in[x];
    }
}

static void
i2f(UINT8* out_, const UINT8* in_, int xsize)
{
    int x;
    INT32* in = (INT32*) in_;
    FLOAT32* out = (FLOAT32*) out_;
    for (x = 0; x < xsize; x++)
	*out++ = (FLOAT32) *in++;
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
	*out++ = (*in++ != 0) ? 255.0 : 0.0;
}

static void
l2f(UINT8* out_, const UINT8* in, int xsize)
{
    int x;
    FLOAT32* out = (FLOAT32*) out_;
    for (x = 0; x < xsize; x++)
	*out++ = (FLOAT32) *in++;
}

static void
f2l(UINT8* out, const UINT8* in_, int xsize)
{
    int x;
    FLOAT32* in = (FLOAT32*) in_;
    for (x = 0; x < xsize; x++) {
        if (in[x] <= 0.0)
            *out++ = 0;
        else if (in[x] >= 255.0)
            *out++ = 255;
        else
            *out++ = (UINT8) in[x];
    }
}

static void
f2i(UINT8* out_, const UINT8* in_, int xsize)
{
    int x;
    FLOAT32* in = (FLOAT32*) in_;
    INT32* out = (INT32*) out_;
    for (x = 0; x < xsize; x++)
	*out++ = (INT32) *in++;
}

/* ----------------- */
/* YCbCr conversions */
/* ----------------- */

/* See ConvertYCbCr.c for RGB/YCbCr tables */

static void
l2ycbcr(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++) {
	*out++ = *in++;
	*out++ = 128;
	*out++ = 128;
	*out++ = 255;
    }
}

static void
ycbcr2l(UINT8* out, const UINT8* in, int xsize)
{
    int x;
    for (x = 0; x < xsize; x++, in += 4)
	*out++ = in[0];
}

/* ------------------------- */
/* I;16 (16-bit) conversions */
/* ------------------------- */

static void
i2i16(UINT8* out, const UINT8* in_, int xsize)
{
    int x, v;
    INT32* in = (INT32*) in_;
    for (x = 0; x < xsize; x++) {
        v = CLIP16(in[x]);
	*out++ = (UINT8) v;
        *out++ = (UINT8) (v >> 8);
    }
}

static void
i2i16b(UINT8* out, const UINT8* in_, int xsize)
{
    int x, v;
    INT32* in = (INT32*) in_;
    for (x = 0; x < xsize; x++) {
        v = CLIP16(in[x]);
        *out++ = (UINT8) (v >> 8);
	*out++ = (UINT8) v;
    }
}

static void
i162i(UINT8* out_, const UINT8* in, int xsize)
{
    int x;
    INT32* out = (INT32*) out_;
    for (x = 0; x < xsize; x++)
	*out++ = in[x+x] + ((int) in[x+x+1] << 8);
}

static void
i16b2i(UINT8* out_, const UINT8* in, int xsize)
{
    int x;
    INT32* out = (INT32*) out_;
    for (x = 0; x < xsize; x++)
	*out++ = ((int) in[x+x] << 8) + in[x+x+1];
}


static struct {
    const char* from;
    const char* to;
    ImagingShuffler convert;
} converters[] = {

    { "1", "L", bit2l },
    { "1", "I", bit2i },
    { "1", "F", bit2f },
    { "1", "RGB", bit2rgb },
    { "1", "RGBA", bit2rgb },
    { "1", "RGBX", bit2rgb },
    { "1", "CMYK", bit2cmyk },
    { "1", "YCbCr", bit2ycbcr },

    { "L", "1", l2bit },
    { "L", "I", l2i },
    { "L", "F", l2f },
    { "L", "RGB", l2rgb },
    { "L", "RGBA", l2rgb },
    { "L", "RGBX", l2rgb },
    { "L", "CMYK", l2cmyk },
    { "L", "YCbCr", l2ycbcr },

    { "I",    "L",    i2l },
    { "I",    "F",    i2f },

    { "F",    "L",    f2l },
    { "F",    "I",    f2i },

    { "RGB", "1", rgb2bit },
    { "RGB", "L", rgb2l },
    { "RGB", "I", rgb2i },
    { "RGB", "F", rgb2f },
    { "RGB", "BGR;15", rgb2bgr15 },
    { "RGB", "BGR;16", rgb2bgr16 },
    { "RGB", "BGR;24", rgb2bgr24 },
    { "RGB", "RGBA", rgb2rgba },
    { "RGB", "RGBX", rgb2rgba },
    { "RGB", "CMYK", rgb2cmyk },
    { "RGB", "YCbCr", ImagingConvertRGB2YCbCr },

    { "RGBA", "1", rgb2bit },
    { "RGBA", "L", rgb2l },
    { "RGBA", "I", rgb2i },
    { "RGBA", "F", rgb2f },
    { "RGBA", "RGB", rgba2rgb },
    { "RGBA", "RGBa", rgba2rgba },
    { "RGBA", "RGBX", rgb2rgba },
    { "RGBA", "CMYK", rgb2cmyk },
    { "RGBA", "YCbCr", ImagingConvertRGB2YCbCr },

    { "RGBX", "1", rgb2bit },
    { "RGBX", "L", rgb2l },
    { "RGBA", "I", rgb2i },
    { "RGBA", "F", rgb2f },
    { "RGBX", "RGB", rgba2rgb },
    { "RGBX", "CMYK", rgb2cmyk },
    { "RGBX", "YCbCr", ImagingConvertRGB2YCbCr },

    { "CMYK", "RGB",  cmyk2rgb },
    { "CMYK", "RGBA", cmyk2rgb },
    { "CMYK", "RGBX", cmyk2rgb },

    { "YCbCr", "L", ycbcr2l },
    { "YCbCr", "RGB", ImagingConvertYCbCr2RGB },

    { "I", "I;16", i2i16 },
    { "I;16", "I", i162i },
    { "I", "I;16B", i2i16b },
    { "I;16B", "I", i16b2i },

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
	*out++ = (L(&palette[in[x]*4]) >= 1000) ? 255 : 0;
}

static void
p2l(UINT8* out, const UINT8* in, int xsize, const UINT8* palette)
{
    int x;
    /* FIXME: precalculate greyscale palette? */
    for (x = 0; x < xsize; x++)
	*out++ = L(&palette[in[x]*4]) / 1000;
}

static void
p2i(UINT8* out_, const UINT8* in, int xsize, const UINT8* palette)
{
    int x;
    INT32* out = (INT32*) out_;
    for (x = 0; x < xsize; x++)
	*out++ = L(&palette[in[x]*4]) / 1000;
}

static void
p2f(UINT8* out_, const UINT8* in, int xsize, const UINT8* palette)
{
    int x;
    FLOAT32* out = (FLOAT32*) out_;
    for (x = 0; x < xsize; x++)
	*out++ = L(&palette[in[x]*4]) / 1000.0F;
}

static void
p2rgb(UINT8* out, const UINT8* in, int xsize, const UINT8* palette)
{
    int x;
    for (x = 0; x < xsize; x++) {
	const UINT8* rgb = &palette[*in++ * 4];
	*out++ = rgb[0];
	*out++ = rgb[1];
	*out++ = rgb[2];
	*out++ = 255;
    }
}

static void
p2rgba(UINT8* out, const UINT8* in, int xsize, const UINT8* palette)
{
    int x;
    for (x = 0; x < xsize; x++) {
	const UINT8* rgba = &palette[*in++ * 4];
	*out++ = rgba[0];
	*out++ = rgba[1];
	*out++ = rgba[2];
	*out++ = rgba[3];
    }
}

static void
p2cmyk(UINT8* out, const UINT8* in, int xsize, const UINT8* palette)
{
    p2rgb(out, in, xsize, palette);
    rgb2cmyk(out, out, xsize);
}

static void
p2ycbcr(UINT8* out, const UINT8* in, int xsize, const UINT8* palette)
{
    p2rgb(out, in, xsize, palette);
    ImagingConvertRGB2YCbCr(out, out, xsize);
}

static Imaging
frompalette(Imaging imOut, Imaging imIn, const char *mode)
{
    int y;
    void (*convert)(UINT8*, const UINT8*, int, const UINT8*);

    /* Map palette image to L, RGB, RGBA, or CMYK */

    if (!imIn->palette)
	return (Imaging) ImagingError_ValueError("no palette");

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
    else if (strcmp(mode, "RGBX") == 0)
	convert = p2rgba;
    else if (strcmp(mode, "CMYK") == 0)
	convert = p2cmyk;
    else if (strcmp(mode, "YCbCr") == 0)
	convert = p2ycbcr;
    else
	return (Imaging) ImagingError_ValueError("conversion not supported");

    imOut = ImagingNew2(mode, imOut, imIn);
    if (!imOut)
        return NULL;

    for (y = 0; y < imIn->ysize; y++)
	(*convert)((UINT8*) imOut->image[y], (UINT8*) imIn->image[y],
		   imIn->xsize, imIn->palette->palette);

    return imOut;
}

static Imaging
topalette(Imaging imOut, Imaging imIn, ImagingPalette palette, int dither)
{
    int x, y;

    /* Map L or RGB/RGBX/RGBA to palette image */
    if (strcmp(imIn->mode, "L") != 0 && strncmp(imIn->mode, "RGB", 3) != 0)
	return (Imaging) ImagingError_ValueError("conversion not supported");

    /* FIXME: make user configurable */
    if (imIn->bands == 1)
	palette = ImagingPaletteNew("RGB"); /* Initialised to grey ramp */
    else
	palette = ImagingPaletteNewBrowser(); /* Standard colour cube */

    if (!palette)
	return (Imaging) ImagingError_ValueError("no palette");

    imOut = ImagingNew2("P", imOut, imIn);
    if (!imOut) {
	ImagingPaletteDelete(palette);
        return NULL;
    }

    imOut->palette = ImagingPaletteDuplicate(palette);

    if (imIn->bands == 1) {
	/* greyscale image */

	/* Greyscale palette: copy data as is */
	for (y = 0; y < imIn->ysize; y++)
	    memcpy(imOut->image[y], imIn->image[y], imIn->linesize);

    } else {
	/* colour image */

	/* Create mapping cache */
	if (ImagingPaletteCachePrepare(palette) < 0) {
	    ImagingDelete(imOut);
	    ImagingPaletteDelete(palette);
	    return NULL;
	}

        if (dither) {
            /* floyd-steinberg dither */

            int* errors;
            errors = calloc(imIn->xsize + 1, sizeof(int) * 3);
            if (!errors) {
                ImagingDelete(imOut);
                ImagingError_MemoryError();
                return NULL;
            }

            /* Map each pixel to the nearest palette entry */
            for (y = 0; y < imIn->ysize; y++) {
                int r, r0, r1, r2;
                int g, g0, g1, g2;
                int b, b0, b1, b2;
                UINT8* in  = (UINT8*) imIn->image[y];
                UINT8* out = imOut->image8[y];
                int* e = errors;

                r = r0 = r1 = 0;
                g = g0 = g1 = 0;
                b = b0 = b1 = 0;

                for (x = 0; x < imIn->xsize; x++, in += 4) {
                    int d2;
                    INT16* cache;

                    r = CLIP(in[0] + (r + e[3+0])/16);
                    g = CLIP(in[1] + (g + e[3+1])/16);
                    b = CLIP(in[2] + (b + e[3+2])/16);

                    /* get closest colour */
                    cache = &ImagingPaletteCache(palette, r, g, b);
                    if (cache[0] == 0x100)
                        ImagingPaletteCacheUpdate(palette, r, g, b);
                    out[x] = cache[0];

                    r -= (int) palette->palette[cache[0]*4];
                    g -= (int) palette->palette[cache[0]*4+1];
                    b -= (int) palette->palette[cache[0]*4+2];

                    /* propagate errors (don't ask ;-)*/
                    r2 = r; d2 = r + r; r += d2; e[0] = r + r0;
                    r += d2; r0 = r + r1; r1 = r2; r += d2;
                    g2 = g; d2 = g + g; g += d2; e[1] = g + g0;
                    g += d2; g0 = g + g1; g1 = g2; g += d2;
                    b2 = b; d2 = b + b; b += d2; e[2] = b + b0;
                    b += d2; b0 = b + b1; b1 = b2; b += d2;

                    e += 3;

                }

                e[0] = b0;
                e[1] = b1;
                e[2] = b2;

            }
            free(errors);

        } else {

            /* closest colour */
            for (y = 0; y < imIn->ysize; y++) {
                int r, g, b;
                UINT8* in  = (UINT8*) imIn->image[y];
                UINT8* out = imOut->image8[y];

                for (x = 0; x < imIn->xsize; x++, in += 4) {
                    INT16* cache;

                    r = in[0]; g = in[1]; b = in[2];

                    /* get closest colour */
                    cache = &ImagingPaletteCache(palette, r, g, b);
                    if (cache[0] == 0x100)
                        ImagingPaletteCacheUpdate(palette, r, g, b);
                    out[x] = cache[0];

                }
            }
        }
	ImagingPaletteCacheDelete(palette);
    }

    ImagingPaletteDelete(palette);

    return imOut;
}

static Imaging
tobilevel(Imaging imOut, Imaging imIn, int dither)
{
    int x, y;
    int* errors;

    /* Map L or RGB to dithered 1 image */
    if (strcmp(imIn->mode, "L") != 0 && strcmp(imIn->mode, "RGB") != 0)
	return (Imaging) ImagingError_ValueError("conversion not supported");

    imOut = ImagingNew2("1", imOut, imIn);
    if (!imOut)
        return NULL;

    errors = calloc(imIn->xsize + 1, sizeof(int));
    if (!errors) {
        ImagingDelete(imOut);
        ImagingError_MemoryError();
        return NULL;
    }

    if (imIn->bands == 1) {

        /* map each pixel to black or white, using error diffusion */
        for (y = 0; y < imIn->ysize; y++) {
            int l, l0, l1, l2, d2;
            UINT8* in  = (UINT8*) imIn->image[y];
            UINT8* out = imOut->image8[y];

            l = l0 = l1 = 0;

            for (x = 0; x < imIn->xsize; x++) {

                /* pick closest colour */
                l = CLIP(in[x] + (l + errors[x+1])/16);
                out[x] = (l > 128) ? 255 : 0;

                /* propagate errors */
                l -= (int) out[x];
                l2 = l; d2 = l + l; l += d2; errors[x] = l + l0;
                l += d2; l0 = l + l1; l1 = l2; l += d2;
            }

            errors[x] = l0;

        }

    } else {

        /* map each pixel to black or white, using error diffusion */
        for (y = 0; y < imIn->ysize; y++) {
            int l, l0, l1, l2, d2;
            UINT8* in  = (UINT8*) imIn->image[y];
            UINT8* out = imOut->image8[y];

            l = l0 = l1 = 0;

            for (x = 0; x < imIn->xsize; x++, in += 4) {

                /* pick closest colour */
                l = CLIP(L(in)/1000 + (l + errors[x+1])/16);
                out[x] = (l > 128) ? 255 : 0;

                /* propagate errors */
                l -= (int) out[x];
                l2 = l; d2 = l + l; l += d2; errors[x] = l + l0;
                l += d2; l0 = l + l1; l1 = l2; l += d2;

            }

            errors[x] = l0;

        }
    }

    free(errors);

    return imOut;
}


static Imaging
convert(Imaging imOut, Imaging imIn, const char *mode,
        ImagingPalette palette, int dither)
{
    int y;
    ImagingShuffler convert;

    if (!imIn)
	return (Imaging) ImagingError_ModeError();

    if (!mode) {
	/* Map palette image to full depth */
	if (!imIn->palette)
	    return (Imaging) ImagingError_ModeError();
	mode = imIn->palette->mode;
    } else
	/* Same mode? */
	if (!strcmp(imIn->mode, mode))
	    return ImagingCopy2(imOut, imIn);


    /* test for special conversions */

    if (strcmp(imIn->mode, "P") == 0)
	return frompalette(imOut, imIn, mode);

    if (strcmp(mode, "P") == 0)
	return topalette(imOut, imIn, NULL, dither);

    if (dither && strcmp(mode, "1") == 0)
	return tobilevel(imOut, imIn, dither);


    /* standard conversion machinery */

    convert = NULL;

    for (y = 0; converters[y].from; y++)
	if (!strcmp(imIn->mode, converters[y].from) &&
	    !strcmp(mode, converters[y].to)) {
	    convert = converters[y].convert;
	    break;
	}

    if (!convert)
	return (Imaging) ImagingError_ValueError("conversion not supported");

    imOut = ImagingNew2(mode, imOut, imIn);
    if (!imOut)
        return NULL;

    for (y = 0; y < imIn->ysize; y++)
	(*convert)((UINT8*) imOut->image[y], (UINT8*) imIn->image[y],
		   imIn->xsize);

    return imOut;
}

Imaging
ImagingConvert(Imaging imIn, const char *mode,
               ImagingPalette palette, int dither)
{
    return convert(NULL, imIn, mode, palette, dither);
}

Imaging
ImagingConvert2(Imaging imOut, Imaging imIn)
{
    return convert(imOut, imIn, imOut->mode, NULL, 0);
}