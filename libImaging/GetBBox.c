/* 
 * The Python Imaging Library
 * $Id: GetBBox.c,v 1.1 1996/08/15 13:12:44 fredrik Exp $
 *
 * get bounding box for image
 *
 * history:
 *	96-07-22 fl:	Created
 *	96-12-30 fl:	Added projection stuff
 *
 * Copyright (c) Fredrik Lundh 1996.
 * Copyright (c) Secret Labs AB 1997.
 *
 * See the README file for details on usage and redistribution.
 */


#include "Imaging.h"


int
ImagingGetBBox(Imaging im, int bbox[4])
{
    /* Get the bounding box for any non-zero data in the image.*/

    int x, y;
    int has_data;

    /* Initialize bounding box to max values */
    bbox[0] = im->xsize;
    bbox[1] = -1;
    bbox[2] = bbox[3] = 0;

#define	GETBBOX(image, mask)\
    for (y = 0; y < im->ysize; y++) {\
	has_data = 0;\
	for (x = 0; x < im->xsize; x++)\
	    if (im->image[y][x] & mask) {\
		has_data = 1;\
		if (x < bbox[0])\
		    bbox[0] = x;\
		if (x >= bbox[2])\
		    bbox[2] = x+1;\
	    }\
	if (has_data) {\
	    if (bbox[1] < 0)\
		bbox[1] = y;\
	    bbox[3] = y+1;\
	}\
    }

    if (im->image8) {
	GETBBOX(image8, 0xff);
    } else {
	INT32 mask = 0xffffffff;
	if (im->bands == 3)
	    ((UINT8*) &mask)[3] = 0;
	GETBBOX(image32, mask);
    }

    /* Check that we got a box */
    if (bbox[1] < 0)
	return 0;

    return 1;
}


int
ImagingGetProjection(Imaging im, UINT8* xproj, UINT8* yproj)
{
    /* Get projection arrays for non-zero data in the image.*/

    int x, y;
    int has_data;

    /* Initialize projection arrays */
    memset(xproj, 0, im->xsize);
    memset(yproj, 0, im->ysize);

#define	GETPROJ(image, mask)\
    for (y = 0; y < im->ysize; y++) {\
	has_data = 0;\
	for (x = 0; x < im->xsize; x++)\
	    if (im->image[y][x] & mask) {\
		has_data = 1;\
		xproj[x] = 1;\
	    }\
	if (has_data)\
	    yproj[y] = 1;\
    }

    if (im->image8) {
	GETPROJ(image8, 0xff);
    } else {
	INT32 mask = 0xffffffff;
	if (im->bands == 3)
	    ((UINT8*) &mask)[3] = 0;
	GETPROJ(image32, mask);
    }

    return 1;
}
