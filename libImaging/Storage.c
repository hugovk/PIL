/*
 * The Python Imaging Library
 * $Id: Storage.c,v 1.3 1996/05/09 21:20:18 fredrik Exp $
 *
 * imaging storage object
 *
 * description:
 *	This baseline core implementation is designed to efficiently
 *	handle large images, provided they fit into the available
 *	memory.
 *
 * history:
 *	95-06-15 fl:	Created
 *	95-09-12 fl:	Updated API, compiles silently under ANSI C++
 *	95-11-26 fl:	Compiles silently under Borland 4.5 as well
 *	96-05-05 fl:	Correctly test status from Prologue
 *	97-05-12 fl:	Increased THRESHOLD (to speed up Tk interface)
 *	97-05-30 fl:	Added support for floating point images
 *	97-11-17 fl:	Added support for RGBX images
 *	98-01-11 fl:	Added support for integer images
 *	98-03-05 fl:	Exported Prologue/Epilogue functions
 *
 * Copyright (c) Fredrik Lundh 1995-97.
 * Copyright (c) Secret Labs AB 1998.
 *
 * See the README file for information on usage and redistribution.
 */


#include "Imaging.h"


/* --------------------------------------------------------------------
 * Standard image object.
 */

Imaging
ImagingNewPrologue(const char *mode, unsigned xsize, unsigned ysize)
{
    Imaging im;

    im = (Imaging) calloc(1, sizeof(struct ImagingInstance));
    if (!im)
	return (Imaging) ImagingError_NoMemory();

    if (strcmp(mode, "1") == 0) {
        /* 1-bit images */
        im->bands = im->pixelsize = 1;
        im->depth = 8;
    } else if (strcmp(mode, "P") == 0) {
        /* 8-bit palette mapped images */
        im->bands = im->pixelsize = 1;
        im->depth = 8;
    } else if (strcmp(mode, "L") == 0) {
        /* 8-bit greyscale (luminance) images */
        im->bands = im->pixelsize = 1;
        im->depth = 8;
    } else if (strcmp(mode, "F") == 0) {
        /* 32-bit floating point images */
        im->bands = 1;
        im->pixelsize = 4;
        im->type = 2;
        im->depth = 32;
    } else if (strcmp(mode, "I") == 0) {
        /* 32-bit integer images */
        im->bands = 1;
        im->pixelsize = 4;
        im->type = 1;
        im->depth = 32;
    } else if (strcmp(mode, "RGB") == 0) {
        /* 24-bit true colour images */
        im->bands = 3;
        im->pixelsize = 4;
        im->depth = 8;
    } else if (strcmp(mode, "RGBX") == 0) {
        /* 32-bit true colour images with padding */
        im->bands = im->pixelsize = 4;
        im->depth = 8;
    } else if (strcmp(mode, "RGBA") == 0) {
        /* 32-bit true colour images with alpha */
        im->bands = im->pixelsize = 4;
        im->depth = 8;
    } else if (strcmp(mode, "CMYK") == 0) {
        /* 32-bit colour separation */
        im->bands = im->pixelsize = 4;
        im->depth = 8;
    } else {
        free(im);
	return (Imaging) ImagingError_Argument(
            "Unsupported mode");
    }

    /* Setup image descriptor */
    strcpy(im->mode, mode);

    im->xsize = xsize;
    im->ysize = ysize;

    im->linesize = xsize * im->pixelsize;

    /* Pointer array */
    im->image = (char **) calloc(ysize, sizeof(void *));
    if (!im->image) {
	free(im);
	return (Imaging) ImagingError_NoMemory();
    }

    return im;
}

Imaging
ImagingNewEpilogue(Imaging im)
{
    /* If the raster data allocator didn't setup a destructor,
       assume that it couldn't allocate the required amount of
       memory. */
    if (!im->im_delete)
	return (Imaging) ImagingError_NoMemory();

    /* Initialize alias pointers to pixel data. */
    switch (im->pixelsize) {
    case 1:
	im->image8 = (UINT8 **) im->image;
	break;
    case 4:
	im->image32 = (INT32 **) im->image;
	break;
    }

    return im;
}

void
ImagingDelete(Imaging im)
{
    if (!im)
	return;

    if (im->palette)
	ImagingPaletteDelete(im->palette);

    if (im->im_delete)
	im->im_delete(im);

    if (im->image)
	free(im->image);

    free(im);
}


/* Array Storage Type */
/* ------------------ */
/* Allocate image as an array of line buffers. */

static void
ImagingDeleteArray(Imaging im)
{
    int y;

    if (im->image)
	for (y = 0; y < im->ysize; y++)
	    if (im->image[y])
		free(im->image[y]);
}

Imaging
ImagingNewArray(const char *mode, int xsize, int ysize)
{
    Imaging im;
    int y;

    im = ImagingNewPrologue(mode, xsize, ysize);
    if (!im)
	return NULL;

    /* Allocate image as an array of lines */
    for (y = 0; y < im->ysize; y++) {
	im->image[y] = (char *) malloc(im->linesize);
	if (!im->image[y]) {
	    ImagingDeleteArray(im);
	    break;
	}
    }

    if (y == im->ysize)
	im->im_delete = ImagingDeleteArray;

    return ImagingNewEpilogue(im);
}


/* Block Storage Type */
/* ------------------ */
/* Allocate image as a single block. */

static void
ImagingDeleteBlock(Imaging im)
{
    if (im->block)
	free(im->block);
}

Imaging
ImagingNewBlock(const char *mode, int xsize, int ysize)
{
    Imaging im;
    int y, i;

    im = ImagingNewPrologue(mode, xsize, ysize);
    if (!im)
	return NULL;

    /* Use a single block */
    im->block = (char *) malloc(im->ysize * im->linesize);

    if (im->block) {

	for (y = i = 0; y < im->ysize; y++) {
	    im->image[y] = im->block + i;
	    i += im->linesize;
	}

	im->im_delete = ImagingDeleteBlock;

    }

    return ImagingNewEpilogue(im);
}

/* --------------------------------------------------------------------
 * Create a new, internally allocated, image.
 */

/* FIXME: should check for 16-bit environments, not just "MSDOS" */
#if defined(MSDOS)
#define	THRESHOLD	16384L
#else
#define	THRESHOLD	1048576L
#endif

Imaging
ImagingNew(const char* mode, int xsize, int ysize)
{
    if ((long) xsize * ysize * strlen(mode) <= THRESHOLD)
	return ImagingNewBlock(mode, xsize, ysize);
    else
	return ImagingNewArray(mode, xsize, ysize);
}

Imaging
ImagingNew2(const char* mode, Imaging imOut, Imaging imIn)
{
    /* allocate or validate output image */

    if (imOut) {
        /* make sure images match */
        if (strcmp(imOut->mode, mode) != 0
            || imOut->xsize != imIn->xsize
            || imOut->ysize != imIn->ysize) {
            ImagingError_Mismatch();
            return NULL;
        }
    } else {
        /* create new image */
        imOut = ImagingNew(mode, imIn->xsize, imIn->ysize);
        if (!imOut)
            return NULL;
    }

    return imOut;
}

void
ImagingCopyInfo(Imaging destination, Imaging source)
{
    if (source->palette)
	destination->palette = ImagingPaletteDuplicate(source->palette);
}
