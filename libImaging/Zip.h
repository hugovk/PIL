/*
 * The Python Imaging Library.
 * $Id: Jpeg.h,v 1.2 1996/08/15 13:12:44 fredrik Exp $
 *
 * declarations for the ZIP codecs
 *
 * Copyright (c) Fredrik Lundh 1996.  All rights reserved.
 */


#include "zlib.h"


typedef struct {

    /* CONFIGURATION */

    /* Optimize (max compression) */
    int optimize;

    /* Palette mode (disables filtering) FIXME: generalize */
    int palette;

    /* Predefined dictionary (experimental) */
    char* dictionary;
    int dictionary_size;

    /* PRIVATE CONTEXT (set by decoder/encoder) */

    z_stream z_stream;		/* (de)compression stream */

    UINT8* previous;		/* previous line (allocated) */

    /* Compressor specific stuff */
    UINT8* prior;		/* filter storage (allocated) */
    UINT8* up;
    UINT8* average;
    UINT8* paeth;

    UINT8* output;		/* output data */

} ZIPSTATE;
