/*
 * The Python Imaging Library.
 * $Id: Pack.c,v 1.4 1996/11/07 12:11:30 fredrik Exp $
 *
 * code to pack raw data
 *
 * history:
 *	96-04-30 fl	Created
 *	96-05-12 fl	Published a few RGB packers
 *	96-11-01 fl	More RGB packers (Tk booster stuff)
 *
 * Copyright (c) Fredrik Lundh 1996.  All rights reserved.
 */


#include "Imaging.h"


#define	R 0
#define	G 1
#define	B 2


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
    /* XRGB, tripleds with left padding */
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
copy4(UINT8* out, const UINT8* in, int pixels)
{
    /* RGBA, CMYK quadruples */
    memcpy(out, in, 4 * pixels);
}

static struct {
    const char* rawmode;
    int bits;
    ImagingShuffler pack;
} packers[] = {

    /* bilevel */
    {"1",   	1,	pack1},
    {"1;I",   	1,	pack1I},
    {"1;R",   	1,	pack1R},
    {"1;IR",   	1,	pack1IR},

    /* greyscale */
    {"L",   	8,	(ImagingShuffler) memcpy},

    /* palette */
    {"P",	8,	(ImagingShuffler) memcpy},

    /* true colour */
    {"RGB",	24,	ImagingPackRGB},
    {"RGBX",	32,	copy4},
    {"XRGB",	32,	ImagingPackXRGB},
    {"BGR",	24,	ImagingPackBGR},
    {"BGRX",	32,	ImagingPackBGRX},
    {"XBGR",	32,	ImagingPackXBGR},

    /* true colour w. transparency */
    {"RGBA",	32,	copy4},

    /* colour separation */
    {"CMYK",	32,	copy4},

    /* video (YCrCb) */
    {"YCC",	24,	ImagingPackRGB},
    {"YCCX",	32,	copy4},
    {"YCCK",	32,	copy4},

    {NULL} /* sentinel */
};


ImagingShuffler
ImagingFindPacker(const char* rawmode, int* bits_out)
{
    int i;

    /* find a suitable pixel packer */
    for (i = 0; packers[i].rawmode; i++)
	if (strcmp(packers[i].rawmode, rawmode) == 0) {
	    if (bits_out)
		*bits_out = packers[i].bits;
	    return packers[i].pack;
	}
    return NULL;
}
