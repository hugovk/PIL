/*
 * The Python Imaging Library.
 * $Id: RawEncode.c,v 1.2 1996/08/15 13:12:44 fredrik Exp $
 *
 * coder for uncompressed GIF data
 *
 * notes:
 *	For legal reasons, this encoder produces "uncompressed"
 *	GIF files, instead of using LZW compression. Such files
 *	are about 13% larger than the raw data itself.
 *
 * history:
 *	97-01-05 fl	created
 *	97-08-27 fl	fixed off-by-one error in buffer size test
 *
 * Copyright (c) Fredrik Lundh 1997.
 * Copyright (c) Secret Labs AB 1997.
 *
 * See the README file for information on usage and redistribution.
 */


#include "Imaging.h"

#include "Gif.h"


int
ImagingGifEncode(Imaging im, ImagingCodecState state, UINT8* buf, int bytes)
{
    UINT8* ptr;
    int code;

    GIFENCODERSTATE *context = (GIFENCODERSTATE*) state->context;

    if (!state->state) {

	/* Initialize decoder */
	state->state = 1;

	context->blocksize = 1; /* count byte */

	/* Place a clear code in the output buffer */
	state->count = 0;
	context->bitbuffer = 256;
	context->bitcount = 9;

    }

    ptr = buf;

    for (;;)

	/* yet another strange state machine */
	switch (state->state) {
	case 1:

	    /* check if block buffer must be flushed */
	    if (context->blocksize == 256) {
		state->state = 2;
		break;
	    }

	    /* copy bytes from the bit buffer to block buffer */
	    if (context->bitcount >= 8) {

		context->block[context->blocksize++] =
		    (UINT8) context->bitbuffer;

		context->bitbuffer >>= 8;
		context->bitcount -= 8;

		break;

	    }

	    /* get a code to insert into the bit buffer */
	    if (context->eof) {

		/* end of image data */
		if (context->eof > 1) {

		    /* flush bit and block buffers */
		    if (context->bitcount > 0) {

			/* flush bit buffer */
			context->block[context->blocksize++] =
			    (UINT8) context->bitbuffer;
			context->bitcount = 0;

		    } else {

			/* flush block buffer and exit */
			context->eof = 3; /* don't come back here */
			state->state = 2;

		    }

		    break;

		}

		/* write an end code */
		code = 257; /* end code for 8-bit data */
		context->eof = 2; /* don't come back here */

	    } else if (state->count == 256-2) {

		/* write a clear code */
		code = 256; /* clear code for 8-bit data */
		state->count = 0; /* reset counter */

	    } else {

		/* get a pixel code */
		if (state->x == 0 || state->x >= state->xsize) {

		    /* get another line of data */
		    if (state->y >= state->ysize) {
			/* end of data */
			context->eof = 1;
			break;

		    }

		    /* get data from the image */
		    state->shuffle(state->buffer,
				   (UINT8*) im->image[state->y + state->yoff] +
				   state->xoff * im->pixelsize, state->xsize);

		    state->x = 0;
		    state->y++;

		}

		code = state->buffer[state->x++];

	    }

	    /* add symbol to bitbuffer */
	    context->bitbuffer |= ((INT32) code) << context->bitcount;
	    context->bitcount += 9;

	    state->count++;

	    break;

	case 2:

	    /* flush block buffer (only if non-empty) */
	    if (context->blocksize > 1) {

		/* check if it fits into output buffer */
		if (bytes <= context->blocksize)
		    return ptr - buf;

		ptr[0] = context->blocksize-1;
		memcpy(ptr+1, context->block+1, context->blocksize);

		ptr += context->blocksize;
		bytes -= context->blocksize;

		context->blocksize = 1;

	    }

	    if (context->eof == 3) {
		/* this was the last block; terminate encoding */
		state->errcode = IMAGING_CODEC_END;
		return ptr - buf;
	    }

	    state->state = 1;

	    break;

	}
}
