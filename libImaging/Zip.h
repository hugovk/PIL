/* $Id: Zip.h,v 1.1 1996/12/16 18:51:03 fl Exp fl $
 *
 * The Python Imaging Library.
 *
 * File:
 *	Zip.h -- Declarations for the ZIP codecs
 *
 * Copyright (c) Fredrik Lundh 1996.  All rights reserved.
 */


#include "zlib.h"


typedef struct {

    /* CONFIGURATION */


    /* PRIVATE CONTEXT (set by decoder) */

    z_stream d_stream;		/* decompression stream */

    UINT8 *previous;		/* previous line */

} ZIPSTATE;
