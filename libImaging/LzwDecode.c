/*
 * The Python Imaging Library.
 * $Id: LzwDecode.c,v 1.4 1996/11/07 12:11:30 fredrik Exp $
 *
 * a fast, suspendable LZW decoder
 *
 * description:
 *	This code is based on the GIF decoder.  There are some
 *	subtle differences between GIF and TIFF LZW, though:
 *	- The fill order is different. In the TIFF file, you
 *	  must shift new bits in to the right, not to the left.
 *	- There is no blocking in the input data stream.
 *	- The code size is increased one step earlier than
 *	  for GIF
 *	- Image data are seen as a byte stream, not a pixel
 *	  stream. This means that the code size will always
 *	  start at 9 bits.
 *
 * history:
 *	95-09-13 fl	Created (derived from gifdecod.c)
 *	96-03-28 fl	Revised API, integrated with PIL
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


#include <stdio.h>
#include <stdlib.h>	/* memcpy() */

#include "Imaging.h"

#include "Lzw.h"


int
ImagingLzwDecode(Imaging im, ImagingCodecState state, UINT8* buf, int bytes)
{
    unsigned char *p;
    int c, i;
    int thiscode;
    LZWSTATE *lzwstate = (LZWSTATE*) state->context;

    unsigned char *ptr = buf;

    if (!state->state) {

	/* Clear code */
	lzwstate->clear = 1 << 8;

	/* End code */
	lzwstate->end = lzwstate->clear + 1;

	state->state = 1;
    }

    for (;;) {

	if (state->state == 1) {

	    /* First free entry in table */
	    lzwstate->next = lzwstate->clear + 2;

	    /* Initial code size */
	    lzwstate->codesize = 8 + 1;
	    lzwstate->codemask = (1 << lzwstate->codesize) - 1;

	    /* Stack pointer.  The stack goes backwards, thus
	       allowing us to return multiple bytes. */
	    lzwstate->stackindex = LZWSTACK;

	    state->state = 2;
	}

	if (lzwstate->stackindex < LZWSTACK) {

	    /* Return whole stack in one chunk */
	    i = LZWSTACK - lzwstate->stackindex;
	    p = &lzwstate->stack[lzwstate->stackindex];

	    lzwstate->stackindex = LZWSTACK;

	} else {

	    /* Get current symbol */
	    while (lzwstate->bitcount < lzwstate->codesize) {

		if (bytes < 1)
		    return ptr - buf;;

		/* Read next byte */
		c = *ptr++; bytes--;

		/* New bits are shifted in from from the right. */
		lzwstate->bitbuffer = (lzwstate->bitbuffer << 8) | c;
		lzwstate->bitcount += 8;

	    }

	    /* Extract current symbol from bit buffer. */
	    c = (lzwstate->bitbuffer >> (lzwstate->bitcount -
					 lzwstate->codesize))
		& lzwstate->codemask;

	    /* Adjust buffer */
	    lzwstate->bitcount -= lzwstate->codesize;

	    /* If c is less than clear, it's a data byte.  Otherwise,
	       it's either clear/end or a code symbol which should be
	       expanded. */

	    if (c == lzwstate->clear) {
		if (state->state != 2)
		    state->state = 1;
		continue;
	    }

	    if (c == lzwstate->end)
		break;

	    i = 1;
	    p = &lzwstate->lastdata;

	    if (state->state == 2) {

		/* First valid symbol after clear; use as is */
		if (c > lzwstate->clear) {
		    state->errcode = IMAGING_CODEC_BROKEN;
		    c = 0;
		}

		lzwstate->lastdata = lzwstate->lastcode = c;
		state->state = 3;

	    } else {

		thiscode = c;

		if (c > lzwstate->next) {
		    state->errcode = IMAGING_CODEC_BROKEN;
		    thiscode = 0;
		}

		if (c == lzwstate->next) {

		    /* c == next is allowed, by some strange reason */
		    lzwstate->stack[--lzwstate->stackindex] = lzwstate->lastdata;
		    c = lzwstate->lastcode;
		}

		while (c >= lzwstate->clear) {

		    /* This was a symbol. Push data string to stack. */
		    lzwstate->stack[--lzwstate->stackindex] = lzwstate->data[c];
		    c = lzwstate->link[c];
		}

		lzwstate->lastdata = c;

		if (lzwstate->next < LZWTABLE) {

		    /* While we still have room for it, add this
		       symbol to the table. */
		    lzwstate->data[lzwstate->next] = c;
		    lzwstate->link[lzwstate->next] = lzwstate->lastcode;

		    lzwstate->next++;

		    if (lzwstate->next == lzwstate->codemask &&
			lzwstate->codesize < LZWBITS) {

			/* Expand code size */
			lzwstate->codesize++;
			lzwstate->codemask = (1 << lzwstate->codesize) - 1;

		    }
		}
		lzwstate->lastcode = thiscode;
	    }
	}

	/* Update the output image */
	for (c = 0; c < i; c++) {

	    state->buffer[state->x] = p[c];

	    if (++state->x >= state->bytes) {

		/* Got a full line, unpack it */
		state->shuffle((UINT8*) im->image[state->y + state->yoff] +
			       state->xoff * im->pixelsize, state->buffer,
			       state->xsize);

		state->x = 0;

		if (++state->y >= state->ysize)
		    /* End of file (errcode = 0) */
		    return -1;
	    }
	}
    }

    return ptr - buf;
}
