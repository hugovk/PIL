/* 
 * The Python Imaging Library
 * $Id: GetBBox.c,v 1.2 1996/11/07 12:11:30 fredrik Exp $
 *
 * get bounding box for image
 *
 * history:
 *	96-07-22 fl:	Created
 *
 * Copyright (c) Fredrik Lundh 1996.  All rights reserved.
 */


#include "Imaging.h"


int
ImagingGetBBox(Imaging im, int bbox[4])
{
    /* Get the bounding box for any non-zero data in the image.
       FIXME: As it stands right now, also the fourth bogus band
       must be zero for a 3-band image. */

    int x, y;
    int has_data;

    /* Initialize bounding box to max values */
    bbox[0] = bbox[1] = -1;
    bbox[2] = bbox[3] = 0;

#define	GETBBOX(image)\
    for (y = 0; y < im->ysize; y++) {\
	has_data = 0;\
	for (x = 0; x < im->xsize; x++)\
	    if (im->image[y][x] != 0) {\
		has_data = 1;\
		if (bbox[0] < 0 || x < bbox[0])\
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

    if (im->image8)
	GETBBOX(image8)
    else
	GETBBOX(image32)

    /* Check that we got a box */
    if (bbox[0] < 0 || bbox[1] < 0)
	return 0;

    return 1;
}
