/* $Id: ImConfig.h.in,v 1.1 1996/03/25 08:52:16 fredrik Exp fredrik $
 *
 * The Python Imaging Library.
 *
 * File:
 *	ImConfig.win -- ImConfig.h template for Windows (from ImConfig.h)
 *
 * Copyright (c) Fredrik Lundh 1995.  All rights reserved.
 */

/* -------------------------------------------------------------------- */

/* Define if you have the IJG jpeg library (-ljpeg).  */
#define HAVE_LIBJPEG

/* Define if you have the Greg Ward's mpeg library (-lmpeg).  */
#undef HAVE_LIBMPEG

/* Define if you have the zlib compression library (-lz).  */
#define HAVE_LIBZ

/* -------------------------------------------------------------------- */

/* Note: this configuration file was designed for Visual C++ 4.0 and
 * later.  It might need some tweaking to work with other compilers,
 * including 16-bit environments. */
#ifndef	WIN32
#define	WIN32
#endif

/* VC++ 4.0 is a bit annoying when it comes to precision issues (like
   claiming that "float a = 0.0;" would lead to loss of precision).  I
   don't like to see warnings from my code, but since I still want to
   keep it readable, I simply switch off a few warnings instead of adding
   the tons of casts that VC++ seem to require.  This code is compiled
   with numerous other compilers as well, so any real errors are likely
   to be catched anyway. */
#pragma warning(disable: 4244) /* conversion from 'float' to 'int' */
#pragma warning(disable: 4305) /* conversion from 'const int' to 'char' */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define as __inline if that's what the C compiler calls it.  */
#define inline __inline

/* Define if you have ANSI prototypes.  */
#define HAVE_PROTOTYPES 1

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

/* The number of bytes in a char.  */
#define SIZEOF_CHAR 1

/* The number of bytes in a double.  */
#define SIZEOF_DOUBLE 8

/* The number of bytes in a float.  */
#define SIZEOF_FLOAT 4

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* The number of bytes in a short.  */
#define SIZEOF_SHORT 2