/* 
 * THIS IS WORK IN PROGRESS.
 *
 * The Python Imaging Library
 * $Id: Quantize.c,v 1.3 1996/12/08 15:32:31 fl Exp $
 *
 * create palette from RGB image
 *
 * history:
 *	96-05-27 fl:	Created
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


#include <math.h>

#include "Imaging.h"

/*
 * Create a palette based on the colour contents of an RGB image.
 * 
 * This implementation is modelled after the one in the IJG JPEG library
 * by Thomas G. Lane.  It is based on the well-known algorithm by Paul
 * Heckbert, as described in:
 *   "Color Image Quantization for Frame Buffer Display", Proc.
 *   SIGGRAPH '82, Computer Graphics v.16 #3 (July 1982).
 *
 * The IJG JPEG library is copyright (C) 1991-1995, Thomas G. Lane.
 *
 * Note that the histogram buffer has the same size as the palette cache,
 * meaning that the buffer may be reused as cache where suitable.
 */

#define	DIST(a, b, s) (a - b) * (a - b) * s

#define	RED	0
#define	GREEN	1
#define	BLUE	2

/* Colour weights */
#define	RSCALE	2
#define	GSCALE	4
#define	BSCALE	1

/* Calculated scaled distances */
#define	RDIST(a, b) DIST(a, b, RSCALE*RSCALE)
#define	GDIST(a, b) DIST(a, b, GSCALE*GSCALE)
#define	BDIST(a, b) DIST(a, b, BSCALE*BSCALE)


typedef struct {
    int count;	    /* number of boxes */
    struct {
	int r0, r1; /* in RGB space */
	int g0, g1;
	int b0, b1;
	int diagonal;
	int count;
    } box[256];
} BOXES;


#define	HISTO(h, r, g, b) h[(r>>2) + (g>>2)*64 + (b>>2)*64*64]


static inline void
setup(BOXES *bx, int i, INT16* h,
      int r0, int r1, int b0, int b1, int g0, int g1)
{
    /* Initialize a new box covering (r0,g0,b0)-(r1,g1,b1) */

    int r,g,b;
    int count;

    /* Step 1 -- shrink box to fit its population */

    for (r = r0; r < r1; r += 4)
	for (g = g0; g < g1; g += 4)
	    for (b = b0; b < b1; b += 4)
		if (HISTO(h, r, g, b) != 0) {
		    r0 = r;
		    goto r0ok;
		}
r0ok:
    for (r = r1-4; r >= r0; r -= 4)
	for (g = g0; g < g1; g += 4)
	    for (b = b0; b < b1; b += 4)
		if (HISTO(h, r, g, b) != 0) {
		    r1 = r+4;
		    goto r1ok;
		}
r1ok:
    for (g = g0; g < g1; g += 4)
	for (r = r0; r < r1; r += 4)
	    for (b = b0; b < b1; b += 4)
		if (HISTO(h, r, g, b) != 0) {
		    g0 = g;
		    goto g0ok;
		}
g0ok:
    for (g = g1-4; g >= g0; g -= 4)
	for (r = r0; r < r1; r += 4)
	    for (b = b0; b < b1; b += 4)
		if (HISTO(h, r, g, b) != 0) {
		    g1 = g+4;
		    goto g1ok;
		}
g1ok:
    for (b = b0; b < b1; b += 4)
	for (r = r0; r < r1; r += 4)
	    for (g = g0; g < g1; g += 4)
		if (HISTO(h, r, g, b) != 0) {
		    b0 = b;
		    goto b0ok;
		}
b0ok:
    for (b = b1-4; b >= b0; b -= 4)
	for (r = r0; r < r1; r += 4)
	    for (g = g0; g < g1; g += 4)
		if (HISTO(h, r, g, b) != 0) {
		    b1 = b+4;
		    goto b1ok;
		}
b1ok:

    /* Step 2 -- Initialize box descriptor */

    bx->box[i].r0 = r0; bx->box[i].r1 = r1;
    bx->box[i].g0 = g0; bx->box[i].g1 = g1;
    bx->box[i].b0 = b0; bx->box[i].b1 = b1;

    bx->box[i].diagonal = RDIST(r0, r1) + GDIST(g0, g1) + BDIST(b0, b1);

    count = 0;
    for (r = r0; r < r1; r += 4)
	for (g = g0; g < g1; g += 4)
	    for (b = b0; b < b1; b += 4)
		if (HISTO(h, r, g, b) != 0)
		    count++;

    bx->box[i].count = count;

}


ImagingPalette
ImagingQuantize(Imaging imIn, int count)
{
    unsigned x, y;
    int i, imax;
    long cmax;
    BOXES bx;
    INT16* h;
    int r, g, b;
    int r0, r1, b0, b1, g0, g1;
    int R0, R1, B0, B1, G0, G1;
    int entries = 64*64*64;

    /* FIXME: check mode */

    if (!imIn)
	return ImagingError_Argument(NULL);

    /* Take a histogram of the image */

    printf("histogram...\n");

    h = (INT16*) calloc(entries, sizeof(INT16));
    if (!h)
	return (ImagingPalette) ImagingError_NoMemory();

    for (y = 0; y < imIn->ysize; y++) {
	UINT8* in = (UINT8*) imIn->image[y];
	for (x = 0; x < imIn->xsize; x++) {
	    INT16* hp = &HISTO(h, in[0], in[1], in[2]);
	    if (++hp[0] <= 0)
		--hp[0];
	    in += 4;
	}
    }

    printf("quantize...\n");

    /* Create the first box, covering the whole colour space. */

    bx.count = 1;
    setup(&bx, 0, h, 0, 255, 0, 255, 0, 255);

    /* Now loop and split until we have enough boxes */

    while (bx.count < count) {

	/* For first half of palette, we look for max population, for
	   the second, for max volume (from the IJG JPEG library). */

	cmax = 0;

	if (bx.count < count/2) {

	    /* Select by population */
	    for (i = 0; i < bx.count; i++)
		if (bx.box[i].count > cmax && bx.box[i].diagonal > 0) {
		    imax = i;
		    cmax = bx.box[i].count;
		}

	} else {

	    /* Select by box volume (squared diagonal, actually) */
	    for (i = 0; i < bx.count; i++)
		if (bx.box[i].diagonal > cmax) {
		    imax = i;
		    cmax = bx.box[i].diagonal;
		}

	}

	if (cmax <= 0)
	    break; /* no more boxes */
	
	/* Get bounds from existing box */
	r0 = R0 = bx.box[imax].r0; r1 = R1 = bx.box[imax].r1;
	g0 = G0 = bx.box[imax].g0; g1 = G1 = bx.box[imax].g1;
	b0 = B0 = bx.box[imax].b0; b1 = B1 = bx.box[imax].b1;

	r = (r1 - r0) * RSCALE;
	g = (g1 - g0) * GSCALE;
	b = (b1 - b0) * BSCALE;

	/* Choose axis to split, and split box along that axis. */

	i = GREEN;
	cmax = g;

	if (r > cmax) {
	    i = RED;
	    cmax = r;
	}

	if (b > cmax)
	    i = BLUE;

	switch (i) {
	case GREEN:
	    g1 = (g0 + g1-4) / 2;
	    G0 = g1+4;
	    break;
	case RED:
	    r1 = (r0 + r1-4) / 2;
	    R0 = r1+4;
	    break;
	case BLUE:
	    b1 = (b0 + b1-4) / 2;
	    B0 = b1+4;
	    break;
	}

	setup(&bx, imax,     h, r0, r1, b0, b1, g0, g1);
	setup(&bx, bx.count, h, R0, R1, B0, B1, G0, G1);

	bx.count++;
    }

    free(h);

    /* Colours are now sorted after importance. */

    printf("ok... (%d colours)\n", bx.count);

    for (i = 0; i < bx.count; i++) {

	/* FIXME: This can be done better... */
	r = (bx.box[i].r0 + bx.box[i].r1 - 4) / 2;
	g = (bx.box[i].g0 + bx.box[i].g1 - 4) / 2;
	b = (bx.box[i].b0 + bx.box[i].b1 - 4) / 2;

	printf("%d: %d/%d/%d\n", i, r, g, b);

    }

    return NULL;

}
