/*
 * The Python Imaging Library.
 * $Id: RawEncode.c,v 1.3 1996/11/07 12:11:30 fredrik Exp $
 *
 * coder for raw data
 *
 * notes:
 *	FIXME: This encoder will fail if the buffer is not large
 *	enough to hold one full line of data.  There's a workaround
 *	for this problem in ImageFile.py, but it should be solved
 *	here instead.
 *
 *	FIXME: Padding is not supported.
 *
 * history:
 *	96-04-30 fl	created
 *
 * Copyright (c) Fredrik Lundh 1996.  All rights reserved.
 */


#include "Imaging.h"


int
ImagingRawEncode(Imaging im, ImagingCodecState state, UINT8* buf, int bytes)
{
    UINT8* ptr;

    if (!state->state) {

	state->bytes = (state->xsize * state->bits + 7) / 8;

	if (state->ystep < 0) {
	    state->y = state->ysize-1;
	    state->ystep = -1;
	} else
	    state->ystep = 1;

	state->state = 1;

    }

    if (bytes < state->bytes) {
	state->errcode = IMAGING_CODEC_MEMORY;
	return 0;
    }

    ptr = buf;

    while (bytes >= state->bytes) {

	state->shuffle(ptr, (UINT8*) im->image[state->y + state->yoff] +
		       state->xoff * im->pixelsize, state->xsize);

	ptr += state->bytes;
	bytes -= state->bytes;

	state->y += state->ystep;

	if (state->y < 0 || state->y >= state->ysize) {
	    state->errcode = IMAGING_CODEC_END;
	    break;
	}

    }

    return ptr - buf;

}
