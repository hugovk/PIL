/* $Id: tkImaging.c,v 1.1 1996/05/09 22:11:39 fredrik Exp $
 *
 * The Python Imaging Library.
 *
 * File:
 *	tkImaging.c -- a TK4 interface for Python Imaging objects
 *
 * Description:
 *	Copy (parts of) a named display memory to a photo image object.
 *	Also contains code to create an display memory.  Under Tk, a
 *	display memory is simply an "L" or "RGB" image memory that is
 *	allocated in a single block.
 *
 * 	To use this module, add the following line to your Tcl_AppInit
 *	function (in tkappinit.c):
 *	
 *		extern void TkImaging_Init(void);
 *		TkImaging_Init();
 *
 *	This registers a image type called "PyImagingCore", which is
 *	use to communicate between PIL and Tk's PhotoImage handler.
 *
 *	Compile and link tkImaging.c with tkappinit.c and _tkinter
 *	(see the Setup file for details on how to use tkappinit.c).
 *	Note that _tkinter.c must be compiled with WITH_APPINIT.
 *
 * History:
 *	95-09-12 fl	Created
 *	96-04-08 fl	Ready for release
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


#include <stdlib.h>

#include "tk.h"

#include "Imaging.h"


static Imaging
ImagingFind(const char* name)
{
    long id;

    /* Hack! */
    id = atol(name);
    if (!id)
	return NULL;

    return (Imaging) id;
}


static int
StringMatchImaging(char* string, char* format,
		   int* width_out, int* height_out)
{
    Imaging im;

    if (!format || strcmp(format, "PyImagingCore") != 0)
	return 0;

    /* Image */
    im = ImagingFind(string);
    if (!im || !im->block)
	return 0;

    *width_out = im->xsize;
    *height_out = im->ysize;

    return 1;
}


static int
StringReadImaging(Tcl_Interp* tcl, char* string,
		  char* format, Tk_PhotoHandle image,
		  int dest_xoffset, int dest_yoffset, int xsize,
		  int ysize, int src_xoffset, int src_yoffset)
{
    Imaging im;
    Tk_PhotoImageBlock block;

    /* Image */
    im = ImagingFind(string);
    if (!im) {
	Tcl_AppendResult(tcl, "Bad name", (char*) NULL);
	return TCL_ERROR;
    }
    if (!im->block) {
	Tcl_AppendResult(tcl, "Bad display memory", (char*) NULL);
	return TCL_ERROR;
    }

    /* Active region */
    if (src_xoffset + xsize > im->xsize)
	xsize = im->xsize - src_xoffset;
    if (src_yoffset + ysize > im->ysize)
	ysize = im->ysize - src_yoffset;
    if (xsize < 0 || ysize < 0
	|| src_xoffset >= im->xsize
	|| src_yoffset >= im->ysize)
	return TCL_OK;

    /* Mode */
    if (strcmp(im->mode, "1") == 0 || strcmp(im->mode, "L") == 0) {
	block.pixelSize = 1;
	block.offset[0] = block.offset[1] = block.offset[2] = 0;
    } else if (strcmp(im->mode, "RGB") == 0) {
	block.pixelSize = 4;
	block.offset[0] = 0;
	block.offset[1] = 1;
	block.offset[2] = 2;
    } else {
	Tcl_AppendResult(tcl, "Bad mode", (char*) NULL);
	return TCL_ERROR;
    }

    block.width = im->xsize;
    block.height = im->ysize;
    block.pitch = im->linesize;
    block.pixelPtr = (unsigned char*) im->block +
	             src_yoffset * im->linesize +
	             src_xoffset * im->pixelsize;

    /* Copy block to photo image, and leave the rest to TK */
    Tk_PhotoPutBlock(image, &block, dest_xoffset, dest_yoffset,
		     xsize, ysize);

    return TCL_OK;
}


/* -------------------------------------------------------------------- */
/* Install imaging photoimage handler  */

static Tk_PhotoImageFormat tkImaging = {
    "PyImagingCore",
    NULL,
    StringMatchImaging,
    NULL,
    StringReadImaging,
    NULL,
    NULL
};

void
ImagingTkInstall(void)
{
    /* Compatibility version */
    Tk_CreatePhotoImageFormat(&tkImaging);
}

void
TkImaging_Init(void)
{
    Tk_CreatePhotoImageFormat(&tkImaging);
}
