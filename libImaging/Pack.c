/*
 * The Python Imaging Library.
 * $Id: Pack.c,v 1.3 1996/08/15 13:12:44 fredrik Exp $
 *
 * code to pack raw data
 *
 * history:
 *	96-04-30 fl	Created
 *	96-05-12 fl	Published a few RGB packers
 *	96-11-01 fl	More RGB packers (Tk booster stuff)
 *	96-12-30 fl	Added P;1, P;2 and P;4 packers
 *	97-06-02 fl	Added F (F;32NF) packer
 *	97-08-28 fl	Added 1 as L packer
 *	98-02-08 fl	Added I packer
 *	98-03-09 fl	Added mode field, RGBA/RGBX as RGB packers
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


static void
pack1(UINT8* out, const UINT8* in, int pixels)
{
    int i, m, b;
    /* bilevel (black is 0) */
    b = 0; m = 128;
    for (i = 0; i < pixels; i++) {
	if (in[i] != 0)
	    b |= m;
	m >>= 1;
	if (m == 0) {
	    *out++ = b;
	    b = 0; m = 128;
	}
    }
    if (m != 128)
	*out++ = b;
}

static void
pack1I(UINT8* out, const UINT8* in, int pixels)
{
    int i, m, b;
    /* bilevel (black is 1) */
    b = 0; m = 128;
    for (i = 0; i < pixels; i++) {
	if (in[i] == 0)
	    b |= m;
	m >>= 1;
	if (m == 0) {
	    *out++ = b;
	    b = 0; m = 128;
	}
    }
    if (m != 128)
	*out++ = b;
}

static void
pack1R(UINT8* out, const UINT8* in, int pixels)
{
    int i, m, b;
    /* bilevel, lsb first (black is 0) */
    b = 0; m = 1;
    for (i = 0; i < pixels; i++) {
	if (in[i] != 0)
	    b |= m;
	m <<= 1;
	if (m == 256){
	    *out++ = b;
	    b = 0; m = 1;
	}
    }
    if (m != 1)
	*out++ = b;
}

static void
pack1IR(UINT8* out, const UINT8* in, int pixels)
{
    int i, m, b;
    /* bilevel, lsb first (black is 1) */
    b = 0; m = 1;
    for (i = 0; i < pixels; i++) {
	if (in[i] == 0)
	    b |= m;
	m <<= 1;
	if (m == 256){
	    *out++ = b;
	    b = 0; m = 1;
	}
    }
    if (m != 1)
	*out++ = b;
}

static void
pack1L(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* bilevel, stored as bytes */
    for (i = 0; i < pixels; i++)
	out[i] = (in[i] != 0) ? 255 : 0;
}

static void
packP4(UINT8* out, const UINT8* in, int pixels)
{
    while (pixels >= 2) {
	*out++ = (in[0] << 4) |
		 (in[1] & 15);
	in += 2; pixels -= 2;
    }

    if (pixels)
	out[0] = (in[0] << 4);
}

static void
packP2(UINT8* out, const UINT8* in, int pixels)
{
    while (pixels >= 4) {
	*out++ = (in[0] << 6) |
		 ((in[1] & 3) << 4) |
		 ((in[2] & 3) << 2) |
		 (in[3] & 3);
	in += 4; pixels -= 4;
    }

    switch (pixels) {
    case 3:
	out[0] = (in[0] << 6) |
		 ((in[1] & 3) << 4) |
		 ((in[2] & 3) << 2);
	break;
    case 2:
	out[0] = (in[0] << 6) |
		 ((in[1] & 3) << 4);
    case 1:
	out[0] = (in[0] << 6);
    }
}

void
ImagingPackRGB(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* RGB triplets */
    for (i = 0; i < pixels; i++) {
	out[0] = in[R];
	out[1] = in[G];
	out[2] = in[B];
	out += 3; in += 4;
    }
}

void
ImagingPackXRGB(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* XRGB, triplets with left padding */
    for (i = 0; i < pixels; i++) {
	out[0] = 0;
	out[1] = in[R];
	out[2] = in[G];
	out[3] = in[B];
	out += 4; in += 4;
    }
}

void
ImagingPackBGR(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* RGB, reversed bytes */
    for (i = 0; i < pixels; i++) {
	out[0] = in[B];
	out[1] = in[G];
	out[2] = in[R];
	out += 3; in += 4;
    }
}

void
ImagingPackBGRX(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* BGRX, reversed bytes with right padding */
    for (i = 0; i < pixels; i++) {
	out[0] = in[B];
	out[1] = in[G];
	out[2] = in[R];
	out[3] = 0;
	out += 4; in += 4;
    }
}

void
ImagingPackXBGR(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* XBGR, reversed bytes with left padding */
    for (i = 0; i < pixels; i++) {
	out[0] = 0;
	out[1] = in[B];
	out[2] = in[G];
	out[3] = in[R];
	out += 4; in += 4;
    }
}

static void
packRGBL(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* RGB, line interleaved */
    for (i = 0; i < pixels; i++) {
	out[i] = in[R];
	out[i+pixels] = in[G];
	out[i+pixels+pixels] = in[B];
	in += 4;
    }
}


static void
packRGBXL(UINT8* out, const UINT8* in, int pixels)
{
    int i;
    /* RGBX, line interleaved */
    for (i = 0; i < pixels; i++) {
	out[i] = in[R];
	out[i+pixels] = in[G];
	out[i+pixels+pixels] = in[B];
	out[i+pixels+pixels+pixels] = in[X];
	in += 4;
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

static struct {
    const char* mode;
    const char* rawmode;
    int bits;
    ImagingShuffler pack;
} packers[] = {

    /* bilevel */
    {"1",	"1",   		1,	pack1},
    {"1",	"1;I",   	1,	pack1I},
    {"1",	"1;R",   	1,	pack1R},
    {"1",	"1;IR",   	1,	pack1IR},
    {"1",	"L",		8,	pack1L},

    /* greyscale */
    {"L",	"L",   		8,	copy1},

    /* palette */
    {"P",	"P;1",		1,	pack1},
    {"P",	"P;2",		2,	packP2},
    {"P",	"P;4",		4,	packP4},
    {"P",	"P",		8,	copy1},

    /* true colour */
    {"RGB",	"RGB",		24,	ImagingPackRGB},
    {"RGB",	"RGBX",		32,	copy4},
    {"RGB",	"XRGB",		32,	ImagingPackXRGB},
    {"RGB",	"BGR",		24,	ImagingPackBGR},
    {"RGB",	"BGRX",		32,	ImagingPackBGRX},
    {"RGB",	"XBGR",		32,	ImagingPackXBGR},
    {"RGB",	"RGB;L",	24,	packRGBL},

    /* true colour w. transparency */
    {"RGBA",	"RGBA",		32,	copy4},
    {"RGBA",	"RGBA;L",	32,	packRGBXL},
    {"RGBA",	"RGB",		24,	ImagingPackRGB},

    /* true colour w. padding */
    {"RGBX",	"RGBX",		32,	copy4},
    {"RGBX",	"RGBX;L",	32,	packRGBXL},
    {"RGBX",	"RGB",		32,	ImagingPackRGB},

    /* colour separation */
    {"CMYK",	"CMYK",		32,	copy4},
    {"CMYK",	"CMYK;L",	32,	packRGBXL},

    /* video (YCrCb) */
    {"YCC",	"YCC",		24,	ImagingPackRGB},
    {"YCCX",	"YCCX",		32,	copy4},
    {"YCCK",	"YCCK",		32,	copy4},

    /* integer */
    {"I", 	"I",		32,	copy4},
    {"I", 	"I;32NF",	32,	copy4},

    /* floating point */
    {"F",	"F",		32,	copy4},
    {"F",	"F;32NF",	32,	copy4},

    {NULL} /* sentinel */
};


ImagingShuffler
ImagingFindPacker(const char* mode, const char* rawmode, int* bits_out)
{
    int i;

    /* find a suitable pixel packer */
    for (i = 0; packers[i].rawmode; i++)
	if (strcmp(packers[i].mode, mode) == 0 &&
            strcmp(packers[i].rawmode, rawmode) == 0) {
	    if (bits_out)
		*bits_out = packers[i].bits;
	    return packers[i].pack;
	}
    return NULL;
}
