/* $Id: ImPlatform.h,v 1.2 1996/12/16 18:51:03 fl Exp fl $
 *
 * The Python Imaging Library
 *
 * File:
 *	ImPlatform.h -- platform declarations for the imaging core library
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


#include "ImConfig.h"


/* For now, we require an ANSI compliant compiler */
#if HAVE_PROTOTYPES == 0
#error Sorry, this library uses ANSI prototypes.
#endif
#if STDC_HEADERS == 0
#error Sorry, this library assumes ANSI header files.
#endif

#if SIZEOF_SHORT == 2
#define	INT16 short
#elif SIZEOF_INT == 2
#define	INT16 int
#else
#error Cannot find appropriate 16-bit integer type
#endif

#if SIZEOF_INT == 4
#define	INT32 int
#elif SIZEOF_LONG == 4
#define	INT32 long
#else
#error Cannot find appropriate 32-bit integer type
#endif

#define	UINT8 unsigned char

#define	UINT16 unsigned INT16
#define	UINT32 unsigned INT32
