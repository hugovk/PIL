/*
 * The Python Imaging Library.
 * $Id: Unpack.c,v 1.8 1996/12/16 18:51:03 fl Exp fl $
 *
 * code to unpack raw data from various file formats
 *
 * history:
 *	96-03-07 fl	Created (from various decoders)
 *	96-04-19 fl	Added band unpackers
 *	96-05-12 fl	Published RGB unpackers
 *	96-05-27 fl	Added nibble unpacker
 *	96-12-10 fl	Added complete set of PNG unpackers
 *
 * Copyright (c) Fredrik Lundh 1996.  All rights reserved.
 */


#include "Imaging.h"


#define	R 0
#define	G 1
#define	B 2
#define	A 3

#define	C 0
#define	M 1
#define	Y 2
#define	K 3


/* Unpack to "1" image */

static void
unpack1(UINT8* out, const UINT8* in, int pixels)
{
    /* bits (msb first, white is non-zero) */
    while (pixels > 0) {
	UINT8 byte = *in++;
	switch (pixels) {
	default:    *out++ = (byte & 128) ? 255 : 0; byte <<= 1;
	case 7:	    *out++ = (byte & 128) ? 255 : 0; byte <<= 1;
	case 6:	    *out++ = (byte & 128) ? 255 : 0; byte <<= 1;
	case 5:	    *out++ = (byte & 128) ? 255 : 0; byte <<= 1;
	case 4:	    *out++ = (byte & 128) ? 255 : 0; byte <<= 1;
	case 3:	    *out++ = (byte & 128) ? 255 : 0; byte <<= 1;
	case 2:	    *out++ = (byte & 128) ? 255 : 0; byte <<= 1;
	case 1:	    *out++ = (byte & 128) ? 255 : 0;
	}
	pixels -= 8;
    }
}

static void
unpack1I(UINT8* out, const UINT8* in, int pixels)
{
    /* bits (msb first, white is zero) */
    while (pixels > 0) {
	UINT8 byte = *in++;
	switch (pixels) {
	default:    *out++ = (byte & 128) ? 0 : 255; byte <<= 1;
	case 7:	    *out++ = (byte & 128) ? 0 : 255; byte <<= 1;
	case 6:	    *out++ = (byte & 128) ? 0 : 255; byte <<= 1;
	case 5:	    *out++ = (byte & 128) ? 0 : 255; byte <<= 1;
	case 4:	    *out++ = (byte & 128) ? 0 : 255; byte <<= 1;
	case 3:	    *out++ = (byte & 128) ? 0 : 255; byte <<= 1;
	case 2:	    *out++ = (byte & 128) ? 0 : 255; byte <<= 1;
	case 1:	    *out++ = (byte & 128) ? 0 : 255;
	}
	pixels -= 8;
    }
}

static void
unpack1R(UINT8* out, const UINT8* in, int pixels)
{
    /* bits (lsb first, white is non-zero) */
    while (pixels > 0) {
	UINT8 byte = *in++;
	switch (pixels) {
	default:    *out++ = (byte & 1) ? 255 : 0; byte >>= 1;
	case 7:	    *out++ = (byte & 1) ? 255 : 0; byte >>= 1;
	case 6:	    *out++ = (byte & 1) ? 255 : 0; byte >>= 1;
	case 5:	    *out++ = (byte & 1) ? 255 : 0; byte >>= 1;
	case 4:	    *out++ = (byte & 1) ? 255 : 0; byte >>= 1;
	case 3:	    *out++ = (byte & 1) ? 255 : 0; byte >>= 1;
	case 2:	    *out++ = (byte & 1) ? 255 : 0; byte >>= 1;
	case 1:	    *out++ = (byte & 1) ? 255 : 0;
	}
	pixels -= 8;
    }
}

static void
unpack1IR(UINT8* out, const UINT8* in, int pixels)
{
    /* bits (lsb first, white is zero) */
    while (pixels > 0) {
	UINT8 byte = *in++;
	switch (pixels) {
	default:    *out++ = (byte & 1) ? 0 : 255; byte >>= 1;
	case 7:	    *out++ = (byte & 1) ? 0 : 255; byte >>= 1;
	case 6:	    *out++ = (byte & 1) ? 0 : 255; byte >>= 1;
	case 5:	    *out++ = (byte & 1) ? 0 : 255; byte >>= 1;
	case 4:	    *out++ = (byte & 1) ? 0 : 255; byte >>= 1;
	case 3:	    *out++ = (byte & 1) ? 0 : 255; byte >>= 1;
	case 2:	    *out++ = (byte & 1) ? 0 : 255; byte >>= 1;
	case 1:	    *out++ = (byte & 1) ? 0 : 255;
	}
	pixels -= 8;
    }
}


/* Unpack to "L" image */

static void
unpackL2(UINT8* out, const UINT8* in, int pixels)
{
    /* nibbles */
    while (pixels > 0) {
	UINT8 byte = *in++;
	switch (pixels) {
	default:    *out++ = ((byte >> 6) & 3) * 255 / 3; byte <<= 2;
	case 3:	    *out++ = ((byte >> 6) & 3) * 255 / 3; byte <<= 2;
	case 2:	    *out++ = ((byte >> 6) & 3) * 255 / 3; byte <<= 2;
	case 1:	    *out++ = ((byte >> 6) & 3) * 255 / 3;
	}
	pixels -= 4;
    }
}

static void
unpackL4(UINT8* out, const UINT8* in, int pixels)
{
    /* nibbles */
    while (pixels > 0) {
	UINT8 byte = *in++;
	switch (pixels) {
	default:    *out++ = ((byte >> 4) & 15) * 255 / 15; byte <<= 4;
	case 1:	    *out++ = ((byte >> 4) & 15) * 255 / 15;
	}
	pixels -= 2;
    }
}

static void
unpackLI(UINT8* out, const UINT8* in, int pixels)
{
    /* negative */
    int i;
    for (i = 0; i < pixels; i++)
	out[i] = ~in[i];
}

static void
unpackL16(UINT8* out, const UINT8* in, int pixels)
{
    /* int16 (upper byte, little endian) */
    int i;
    for (i = 0; i < pixels; i++) {
	out[i] = in[1];
	in += 2;
    }
}

static void
unpackL16B(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* int16 (upper byte, big endian) */
    for (i = 0; i < pixels; i++) {
	out[i] = in[0];
	in += 2;
    }
}


/* Unpack to "P" image */

static void
unpackP1(UINT8* out, const UINT8* in, int pixels)
{
    /* bits */
    while (pixels > 0) {
	UINT8 byte = *in++;
	switch (pixels) {
	default:    *out++ = (byte >> 7) & 1; byte <<= 1;
	case 7:	    *out++ = (byte >> 7) & 1; byte <<= 1;
	case 6:	    *out++ = (byte >> 7) & 1; byte <<= 1;
	case 5:	    *out++ = (byte >> 7) & 1; byte <<= 1;
	case 4:	    *out++ = (byte >> 7) & 1; byte <<= 1;
	case 3:	    *out++ = (byte >> 7) & 1; byte <<= 1;
	case 2:	    *out++ = (byte >> 7) & 1; byte <<= 1;
	case 1:	    *out++ = (byte >> 7) & 1;
	}
	pixels -= 8;
    }
}

static void
unpackP2(UINT8* out, const UINT8* in, int pixels)
{
    /* bit pairs */
    while (pixels > 0) {
	UINT8 byte = *in++;
	switch (pixels) {
	default:    *out++ = (byte >> 6) & 3; byte <<= 2;
	case 3:	    *out++ = (byte >> 6) & 3; byte <<= 2;
	case 2:	    *out++ = (byte >> 6) & 3; byte <<= 2;
	case 1:	    *out++ = (byte >> 6) & 3;
	}
	pixels -= 4;
    }
}

static void
unpackP4(UINT8* out, const UINT8* in, int pixels)
{
    /* nibbles */
    while (pixels > 0) {
	UINT8 byte = *in++;
	switch (pixels) {
	default:    *out++ = (byte >> 4) & 15; byte <<= 4;
	case 1:	    *out++ = (byte >> 4) & 15;
	}
	pixels -= 2;
    }
}


/* Unpack to "RGB" image */

void
ImagingUnpackRGB(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* RGB triplets */
    for (i = 0; i < pixels; i++) {
	out[R] = in[0];
	out[G] = in[1];
	out[B] = in[2];
	out += 4; in += 3;
    }
}

void
unpackRGB16B(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* 16-bit RGB triplets, big-endian order */
    for (i = 0; i < pixels; i++) {
	out[R] = in[0];
	out[G] = in[2];
	out[B] = in[4];
	out += 4; in += 6;
    }
}

static void
unpackRGBL(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* RGB, line interleaved */
    for (i = 0; i < pixels; i++) {
	out[R] = in[i];
	out[G] = in[i+pixels];
	out[B] = in[i+pixels+pixels];
	out += 4;
    }
}


void
ImagingUnpackBGR(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* RGB, reversed bytes */
    for (i = 0; i < pixels; i++) {
	out[R] = in[2];
	out[G] = in[1];
	out[B] = in[0];
	out += 4; in += 3;
    }
}

static void
ImagingUnpackBGRX(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* RGB, reversed bytes with padding */
    for (i = 0; i < pixels; i++) {
	out[R] = in[2];
	out[G] = in[1];
	out[B] = in[0];
	out += 4; in += 4;
    }
}


/* Unpack to "RGBA" image */

static void
unpackLA(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* greyscale with alpha */
    for (i = 0; i < pixels; i++) {
	out[R] = out[G] = out[B] = in[0];
	out[A] = in[1];
	out += 4; in += 2;
    }
}

static void
unpackLA16B(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* 16-bit greyscale with alpha, big-endian */
    for (i = 0; i < pixels; i++) {
	out[R] = out[G] = out[B] = in[0];
	out[A] = in[2];
	out += 4; in += 4;
    }
}

static void
unpackRGBA16B(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* 16-bit truecolour with alpha, big-endian */
    for (i = 0; i < pixels; i++) {
	out[R] = in[0];
	out[G] = in[2];
	out[B] = in[4];
	out[A] = in[6];
	out += 4; in += 8;
    }
}

static void
unpackABGR(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* RGBA, reversed bytes */
    for (i = 0; i < pixels; i++) {
	out[R] = in[3];
	out[G] = in[2];
	out[B] = in[1];
	out[A] = in[0];
	out += 4; in += 4;
    }
}


/* Unpack to "CMYK" image */

static void
unpackCMYKI(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* CMYK, inverted bytes (Photoshop 2.5) */
    for (i = 0; i < pixels; i++) {
	out[C] = ~in[0];
	out[M] = ~in[1];
	out[Y] = ~in[2];
	out[K] = ~in[3];
	out += 4; in += 4;
    }
}

static void
copy4(UINT8* out, const UINT8* in, int pixels)
{
    /* RGBA, CMYK quadruples */
    memcpy(out, in, 4 * pixels);
}

static void
band0(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* band 0 only */
    for (i = 0; i < pixels; i++) {
	out[0] = in[i];
	out += 4;
    }
}

static void
band1(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* band 1 only */
    for (i = 0; i < pixels; i++) {
	out[1] = in[i];
	out += 4;
    }
}

static void
band2(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* band 2 only */
    for (i = 0; i < pixels; i++) {
	out[2] = in[i];
	out += 4;
    }
}

static void
band3(UINT8* out, const UINT8* in, int pixels)
{
    /* band 3 only */
    int i;
    for (i = 0; i < pixels; i++) {
	out[3] = in[i];
	out += 4;
    }
}

static struct {
    const char* rawmode;
    int bits;
    ImagingShuffler unpack;
} unpackers[] = {

    /* bilevel */
    {"1",	1,	unpack1},
    {"1;I",	1,	unpack1I},
    {"1;R",	1,	unpack1R},
    {"1;IR",	1,	unpack1IR},

    /* greyscale */
    {"L;2",  	2,	unpackL2},
    {"L;4",  	4,	unpackL4},
    {"L",   	8,	(ImagingShuffler) memcpy},
    {"L;I",   	8,	unpackLI},
    {"L;16",  	16,	unpackL16},
    {"L;16B",  	16,	unpackL16B},

    /* palette */
    {"P;1",   	1,	unpackP1},
    {"P;2",   	2,	unpackP2},
    {"P;4",   	4,	unpackP4},
    {"P",	8,	(ImagingShuffler) memcpy},

    /* true colour */
    {"RGB",	24,	ImagingUnpackRGB},
    {"RGB;16B",	48,	unpackRGB16B},
    {"RGBX",	32,	copy4},
    {"RGB;L",	24,	unpackRGBL},
    {"BGR",	24,	ImagingUnpackBGR},
    {"BGRX",	32,	ImagingUnpackBGRX},
    {"R",   	8,	band0},
    {"G",   	8,	band1},
    {"B",   	8,	band2},

    /* true colour w. transparency */
    {"LA",	16,	unpackLA},
    {"LA;16B",	32,	unpackLA16B},
    {"RGBA",	32,	copy4},
    {"RGBA;16B",64,	unpackRGBA16B},
    {"ABGR",	32,	unpackABGR},
    {"A",   	8,	band3},

    /* colour separation */
    {"CMYK",	32,	copy4},
    {"CMYK;I",	32,	unpackCMYKI},
    {"C",   	8,	band0},
    {"M",   	8,	band1},
    {"Y",   	8,	band2},
    {"K",   	8,	band3},

    /* video (YCrCb) */
    {"YCC",	24,	ImagingUnpackRGB},
    {"YCCX",	32,	copy4},
    {"YCCK",	32,	copy4},

    {NULL} /* sentinel */
};


ImagingShuffler
ImagingFindUnpacker(const char* rawmode, int* bits_out)
{
    int i;

    /* find a suitable pixel unpacker */
    for (i = 0; unpackers[i].rawmode; i++)
	if (strcmp(unpackers[i].rawmode, rawmode) == 0) {
	    if (bits_out)
		*bits_out = unpackers[i].bits;
	    return unpackers[i].unpack;
	}
    return NULL;
}
