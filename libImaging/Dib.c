/*
 * The Python Imaging Library
 * $Id: Dib.c,v 1.4 1996/11/07 12:11:30 fredrik Exp $
 *
 * imaging display object for Windows
 *
 * history:
 *	96-05-12 fl:	Created
 *	96-05-17 fl:	Up and running
 *	96-05-21 fl:	Added palette stuff
 *	96-05-26 fl:	Added query palette and mode inquery
 *
 * Copyright (c) Fredrik Lundh 1996.  All rights reserved.
 */


#include "Imaging.h"

#ifdef WIN32

#include "ImDib.h"


char*
ImagingGetModeDIB(int size_out[2])
{
    /* Get device characteristics */

    HDC dc;
    char* mode;

    dc = CreateCompatibleDC(NULL);

    mode = "P";
    if (!(GetDeviceCaps(dc, RASTERCAPS) & RC_PALETTE)) {
	mode = "RGB";
	if (GetDeviceCaps(dc, BITSPIXEL) == 1)
	    mode = "1";
    }

    if (size_out) {
	size_out[0] = GetDeviceCaps(dc, HORZRES);
	size_out[1] = GetDeviceCaps(dc, VERTRES);
    }

    DeleteDC(dc);

    return mode;
}


ImagingDIB
ImagingNewDIB(const char *mode, int xsize, int ysize)
{
    /* Create a Windows bitmap */

    ImagingDIB dib;
    BITMAPINFOHEADER info;
    RGBQUAD palette[256];
    int i;

    /* Check mode */
    if (strcmp(mode, "1") != 0 && strcmp(mode, "L") != 0 &&
	strcmp(mode, "RGB") != 0)
	return (ImagingDIB) ImagingError_Argument("bad mode");

    memset(&info, 0, sizeof(info));

    info.biSize = sizeof(info);
    info.biWidth = xsize;
    info.biHeight = ysize;
    info.biPlanes = 1;
    info.biBitCount = strlen(mode)*8;
    info.biCompression = BI_RGB;

    /* Create DIB context */
    dib = (ImagingDIB) malloc(sizeof(*dib));
    if (!dib)
	return (ImagingDIB) ImagingError_NoMemory();

    /* Create DIB */
    dib->dc = CreateCompatibleDC(NULL);

    if (!dib->dc) {
	free(dib);
	return (ImagingDIB) ImagingError_NoMemory();
    }

    if (strcmp(mode, "P") == 0)
	dib->bitmap = CreateDIBSection(dib->dc, (BITMAPINFO*) &info,
				       DIB_PAL_COLORS, &dib->bits, NULL, 0);
    else
	dib->bitmap = CreateDIBSection(dib->dc, (BITMAPINFO*) &info,
				       DIB_RGB_COLORS, &dib->bits, NULL, 0);

    if (!dib->bitmap) {
	free(dib);
	return (ImagingDIB) ImagingError_NoMemory();
    }

    strcpy(dib->mode, mode);
    dib->xsize = xsize;
    dib->ysize = ysize;

    dib->pixelsize = strlen(mode);
    dib->linesize = (xsize * dib->pixelsize + 3) & -4;

    if (dib->pixelsize == 1)
	dib->pack = dib->unpack = (ImagingShuffler) memcpy;
    else {
	dib->pack = ImagingPackBGR;
	dib->unpack = ImagingPackBGR;
    }

    /* Bind the DIB to the device context */
    SelectObject(dib->dc, dib->bitmap);

    /* Bind a palette to it as well (only required for 8-bit DIBs) */
    if (dib->pixelsize == 1) {
	for (i = 0; i < 256; i++) {
	    palette[i].rgbRed = 
	    palette[i].rgbGreen =
	    palette[i].rgbBlue = i;
	    palette[i].rgbReserved = 0;
        }
	SetDIBColorTable(dib->dc, 0, 256, palette);
    }

    /* Create an associated palette (for 8-bit displays only) */
    if (strcmp(ImagingGetModeDIB(NULL), "P") == 0) {

	char palbuf[sizeof(LOGPALETTE)+256*sizeof(PALETTEENTRY)];
	LPLOGPALETTE pal = (LPLOGPALETTE) palbuf;
	int i, r, g, b;

	/* Load system palette */
	pal->palVersion = 0x300;
	pal->palNumEntries = 256;
	GetSystemPaletteEntries(dib->dc, 0, 256, pal->palPalEntry);

	if (strcmp(mode, "L") == 0) {

	    /* Greyscale DIB.  Fill all 236 slots with a greyscale ramp
	     * (this is usually overkill on Windows since VGA only offers
	     * 6 bits greyscale resolution).  Ignore the slots already
	     * allocated by Windows */

	    i = 10;
	    for (r = 0; r < 236; r++) {
		pal->palPalEntry[i].peRed =
		pal->palPalEntry[i].peGreen =
	        pal->palPalEntry[i].peBlue = i;
		i++;
	    }

	    dib->palette = CreatePalette(pal);

	} else if (strcmp(mode, "RGB") == 0) {

#ifdef CUBE216

	    /* Colour DIB.  Create a 6x6x6 colour cube (216 entries) and
	     * add 20 extra greylevels for best result with greyscale
	     * images. */

	    i = 10;
	    for (r = 0; r < 256; r += 51)
		for (g = 0; g < 256; g += 51)
		    for (b = 0; b < 256; b += 51) {
			pal->palPalEntry[i].peRed = r;
			pal->palPalEntry[i].peGreen = g;
			pal->palPalEntry[i].peBlue = b;
			i++;
		    }
	    for (r = 1; r < 22-1; r++) {
		/* Black and white are already provided by the cube. */ 
		pal->palPalEntry[i].peRed =
		pal->palPalEntry[i].peGreen =
		pal->palPalEntry[i].peBlue = r * 255 / (22-1);
		i++;
	    }

#else

	    /* Colour DIB.  Alternate palette. */

	    i = 10;
	    for (r = 0; r < 256; r += 37)
		for (g = 0; g < 256; g += 32)
		    for (b = 0; b < 256; b += 64) {
			pal->palPalEntry[i].peRed = r;
			pal->palPalEntry[i].peGreen = g;
			pal->palPalEntry[i].peBlue = b;
			i++;
		    }

#endif

#if 1
	    {
		/* DEBUG: dump palette to file */
		FILE *err = fopen("dib.pal", "w");
		for (i = 0; i < 256; i++)
		    fprintf(err, "%d: %d/%d/%d\n", i,
			    pal->palPalEntry[i].peRed,
			    pal->palPalEntry[i].peGreen,
			    pal->palPalEntry[i].peBlue);
		fclose(err);
	    }
#endif

	    dib->palette = CreatePalette(pal);

	}

    }

    return dib;
}

void
ImagingPasteDIB(ImagingDIB dib, Imaging im, int x0, int y0, int x1, int y1)
{
    /* Paste image data into a bitmap */

    int y;
    for (y = 0; y < im->ysize; y++)
	dib->pack(dib->bits + dib->linesize*(dib->ysize-(y0+y)-1) +
		  x0*dib->pixelsize, im->image[y], im->xsize);

}

void
ImagingExposeDIB(ImagingDIB dib, int dc)
{
    /* Copy bitmap to display */

    if (dib->palette != 0)
	SelectPalette((HDC) dc, dib->palette, FALSE);
    BitBlt((HDC) dc, 0, 0, dib->xsize, dib->ysize, dib->dc, 0, 0, SRCCOPY);
}

int
ImagingQueryPaletteDIB(ImagingDIB dib, int dc)
{
    /* Install bitmap palette */

    int n;

    if (dib->palette != 0) {

	/* Realize associated palette */
	HPALETTE now = SelectPalette((HDC) dc, dib->palette, FALSE);
	n = RealizePalette((HDC) dc);

	/* Restore palette */
	SelectPalette((HDC) dc, now, FALSE);

    } else
	n = 0;

    return n; /* number of colours that was changed */
}

void
ImagingDeleteDIB(ImagingDIB dib)
{
    /* Clean up */

    if (dib->palette)
	DeleteObject(dib->palette);
    if (dib->bitmap)
	DeleteObject(dib->bitmap);
    if (dib->dc)
	DeleteDC(dib->dc);
}

#endif /* WIN32 */
