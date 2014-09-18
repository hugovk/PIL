/* 
 * The Python Imaging Library
 * $Id$
 *
 * default exception handling
 *
 * description:
 * This code is usually overridden by application code.
 *
 * history:
 * 95-06-15 fl	Created
 * 98-12-29 fl	Minor tweaks
 *
 * Copyright (c) Fredrik Lundh 1995.
 * Copyright (c) Secret Labs AB 1997-98.
 *
 * See the README file for information on usage and redistribution.
 */


#include "Imaging.h"


void *
ImagingError_IOError(void)
{
    fprintf(stderr, "*** exception: file access error\n");
    return NULL;
}

void *
ImagingError_MemoryError(void)
{
    fprintf(stderr, "*** exception: out of memory\n");
    return NULL;
}

void *
ImagingError_ModeError(void)
{
    return ImagingError_ValueError("bad image mode");
    return NULL;
}

void *
ImagingError_Mismatch(void)
{
    return ImagingError_ValueError("images don't match");
    return NULL;
}

void *
ImagingError_ValueError(const char *message)
{
    if (!message)
	message = "exception: bad argument to function";
    fprintf(stderr, "*** %s\n", message);
    return NULL;
}

