/*
 * The Python Imaging Library.
 * $Id: PcxDecode.c,v 1.3 1996/08/15 13:12:44 fredrik Exp $
 *
 * decoder for Targa RLE data.
 *
 * history:
 *	97-01-04 fl	Created
 *
 * Copyright (c) Fredrik Lundh 1997.
 * Copyright (c) Secret Labs AB 1997.
 *
 * See the README file for information on usage and redistribution.
 */


#include "Imaging.h"


int
ImagingTgaRleDecode(Imaging im, ImagingCodecState state,
		    UINT8* buf, int bytes)
{
    int n;
    UINT8* ptr;

    ptr = buf;

    for (;;) {

	if (bytes < 1)
	    return ptr - buf;

	if (ptr[0] & 0x80) {

	    /* Run (2 bytes) */

	    if (bytes < 2)
		break;

	    n = (ptr[0] & 0x7f) + 1;

	    if (state->x + n > state->bytes) {
		state->errcode = IMAGING_CODEC_OVERRUN;
		return -1;
	    }

	    memset(state->buffer + state->x, ptr[1], n);
		
	    ptr += 2;
	    bytes -= 2;

	} else {

	    /* Literal (1+n+1 bytes block) */
	    n = ptr[0] + 1;

	    if (bytes < 1 + n + 1)
		break;

	    if (state->x + n > state->bytes) {
		state->errcode = IMAGING_CODEC_OVERRUN;
		return -1;
	    }

	    memcpy(state->buffer + state->x, ptr + 1, n);

	    ptr += 1 + n + 1;
	    bytes -= 1 + n + 1;

	}

	state->x += n;

	if (state->x >= state->bytes) {

	    /* Got a full line, unpack it */
	    state->shuffle((UINT8*) im->image[state->y + state->yoff] +
			   state->xoff * im->pixelsize, state->buffer,
			   state->xsize);

	    state->x = 0;

	    if (++state->y >= state->ysize) {
		/* End of file (errcode = 0) */
		return -1;
	    }
	}

    }

    return ptr - buf;
}
