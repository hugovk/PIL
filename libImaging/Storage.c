/*
 * The Python Imaging Library
 * $Id: Storage.c,v 1.5 1996/12/16 18:51:03 fl Exp fl $
 *
 * imaging storage object
 *
 * description:
 *	This baseline core implementation is designed to efficiently
 *	handle large images, provided they fit into the available
 *	memory.
 *
 * to do:
 *	FIXME: A better block/array threshold is needed.
 *	FIXME: Implement memory mapping schemes (Win32, Unix)
 *
 * history:
 *	95-06-15 fl:	Created
 *	95-09-12 fl:	Updated API, compiles silently under ANSI C++
 *	95-11-26 fl:	Compiles silently under Borland 4.5 as well
 *	96-05-05 fl:	Correctly test status from Prologue
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


#include "Imaging.h"


/* --------------------------------------------------------------------
 * Standard image object.
 */

static Imaging
ImagingNewPrologue(const char *mode, unsigned xsize, unsigned ysize)
{
    Imaging im;
    unsigned bands;

    bands = strlen(mode);
    if (bands != 1 && bands != 3 && bands != 4)
	return (Imaging) ImagingError_Argument("Illegal mode");

    im = (Imaging) calloc(1, sizeof(struct ImagingInstance));
    if (!im)
	return (Imaging) ImagingError_NoMemory();

    /* Setup image descriptor */
    strcpy(im->mode, mode);
    im->xsize = xsize;
    im->ysize = ysize;

    im->type  = 0;
    im->depth = 8;
    im->bands = bands;

    /* Some useful values */
    im->pixelsize = (bands != 3) ? bands : 4;
    im->linesize = xsize * im->pixelsize;

    im->image = (char **) calloc(ysize, sizeof(void *));
    if (!im->image) {
	free(im);
	return (Imaging) ImagingError_NoMemory();
    }
    return im;
}

static Imaging
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
    unsigned y;

    if (im->image)
	for (y = 0; y < im->ysize; y++)
	    if (im->image[y])
		free(im->image[y]);
}

Imaging
ImagingNewArray(const char *mode, unsigned xsize, unsigned ysize)
{
    Imaging im;
    unsigned y;

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
ImagingNewBlock(const char *mode, unsigned xsize, unsigned ysize)
{
    Imaging im;
    unsigned y, i;

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
#define	THRESHOLD	10000L
#else
#define	THRESHOLD	65536L
#endif

Imaging
ImagingNew(const char *mode, unsigned xsize, unsigned ysize)
{
    if ((long) xsize * ysize * strlen(mode) <= THRESHOLD)
	return ImagingNewBlock(mode, xsize, ysize);
    else
	return ImagingNewArray(mode, xsize, ysize);
}

void
ImagingCopyInfo(Imaging destination, Imaging source)
{
    if (source->palette)
	destination->palette = ImagingPaletteDuplicate(source->palette);
}
