/* $Id: ImDib.h,v 1.3 1996/08/15 13:12:44 fredrik Exp $
 *
 * The Python Imaging Library
 *
 * File:
 *	ImDib.h -- Windows DIB specifics
 *
 * Copyright (c) Fredrik Lundh 1996.  All rights reserved.
 */


#ifdef WIN32

#include <windows.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct ImagingDIBInstance {
    /* Windows interface */
    HDC dc;
    HBITMAP bitmap;
    UINT8 *bits;
    HPALETTE palette;
    /* Used by cut and paste */
    char mode[4];
    int xsize, ysize;
    int pixelsize;
    int linesize;
    ImagingShuffler pack;
    ImagingShuffler unpack;
};

typedef struct ImagingDIBInstance* ImagingDIB;

extern char* ImagingGetModeDIB(int size_out[2]);

extern ImagingDIB ImagingNewDIB(const char *mode, int xsize, int ysize);

extern void ImagingDeleteDIB(ImagingDIB im);

extern void ImagingExposeDIB(ImagingDIB dib, int dc);

extern int ImagingQueryPaletteDIB(ImagingDIB dib, int dc);

extern void ImagingPasteDIB(ImagingDIB dib, Imaging im,
			    int x0, int y0, int x1, int y1);


#if defined(__cplusplus)
}
#endif

#endif
