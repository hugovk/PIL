/*
 * The Python Imaging Library.
 * $Id: Jpeg.h,v 1.2 1996/08/15 13:12:44 fredrik Exp $
 *
 * declarations for the IJG JPEG codec interface.
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */

#include "jpeglib.h"

#include <setjmp.h>


typedef struct {
    struct jpeg_error_mgr pub;	/* "public" fields */
    jmp_buf setjmp_buffer;	/* for return to caller */
} JPEGERROR;


/* -------------------------------------------------------------------- */
/* Decoder								*/

typedef struct {
    struct jpeg_source_mgr pub;
    int skip;
} JPEGSOURCE;

typedef struct {

    /* CONFIGURATION */

    /* Raw mode (may differ from real raw mode) */
    char rawmode[8+1];

    /* If set, trade quality for speed */
    int draft;

    /* Scale factor (1, 2, 4, 8) */
    int scale;

    /* PRIVATE CONTEXT (set by decoder) */

    struct jpeg_decompress_struct cinfo;

    JPEGERROR error;

    JPEGSOURCE source;

} JPEGSTATE;


/* -------------------------------------------------------------------- */
/* Encoder								*/

typedef struct {
    struct jpeg_destination_mgr pub;
    /* might add something some other day */
} JPEGDESTINATION;

typedef struct {

    /* CONFIGURATION */

    /* Quality (1-100, 0 means default) */
    int quality;

    /* Progressive mode */
    int progressive;

    /* Smoothing factor (1-100, 0 means none) */
    int smooth;

    /* Optimize Huffman tables (slow) */
    int optimize;

    /* Stream type (0=interchange, 1=tables only, 2=image only) */
    int streamtype;

    /* PRIVATE CONTEXT (set by encoder) */

    struct jpeg_compress_struct cinfo;

    JPEGERROR error;

    JPEGDESTINATION destination;

} JPEGENCODERSTATE;
