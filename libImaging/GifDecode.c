/*
 * The Python Imaging Library.
 * $Id: GifDecode.c,v 1.6 1996/12/16 18:51:03 fl Exp fl $
 *
 * a fast, suspendable GIF decoder
 *
 * history:
 *	95-09-03 fl	Created
 *	95-09-05 fl	Fixed sign problem on 16-bit platforms
 *	95-09-13 fl	Added some storage shortcuts
 *	96-03-28 fl	Revised API, integrated with PIL
 *	96-12-10 fl	Added interlace support
 *	96-12-16 fl	Fixed premature termination bug introduced by last fix
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


#include <stdio.h>
#include <memory.h>	/* memcpy() */

#include "Imaging.h"

#include "Gif.h"


#define NEWLINE(state, gifstate) {\
    state->x = 0;\
    state->y += gifstate->step;\
    if (state->y >= state->ysize)\
	switch (gifstate->interlace) {\
	case 1:\
	    gifstate->repeat = state->y = 4;\
	    gifstate->interlace = 2;\
	    break;\
	case 2:\
	    gifstate->step = 4;\
	    gifstate->repeat = state->y = 2;\
	    gifstate->interlace = 3;\
	    break;\
	case 3:\
	    gifstate->step = 2;\
	    gifstate->repeat = state->y = 1;\
	    gifstate->interlace = 0;\
	    break;\
	default:\
	    return -1;\
        }\
}


int
ImagingGifDecode(Imaging im, ImagingCodecState state, UINT8* buffer, int bytes)
{
    unsigned char *p;
    int c, i;
    int thiscode;
    GIFSTATE *gifstate = (GIFSTATE*) state->context;

    unsigned char *ptr = buffer;

    if (!state->state) {

	/* Initialise state */
					
	/* Clear code */
	gifstate->clear = 1 << gifstate->bits;

	/* End code */
	gifstate->end = gifstate->clear + 1;

	/* Interlace */
	if (gifstate->interlace) {
	    gifstate->interlace = 1;
	    gifstate->step = gifstate->repeat = 8;
	} else
	    gifstate->step = 1;

	state->state = 1;
    }

    for (;;) {

	if (state->state == 1) {

	    /* First free entry in table */
	    gifstate->next = gifstate->clear + 2;

	    /* Initial code size */
	    gifstate->codesize = gifstate->bits + 1;
	    gifstate->codemask = (1 << gifstate->codesize) - 1;

	    /* Stack pointer.  The stack goes backwards, thus
	       allowing us to return multiple bytes. */
	    gifstate->stackindex = GIFSTACK;

	    state->state = 2;
	}

	if (gifstate->stackindex < GIFSTACK) {

	    /* Return whole stack in one chunk */
	    i = GIFSTACK - gifstate->stackindex;
	    p = &gifstate->stack[gifstate->stackindex];

	    gifstate->stackindex = GIFSTACK;

	} else {

	    /* Get current symbol */

	    while (gifstate->bitcount < gifstate->codesize) {

		if (gifstate->blocksize > 0) {

		    /* Read next byte */
		    c = *ptr++; bytes--;

		    gifstate->blocksize--;

		    /* New bits are shifted in from from the left. */
		    gifstate->bitbuffer |= (INT32) c << gifstate->bitcount;
		    gifstate->bitcount += 8;

		} else {

		    /* New GIF block */

		    /* We don't start decoding unless we have a full block */
		    if (bytes < 1)
			return ptr - buffer;
		    c = *ptr;
		    if (bytes < c+1)
			return ptr - buffer;

		    gifstate->blocksize = c;

		    ptr++; bytes--;

		}
	    }

	    /* Extract current symbol from bit buffer. */
	    c = (int) gifstate->bitbuffer & gifstate->codemask;

	    /* Adjust buffer */
	    gifstate->bitbuffer >>= gifstate->codesize;
	    gifstate->bitcount -= gifstate->codesize;

	    /* If c is less than clear, it's a data byte.  Otherwise,
	       it's either clear/end or a code symbol which should be
	       expanded. */

	    if (c == gifstate->clear) {
		if (state->state != 2)
		    state->state = 1;
		continue;
	    }

	    if (c == gifstate->end)
		break;

	    i = 1;
	    p = &gifstate->lastdata;

	    if (state->state == 2) {

		/* First valid symbol after clear; use as is */
		if (c > gifstate->clear) {
		    state->errcode = IMAGING_CODEC_BROKEN;
		    c = 0; /* Continue anyway */
		}

		gifstate->lastdata = gifstate->lastcode = c;
		state->state = 3;

	    } else {

		thiscode = c;

		if (c > gifstate->next) {
		    state->errcode = IMAGING_CODEC_BROKEN;
		    thiscode = 0; /* continue anyway */
		}

		if (c == gifstate->next) {

		    /* c == next is allowed. not sure why. */
		    gifstate->stack[--gifstate->stackindex] =
			gifstate->lastdata;
		    c = gifstate->lastcode;
		}

		while (c >= gifstate->clear) {

		    /* This was a symbol. Push data string to stack. */
		    gifstate->stack[--gifstate->stackindex] =
			gifstate->data[c];
		    c = gifstate->link[c];
		}

		gifstate->lastdata = c;

		if (gifstate->next < GIFTABLE) {

		    /* While we still have room for it, add this
		       symbol to the table. */
		    gifstate->data[gifstate->next] = c;
		    gifstate->link[gifstate->next] = gifstate->lastcode;

		    if (gifstate->next == gifstate->codemask &&
			gifstate->codesize < GIFBITS) {

			/* Expand code size */
			gifstate->codesize++;
			gifstate->codemask = (1 << gifstate->codesize) - 1;
		    }

		    gifstate->next++;

		}
		gifstate->lastcode = thiscode;
	    }
	}

	/* Copy the bytes into the image */
	if (state->y >= state->ysize) {
	    state->errcode = IMAGING_CODEC_OVERRUN;
	    return -1;
	}

	/* To squeeze some extra pixels out every second, we test
	   for some common cases and handle them separately. */ 

	if (i == 1) {
	    if (state->x < state->xsize-1) {
		/* Single pixel, not at the end of the line. */
		im->image8[state->y+state->yoff][state->x+state->xoff] = p[0];
		state->x++;
		continue;
	    }
	} else if (state->x + i <= state->xsize) {
	    /* This string fits into current line. */
	    memcpy(im->image8[state->y+state->yoff] + state->x +
		   state->xoff, p, i);
	    state->x += i;
	    if (state->x == state->xsize) {
		NEWLINE(state, gifstate);
	    }
	    continue;
	}

	/* Didn't find a shortcut */
	for (c = 0; c < i; c++) {
	    im->image8[state->y+state->yoff][state->x+state->xoff] = p[c];
	    if (++state->x >= state->xsize) {
		NEWLINE(state, gifstate);
	    }
	}
    }

    return ptr - buffer;
}
