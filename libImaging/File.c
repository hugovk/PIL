/*
 * The Python Imaging Library
 * $Id: File.c,v 1.4 1996/11/07 12:11:30 fredrik Exp $
 *
 * built-in image file handling
 *
 * history:
 *	95-11-26 fl:	Created, supports PGM/PPM and PyPPM only
 *	96-08-07 fl:	Write "1" images as PGM
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


#include <ctype.h>		/* isspace(), isdigit() */
#include "Imaging.h"


Imaging
ImagingOpenPPM(const char* infile)
{
    FILE* fp;
    int i, c, v;
    char* mode;
    unsigned x, y, max;
    Imaging im;

    if (!infile)
	return ImagingError_Argument(NULL);

    fp = fopen(infile, "rb");
    if (!fp)
	return ImagingError_IOError();

    /* PPM magic */
    if (fgetc(fp) != 'P')
	goto syntax;
    switch (fgetc(fp)) {
    case '4': /* FIXME: 1-bit images are not yet supported */
	goto syntax;
    case '5':
	mode = "L";
	break;
    case '6':
	mode = "RGB";
	break;
    case 'y': /* FIXME: PyPPM images are not yet supported */
	goto syntax;
    default:
	goto syntax;
    }

    i = 0;
    c = fgetc(fp);

    x = y = max = 0;

    while (i < 3) {	

	/* Ignore optional comment fields */
	while (c == '\n') {
	    c = fgetc(fp);
	    if (c == '#') {
		do {
		    c = fgetc(fp);
		    if (c == EOF)
			goto syntax;
		} while (c != '\n');
		c = fgetc(fp);
	    }
	}

	/* Skip forward to next value */
	while (isspace(c))
	    c = fgetc(fp);

	/* And parse it */
	v = 0;
	while (isdigit(c)) {
	    v = v * 10 + (c - '0');
	    c = fgetc(fp);
	}

	if (c == EOF)
	    goto syntax;

	switch (i++) {
	case 0:
	    x = v;
	    break;
	case 1:
	    y = v;
	    break;
	case 2:
	    max = v;
	    break;
	}
    }

    im = ImagingNew(mode, x, y);
    if (!im)
	return NULL;

    /* if (max != 255) ... FIXME: does anyone ever use this feature? */

    if (strcmp(im->mode, "L") == 0) {

	/* PPM "L" */
	for (y = 0; y < im->ysize; y++)
	    fread(im->image[y], 1, im->xsize, fp);

    } else {

	/* PPM "RGB" or PyPPM mode */
	for (y = 0; y < im->ysize; y++)
	    for (x = i = 0; x < im->xsize; x++, i += im->pixelsize)
		fread(im->image[y]+i, 1, im->bands, fp);

    }

    fclose(fp);

    return im;

syntax:
    fclose(fp);
    return ImagingError_IOError();
}


void
ImagingSaveRaw(Imaging im, FILE* fp)
{
    unsigned x, y, i;

    if (strcmp(im->mode, "L") == 0) {

	/* PPM "L" */
	for (y = 0; y < im->ysize; y++)
	    fwrite(im->image[y], 1, im->xsize, fp);

    } else {

	/* PPM "RGB" or PyPPM */
	for (y = 0; y < im->ysize; y++)
	    for (x = i = 0; x < im->xsize; x++, i += im->pixelsize)
		fwrite(im->image[y]+i, 1, im->bands, fp);

    }
}


void
ImagingSavePPM(Imaging im, const char* outfile)
{
    FILE* fp;

    if (!im) {
	ImagingError_Argument(NULL);
	return;
    }

    fp = fopen(outfile, "wb");
    if (!fp) {
	ImagingError_IOError();
	return;
    }

    if (strcmp(im->mode, "1") == 0 || strcmp(im->mode, "L") == 0) {

	/* Write "PGM" */
	fprintf(fp, "P5\n%d %d\n255\n",
		im->xsize, im->ysize);

    } else {

	if (strcmp(im->mode, "RGB") == 0)
	    /* Write "PPM" */
	    fprintf(fp, "P6\n%d %d\n255\n",
		    im->xsize, im->ysize);
	else
	    /* Write "PyPPM" file */
	    fprintf(fp, "Py%s\n%d %d\n255\n",
		    im->mode, im->xsize, im->ysize);

    }

    ImagingSaveRaw(im, fp);

    fclose(fp);
}

