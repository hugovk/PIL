/*
 * The Python Imaging Library.
 * $Id: Unpack.c,v 1.6 1996/08/15 13:12:44 fredrik Exp $
 *
 * code to unpack raw data from various file formats
 *
 * history:
 *	96-03-07 fl	Created (from various decoders)
 *	96-04-19 fl	Added band unpackers
 *	96-05-12 fl	Published RGB unpackers
 *	96-05-27 fl	Added nibble unpacker
 *	96-12-10 fl	Added complete set of PNG unpackers
 *	96-12-29 fl	Set alpha byte in RGB unpackers
 *	97-01-05 fl	Added remaining TGA unpackers
 *	97-01-18 fl	Added inverting band unpackers
 *	97-01-25 fl	Added FlashPix unpackers
 *	97-05-31 fl	Added floating point unpackers
 *	98-02-08 fl	Added I unpacker
 *
 * Copyright (c) Secret Labs AB 1997-98.
 * Copyright (c) Fredrik Lundh 1996-97.
 *
 * See the README file for information on usage and redistribution.
 */


#include "Imaging.h"


#define	R 0
#define	G 1
#define	B 2
#define	X 3

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

static void
unpackP2L(UINT8* out, const UINT8* in, int pixels)
{
    int i, j, m, s;
    /* bit layers */
    m = 128;
    s = (pixels+7)/8;
    for (i = j = 0; i < pixels; i++) {
	out[i] = ((in[j] & m) ? 1 : 0) + ((in[j + s] & m) ? 2 : 0);
	if ((m >>= 1) == 0) {
	    m = 128;
	    j++;
	}
    }
}

static void
unpackP4L(UINT8* out, const UINT8* in, int pixels)
{
    int i, j, m, s;
    /* bit layers (trust the optimizer ;-) */
    m = 128;
    s = (pixels+7)/8;
    for (i = j = 0; i < pixels; i++) {
	out[i] = ((in[j] & m) ? 1 : 0) + ((in[j + s] & m) ? 2 : 0) +
		 ((in[j + 2*s] & m) ? 4 : 0) + ((in[j + 3*s] & m) ? 8 : 0);
	if ((m >>= 1) == 0) {
	    m = 128;
	    j++;
	}
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
	out[A] = 255;
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
	out[A] = 255;
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
	out[A] = 255;
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
	out[A] = 255;
	out += 4; in += 3;
    }
}

void
ImagingUnpackBGR5(UINT8* out, const UINT8* in, int pixels)
{
    int i, pixel;
    /* RGB, reversed bytes, 5 bits per pixel */
    for (i = 0; i < pixels; i++) {
	pixel = in[0] + (in[1] << 8);
	out[B] = (pixel & 31) * 255 / 31;
	out[G] = ((pixel>>5) & 31) * 255 / 31;
	out[R] = ((pixel>>10) & 31) * 255 / 31;
	out[A] = 255;
	out += 4; in += 2;
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
	out[A] = 255;
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
unpackRGBAI(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* RGBA, inverted RGB bytes (FlashPix) */
    for (i = 0; i < pixels; i++) {
	out[R] = ~in[0];
	out[G] = ~in[1];
	out[B] = ~in[2];
	out[A] = in[3];
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
copy1(UINT8* out, const UINT8* in, int pixels)
{
    /* L, P */
    memcpy(out, in, pixels);
}

static void
copy4(UINT8* out, const UINT8* in, int pixels)
{
    /* RGBA, CMYK quadruples */
    memcpy(out, in, 4 * pixels);
}


/* Unpack to "F" image (now this is pretty ugly!) */

/* bytewise copy */

#define C16N\
        (tmp[0]=in[0], tmp[1]=in[1]);
#define C16S\
        (tmp[1]=in[0], tmp[0]=in[1]);
#define C32N\
        (tmp[0]=in[0], tmp[1]=in[1], tmp[2]=in[2], tmp[3]=in[3]);
#define C32S\
        (tmp[3]=in[0], tmp[2]=in[1], tmp[1]=in[2], tmp[0]=in[3]);
#define C64N\
        (tmp[0]=in[0], tmp[1]=in[1], tmp[2]=in[2], tmp[3]=in[3],\
         tmp[4]=in[4], tmp[5]=in[5], tmp[6]=in[6], tmp[7]=in[7]);
#define C64S\
        (tmp[7]=in[0], tmp[6]=in[1], tmp[5]=in[2], tmp[4]=in[3],\
         tmp[3]=in[4], tmp[2]=in[5], tmp[1]=in[6], tmp[0]=in[7]);

#ifdef WORDS_BIGENDIAN
#define C16B C16N
#define C16L C16S
#define C32B C32N
#define C32L C32S
#define C64B C64N
#define C64L C64S
#else
#define C16B C16S
#define C16L C16N
#define C32B C32S
#define C32L C32N
#define C64B C64S
#define C64L C64N
#endif

#define UNPACK_F(NAME, GET, TYPE)\
static void NAME(UINT8* out_, const UINT8* in, int pixels)\
{\
    int i;\
    FLOAT32* out = (FLOAT32*) out_;\
    for (i = 0; i < pixels; i++, in += sizeof(TYPE))\
        out[i] = (FLOAT32) ((TYPE) GET);\
}

#define UNPACK_C(NAME, COPY, TYPE)\
static void NAME(UINT8* out_, const UINT8* in, int pixels)\
{\
    int i;\
    FLOAT32* out = (FLOAT32*) out_;\
    TYPE tmp_;\
    UINT8* tmp = (UINT8*) &tmp_;\
    for (i = 0; i < pixels; i++, in += sizeof(TYPE)) {\
        COPY;\
        out[i] = (FLOAT32) tmp_;\
    }\
}

UNPACK_F(unpackF8,    in[0], UINT8)
UNPACK_F(unpackF8S,   in[0], INT8)
UNPACK_C(unpackF16,   C16L,  UINT16)
UNPACK_C(unpackF16S,  C16L,  INT16)
UNPACK_C(unpackF16B,  C16B,  UINT16)
UNPACK_C(unpackF16BS, C16B,  INT16)
UNPACK_C(unpackF16N,  C16N,  UINT16)
UNPACK_C(unpackF16NS, C16N,  INT16)
UNPACK_C(unpackF32,   C32L,  UINT32)
UNPACK_C(unpackF32S,  C32L,  INT32)
UNPACK_C(unpackF32B,  C32B,  UINT32)
UNPACK_C(unpackF32BS, C32B,  INT32)
UNPACK_C(unpackF32N,  C32N,  UINT32)
UNPACK_C(unpackF32NS, C32N,  INT32)
UNPACK_C(unpackF32F,  C32L,  FLOAT32)
UNPACK_C(unpackF32BF, C32B,  FLOAT32)
UNPACK_C(unpackF32NF, C32N,  FLOAT32)
#ifdef FLOAT64
UNPACK_C(unpackF64F,  C64L,  FLOAT64)
UNPACK_C(unpackF64BF, C64B,  FLOAT64)
UNPACK_C(unpackF64NF, C64N,  FLOAT64)
#endif


/* Misc. unpackers */

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

static void
band0I(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* band 0 only */
    for (i = 0; i < pixels; i++) {
	out[0] = ~in[i];
	out += 4;
    }
}

static void
band1I(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* band 1 only */
    for (i = 0; i < pixels; i++) {
	out[1] = ~in[i];
	out += 4;
    }
}

static void
band2I(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* band 2 only */
    for (i = 0; i < pixels; i++) {
	out[2] = ~in[i];
	out += 4;
    }
}

static void
band3I(UINT8* out, const UINT8* in, int pixels)
{
    /* band 3 only */
    int i;
    for (i = 0; i < pixels; i++) {
	out[3] = ~in[i];
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
    {"L",   	8,	copy1},
    {"L;I",   	8,	unpackLI},
    {"L;16",  	16,	unpackL16},
    {"L;16B",  	16,	unpackL16B},

    /* palette */
    {"P;1",   	1,	unpackP1},
    {"P;2",   	2,	unpackP2},
    {"P;2L",   	2,	unpackP2L},
    {"P;4",   	4,	unpackP4},
    {"P;4L",   	4,	unpackP4L},
    {"P",	8,	copy1},

    /* true colour */
    {"RGB",	24,	ImagingUnpackRGB},
    {"RGB;16B",	48,	unpackRGB16B},
    {"RGBX",	32,	copy4},
    {"RGB;L",	24,	unpackRGBL},
    {"BGR",	24,	ImagingUnpackBGR},
    {"BGRX",	32,	ImagingUnpackBGRX},
    {"BGR;5",	16,	ImagingUnpackBGR5},
    {"YCC;P",	24,	ImagingUnpackYCC},
    {"R",   	8,	band0},
    {"G",   	8,	band1},
    {"B",   	8,	band2},

    /* true colour w. transparency */
    {"LA",	16,	unpackLA},
    {"LA;16B",	32,	unpackLA16B},
    {"RGBA",	32,	copy4},
    {"RGBA;I",	32,	unpackRGBAI},
    {"RGBA;16B",64,	unpackRGBA16B},
    {"ABGR",	32,	unpackABGR},
    {"YCCA;P",	32,	ImagingUnpackYCCA},
    {"A",   	8,	band3},

    /* colour separation */
    {"CMYK",	32,	copy4},
    {"CMYK;I",	32,	unpackCMYKI},
    {"C",   	8,	band0},
    {"M",   	8,	band1},
    {"Y",   	8,	band2},
    {"K",   	8,	band3},
    {"C;I",   	8,	band0I},
    {"M;I",   	8,	band1I},
    {"Y;I",   	8,	band2I},
    {"K;I",   	8,	band3I},

    /* video (YCrCb) */
    {"YCC",	24,	ImagingUnpackRGB},
    {"YCCX",	32,	copy4},
    {"YCCK",	32,	copy4},

    /* integer */
    {"I",	32,	copy4},

    /* floating point variations */
    {"F",	32,	copy4},
    {"F;8",	8,	unpackF8},
    {"F;8S",	8,	unpackF8S},
    {"F;16",	16,	unpackF16},
    {"F;16S",	16,	unpackF16S},
    {"F;16B",	16,	unpackF16B},
    {"F;16BS",	16,	unpackF16BS},
    {"F;16N",	16,	unpackF16N},
    {"F;16NS",	16,	unpackF16NS},
    {"F;32",	32,	unpackF32},
    {"F;32S",	32,	unpackF32S},
    {"F;32B",	32,	unpackF32B},
    {"F;32BS",	32,	unpackF32BS},
    {"F;32N",	32,	unpackF32N},
    {"F;32NS",	32,	unpackF32NS},
    {"F;32F",	32,	unpackF32F},
    {"F;32BF",	32,	unpackF32BF},
    {"F;32NF",	32,	unpackF32NF},
#ifdef FLOAT64
    {"F;64F",	64,	unpackF64F},
    {"F;64BF",	64,	unpackF64BF},
    {"F;64NF",	64,	unpackF64NF},
#endif

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
