/* 
 * The Python Imaging Library
 * $Id: Except.c,v 1.4 1996/11/07 12:11:30 fredrik Exp $
 *
 * default exception handling
 *
 * description:
 *	This code is usually overridden by application code.
 *
 * history:
 *	95-06-15 fl:	Created
 *
 * Copyright (c) Fredrik Lundh 1995.  All rights reserved.
 */


#include "Imaging.h"


void *
ImagingError_Argument(const char *message)
{
    if (!message)
	message = "illegal argument exception";
    fprintf(stderr, "*** %s\n", message);
    return NULL;
}

void *
ImagingError_IOError(void)
{
    fprintf(stderr, "*** I/O error exception\n");
    return NULL;
}

void *
ImagingError_Mismatch(void)
{
    return ImagingError_Argument("image mismatch");
    return NULL;
}

void *
ImagingError_NoMemory(void)
{
    fprintf(stderr, "*** out of memory exception\n");
    return NULL;
}
