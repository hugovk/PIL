/*
 * The Python Imaging Library.
 * $Id: FliDecode.c,v 1.1 1997/01/14 16:40:04 fl Exp $
 *
 * decoder for FLI/FLC animations
 *
 * history:
 *	97-01-03 fl	Created
 *
 * Copyright (c) Fredrik Lundh 1997.  All rights reserved.
 */


#include "Imaging.h"


#define	I16(ptr)\
    ((ptr)[0] + ((ptr)[1] << 8))

#define	I32(ptr)\
    ((ptr)[0] + ((ptr)[1] << 8) + ((ptr)[2] << 16) + ((ptr)[3] << 24))


int
ImagingFliDecode(Imaging im, ImagingCodecState state, UINT8* buf, int bytes)
{
    UINT8* ptr;
    int framesize;
    int c, chunkcount;
    int i, x, y, ymax;

    ptr = buf;

    /* We don't decode anything unless we have a full frame in the
       input buffer (on the other hand, the Python part of the driver
       makes sure this is always the case) */
    framesize = I32(ptr);
    if (framesize < I32(ptr))
	return 0;

    /* Process the frame chunk */
    if (I16(ptr+4) != 0xF1FA) {
	state->errcode = IMAGING_CODEC_UNKNOWN;
	return -1;
    }
    chunkcount = I16(ptr+6);
    ptr += 16;

    /* Process subchunks */
    for (c = 0; c < chunkcount; c++) {
	UINT8 *data = ptr + 6;
	switch (I16(ptr+4)) {
	case 11:
	    /* COLOR chunk */
	    break; /* ignored; handled by Python code */
	case 12:
	    /* FLI DELTA chunk */
	    y = I16(data); ymax = y + I16(data+2);
	    data += 4;
	    while (y < ymax && y < state->ysize) {
		UINT8* buf = (UINT8*) im->image[y];
		int p, packets = *data++;
		for (p = x = 0; p < packets; p++, x += i) {
		    x += data[0];
		    if (data[1] >= 128) {
			i = 256-data[1];
			if (x + i > state->xsize)
			    break; /* safety first */
			memset(buf + x, data[2], i);
			data += 3;
		    } else if (data[1]) {
			i = data[1];
			if (x + i > state->xsize)
			    break; /* safety first */
			memcpy(buf + x, data + 2, i);
			data += i + 2;
		    } else {
			i = 0;
			data += 2;
		    }
		}
		if (p < packets)
		    break;
		y++;
	    }
	    if (y < ymax) {
		state->errcode = IMAGING_CODEC_OVERRUN;
		return -1;
	    }
	    break;
	case 13:
	    /* BLACK chunk */
	    for (y = 0; y < state->ysize; y++)
		memset(im->image[y], 0, state->xsize);
	    break;
	case 15:
	    /* BRUN chunk */
	    for (y = 0; y < state->ysize; y++) {
		UINT8* buf = (UINT8*) im->image[y];
		data += 1; /* skip packetcount byte */
		for (x = 0; x < state->xsize; x += i) {
		    if (data[0] >= 128) {
			i = 256 - data[0];
			if (x + i > state->xsize)
			    break; /* safety first */
			memcpy(buf + x, data + 1, i);
			data += i + 1;
		    } else if (data[0]) {
			i = data[0];
			if (x + i > state->xsize)
			    break; /* safety first */
			memset(buf + x, data[1], i);
			data += 2;
		    } else {
			i = 0;
			data++;
		    }
		}
		if (x != state->xsize) {
		    state->errcode = IMAGING_CODEC_OVERRUN;
		    return -1;
		}
	    }
	    break;
	case 16:
	    /* COPY chunk */
	    for (y = 0; y < state->ysize; y++) {
		UINT8* buf = (UINT8*) im->image[y];
		memcpy(buf+x, data, state->xsize);
		data += state->xsize;
	    }
	    break;
	case 18:
	    /* PSTAMP chunk */
	    break; /* ignored */
	default:
	    /* unknown chunk */
	    state->errcode = IMAGING_CODEC_UNKNOWN;
	    return -1;
	}
	ptr += I32(ptr);
    }

    return -1; /* end of frame */
}
