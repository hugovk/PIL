/* $Id: Imaging.h,v 1.8 1996/12/16 18:51:03 fl Exp fl $
 *
 * The Python Imaging Library
 *
 * File:
 *	Imaging.h -- declarations for the imaging core library
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include <string.h>


#include "ImPlatform.h"


#if defined(__cplusplus)
extern "C" {
#endif


#ifndef M_PI
#define	M_PI	3.14159265359
#endif


/* -------------------------------------------------------------------- */

/*
 * Image data organization:
 *
 * mode	    bytes	byte order
 * -------------------------------
 * 1	    1		1
 * L	    1		L
 * P	    1		P
 * RGB	    4		R, G, B, -
 * RGBA	    4		R, G, B, A
 * CMYK	    4		C, M, Y, K
 *
 * "P" is an 8-bit palette mode, which should be mapped through the
 * palette member to get an output image.  Check palette->mode to
 * find the corresponding "real" mode.
 */


/* Handles */

typedef struct ImagingInstance *Imaging; /* a.k.a. ImagingImage :-) */
typedef struct ImagingHistogramInstance *ImagingHistogram;
typedef struct ImagingPaletteInstance *ImagingPalette;


struct ImagingInstance {

    /* Format */
    char mode[4+1];	/* Band names ("1", "L", "P", "RGB", "RGBA", "CMYK") */
    unsigned type;	/* Fixed to 0 in this version */
    unsigned depth;	/* Fixed to 8 in this version */
    unsigned bands;	/* Number of bands (1, 3, or 4) */
    unsigned xsize;	/* Image dimension. */
    unsigned ysize;

    /* Colour palette (for "P" images only) */
    ImagingPalette palette;

    /* Data pointers */
    UINT8 **image8;	/* Set for 8-bit image (pixelsize=1). */
    INT32 **image32;	/* Set for 32-bit image (pixelsize=4). */

    /* Internals */
    char **image;	/* Actual raster data. */
    char *block;	/* Set if data is allocated in a single block. */

    unsigned pixelsize;	/* Size of a pixel, in bytes (1 or 4) */
    unsigned linesize;	/* Size of a line, in bytes (xsize * pixelsize) */

    /* Virtual methods */
    void (*im_delete)(Imaging im);

};


struct ImagingHistogramInstance {

    /* Format */
    char mode[4+1];	/* Band names (of corresponding source image) */
    int bands;		/* Number of bands (1, 3, or 4) */

    /* Data */
    long *histogram;	/* Histogram (bands*256 longs) */

};


struct ImagingPaletteInstance {

    /* Format */
    char mode[4+1];	/* Band names */

    /* Data */
    UINT8 palette[1024];/* Palette data (same format as image data) */
    INT16* cache;	/* Palette cache (used for predefined palettes) */

};



/* Objects */
/* ------- */

extern Imaging ImagingNew(const char* mode, unsigned xsize, unsigned ysize);

extern Imaging ImagingNewBlock(const char* mode, unsigned xsize,
			       unsigned ysize);
extern Imaging ImagingNewArray(const char* mode, unsigned xsize,
			       unsigned ysize);
extern void ImagingCopyInfo(Imaging destination, Imaging source);
extern void ImagingDelete(Imaging im);

extern void ImagingHistogramDelete(ImagingHistogram histogram);

extern ImagingPalette ImagingPaletteNew(const char *mode);
extern ImagingPalette ImagingPaletteDuplicate(ImagingPalette palette);
extern void ImagingPaletteDelete(ImagingPalette palette);

extern int ImagingPaletteCachePrepare(ImagingPalette palette);
extern void ImagingPaletteCacheUpdate(ImagingPalette palette,
				      int r, int g, int b);
extern void ImagingPaletteCacheDelete(ImagingPalette palette);

#define	ImagingPaletteCache(p, r, g, b)\
    p->cache[(r>>2) + (g>>2)*64 + (b>>2)*64*64]

extern ImagingPalette ImagingQuantize(Imaging im, int colours);

/* Exceptions */
/* ---------- */

extern void* ImagingError_Argument(const char* message);
extern void* ImagingError_IOError(void);
extern void* ImagingError_Mismatch(void); /* maps to Argument */
extern void* ImagingError_NoMemory(void);


/* Image Manipulation Methods */
/* -------------------------- */

extern Imaging ImagingBlend(Imaging imIn1, Imaging imIn2, float alpha);
extern Imaging ImagingCopy(Imaging im);
extern Imaging ImagingConvert(Imaging im, const char* mode);
extern Imaging ImagingConvertMatrix(Imaging im, const char *mode, float m[]);
extern Imaging ImagingCrop(Imaging im, int x0, int y0, int x1, int y1);
extern Imaging ImagingFill(const char* mode, unsigned xsize, unsigned ysize,
			   void* colour);
extern Imaging ImagingFilter(Imaging im, const int* kernel);
extern Imaging ImagingFilterThin(Imaging im, int maxpass);
extern Imaging ImagingFillLinearGradient(const char* mode);
extern Imaging ImagingFillRadialGradient(const char* mode);
extern Imaging ImagingFlipLeftRight(Imaging im);
extern Imaging ImagingFlipTopBottom(Imaging im);
extern Imaging ImagingGetBand(Imaging im, int band);
extern int ImagingGetBBox(Imaging im, int bbox[4]);
extern ImagingHistogram ImagingHisto(Imaging im);
extern Imaging ImagingNegative(Imaging im);
extern Imaging ImagingOffset(Imaging im, int xoffset, int yoffset);
extern int ImagingPaste(Imaging into, Imaging im, Imaging mask,
			int x0, int y0, int x1, int y1);
extern Imaging ImagingPoint(Imaging im, const char* tablemode,
			    const UINT8* table);
extern int ImagingPutBand(Imaging im, Imaging imIn, int band);
extern Imaging ImagingResize(Imaging im, unsigned xsize, unsigned ysize);
extern Imaging ImagingRotate(Imaging im, double theta);
extern Imaging ImagingRotate90(Imaging im);
extern Imaging ImagingRotate180(Imaging im);
extern Imaging ImagingRotate270(Imaging im);
extern Imaging ImagingTransform(Imaging im, unsigned xsize, unsigned ysize,
				void (*transform)(double* X, double* Y,
						  int x, int y, void* data),
				void* transform_data,
				int fill);
extern Imaging ImagingTransformAffine(Imaging im, unsigned xsize,
				      unsigned ysize, double a[6],
				      int fill);

/* Standard Image Enhancement Filters */
extern Imaging ImagingFilterBlur(Imaging im);
extern Imaging ImagingFilterContour(Imaging im);
extern Imaging ImagingFilterDetail(Imaging im);
extern Imaging ImagingFilterEdgeEnhance(Imaging im);
extern Imaging ImagingFilterEdgeEnhanceMore(Imaging im);
extern Imaging ImagingFilterEmboss(Imaging im);
extern Imaging ImagingFilterFindEdges(Imaging im);
extern Imaging ImagingFilterSmooth(Imaging im);
extern Imaging ImagingFilterSmoothMore(Imaging im);
extern Imaging ImagingFilterSharpen(Imaging im);

/* Channel operations */
/* any mode */
extern Imaging ImagingChopLighter(Imaging imIn1, Imaging imIn2);
extern Imaging ImagingChopDarker(Imaging imIn1, Imaging imIn2);
extern Imaging ImagingChopDifference(Imaging imIn1, Imaging imIn2);
extern Imaging ImagingChopMultiply(Imaging imIn1, Imaging imIn2);
extern Imaging ImagingChopScreen(Imaging imIn1, Imaging imIn2);
extern Imaging ImagingChopAdd(Imaging imIn1, Imaging imIn2,
			      float scale, int offset);
extern Imaging ImagingChopSubtract(Imaging imIn1, Imaging imIn2,
				   float scale, int offset);
extern Imaging ImagingChopAddModulo(Imaging imIn1, Imaging imIn2);
extern Imaging ImagingChopSubtractModulo(Imaging imIn1, Imaging imIn2);

/* "1" images only */
extern Imaging ImagingChopAnd(Imaging imIn1, Imaging imIn2);
extern Imaging ImagingChopOr(Imaging imIn1, Imaging imIn2);
extern Imaging ImagingChopXor(Imaging imIn1, Imaging imIn2);

/* Image measurement */
extern void ImagingCrack(Imaging im, int x0, int y0);

/* Graphics */
struct ImagingAffineMatrixInstance {
    float a[9];
};

typedef struct ImagingAffineMatrixInstance *ImagingAffineMatrix;

extern int ImagingDrawPoint(Imaging im, int x, int y, int ink);
extern int ImagingDrawLine(Imaging im, int x0, int y0, int x1, int y1,
			   int ink);
extern int ImagingDrawRectangle(Imaging im, int x0, int y0, int x1, int y1,
				int ink, int fill);
extern int ImagingDrawPolygon(Imaging im, int points, int *xy,
			      int ink, int fill);

/* Experimental */
extern int ImagingToString(Imaging im, int lineno, char *buffer);
extern int ImagingFromString(Imaging im, int lineno, char *buffer);


/* File I/O */
/* -------- */

/* Built-in drivers */
extern Imaging ImagingOpenPPM(const char* filename);
extern void ImagingSavePPM(Imaging im, const char* filename);

/* Utility functions */
extern UINT32 ImagingCRC32(UINT32 crc, UINT8* buffer, int bytes);

/* Codecs */
typedef struct ImagingCodecStateInstance *ImagingCodecState;
typedef int (*ImagingCodec)(Imaging im, ImagingCodecState state,
			    UINT8* buffer, int bytes);

extern int ImagingEpsEncode(Imaging im, ImagingCodecState state,
			    UINT8* buffer, int bytes);
extern int ImagingGifDecode(Imaging im, ImagingCodecState state,
			    UINT8* buffer, int bytes);
extern int ImagingHexDecode(Imaging im, ImagingCodecState state,
			    UINT8* buffer, int bytes);
#ifdef	HAVE_LIBJPEG
extern int ImagingJpegDecode(Imaging im, ImagingCodecState state,
			     UINT8* buffer, int bytes);
extern int ImagingJpegEncode(Imaging im, ImagingCodecState state,
			     UINT8* buffer, int bytes);
#endif
extern int ImagingLzwDecode(Imaging im, ImagingCodecState state,
			    UINT8* buffer, int bytes);
#ifdef	HAVE_LIBMPEG
extern int ImagingMpegDecode(Imaging im, ImagingCodecState state,
			     UINT8* buffer, int bytes);
#endif
extern int ImagingPackbitsDecode(Imaging im, ImagingCodecState state,
				 UINT8* buffer, int bytes);
extern int ImagingPcdDecode(Imaging im, ImagingCodecState state,
			    UINT8* buffer, int bytes);
extern int ImagingPcxDecode(Imaging im, ImagingCodecState state,
			    UINT8* buffer, int bytes);
extern int ImagingRawDecode(Imaging im, ImagingCodecState state,
			    UINT8* buffer, int bytes);
extern int ImagingRawEncode(Imaging im, ImagingCodecState state,
			    UINT8* buffer, int bytes);
extern int ImagingXbmDecode(Imaging im, ImagingCodecState state,
			    UINT8* buffer, int bytes);
extern int ImagingXbmEncode(Imaging im, ImagingCodecState state,
			    UINT8* buffer, int bytes);
#ifdef	HAVE_LIBZ
extern int ImagingZipDecode(Imaging im, ImagingCodecState state,
			    UINT8* buffer, int bytes);
#endif

typedef void (*ImagingShuffler)(UINT8* out, const UINT8* in, int pixels);

extern void ImagingPackRGB(UINT8* out, const UINT8* in, int pixels);
extern void ImagingPackBGR(UINT8* out, const UINT8* in, int pixels);
extern void ImagingUnpackRGB(UINT8* out, const UINT8* in, int pixels);
extern void ImagingUnpackBGR(UINT8* out, const UINT8* in, int pixels);

extern ImagingShuffler ImagingFindUnpacker(const char* rawmode, int* bits_out);
extern ImagingShuffler ImagingFindPacker(const char* rawmode, int* bits_out);

struct ImagingCodecStateInstance {
    int count;
    int state;
    int errcode;
    int x, y;
    int ystep;
    int xsize, ysize, xoff, yoff;
    ImagingShuffler shuffle;
    int bits, bytes;
    UINT8 *buffer;
    void *context;
};

/* Errcodes */
#define	IMAGING_CODEC_END	 1
#define	IMAGING_CODEC_OVERRUN	-1
#define	IMAGING_CODEC_BROKEN	-2
#define	IMAGING_CODEC_CONFIG	-8
#define	IMAGING_CODEC_MEMORY	-9

#if defined(__cplusplus)
}
#endif
