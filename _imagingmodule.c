/* $Id: _imagingmodule.c,v 1.6 1996/10/04 19:40:33 fredrik Exp $
 *
 * The Python Imaging Library.
 *
 * File:
 *	_imagingmodule.c -- the imaging library bindings
 *
 * History:
 *	95-09-24 fl	Created
 *	96-03-24 fl	Ready for first public release (release 0.0)
 *	96-03-25 fl	Added fromstring (for Jack's "img" library)
 *	96-03-28 fl	Added channel operations
 *	96-03-31 fl	Added point operation
 *	96-04-08 fl	Added new/new_block/new_array factories
 *	96-04-13 fl	Added decoders
 *	96-05-04 fl	Added palette hack
 *	96-05-12 fl	Compile cleanly as C++
 *	96-05-19 fl	Added matrix conversions, gradient fills
 *	96-05-27 fl	Added display_mode, quantization
 *	96-07-22 fl	Added getbbox, offset
 *	96-07-23 fl	Added sequence semantics
 *	96-08-13 fl	Added logical operators, point mode
 *	96-08-16 fl	Modified paste interface
 *	96-09-06 fl	Added putdata methods, use abstract interface
 *	96-11-01 fl	Added xbm encoder
 *	96-11-04 fl	Added experimental path stuff, draw_lines, etc
 *	96-12-10 fl	Added zip decoder, crc32 interface
 *	96-12-14 fl	Added modulo arithmetics
 *	96-12-29 fl	Added zip encoder
 *	97-01-03 fl	Added fli and msp decoders
 *	97-01-04 fl	Added experimental sun_rle and tga_rle decoders
 *	97-01-05 fl	Added gif encoder, getpalette hack
 *	97-02-23 fl	Added histogram mask
 *	97-05-12 fl	Minor tweaks to match the IFUNC95 interface
 *	97-05-21 fl	Added noise generator, spread effect
 *	97-06-05 fl	Added mandelbrot generator
 *	97-08-02 fl	Modified putpalette to coerce image mode if necessary
 *	98-01-11 fl	Added INT32 support
 *	98-01-22 fl	Fixed draw_points to draw the last point too
 *
 * Copyright (c) Secret Labs AB 1997-98.
 * Copyright (c) Fredrik Lundh 1995-97.
 *
 * See the README file for information on usage and redistribution.
 */


#include "Python.h"

#include "Imaging.h"


/* Configuration stuff. Feel free to undef things you don't need. */
#define WITH_IMAGECHOPS
#define	WITH_IMAGEDRAW
#define	WITH_MAPPING
#define	WITH_LZW
#define	WITH_IMAGEPATH
#define	WITH_EFFECTS
#define	WITH_DEBUG

#undef	VERBOSE


/* -------------------------------------------------------------------- */
/* OBJECT ADMINISTRATION						*/
/* -------------------------------------------------------------------- */

typedef struct {
    PyObject_HEAD
    Imaging image;
} ImagingObject;

staticforward PyTypeObject Imaging_Type;

PyObject* 
PyImagingNew(Imaging imOut)
{
    ImagingObject* imagep;

    if (!imOut)
	return NULL;

    imagep = PyObject_NEW(ImagingObject, &Imaging_Type);
    if (imagep == NULL) {
	ImagingDelete(imOut);
	return NULL;
    }

#ifdef VERBOSE
    printf("imaging %p allocated\n", imagep);
#endif

    imagep->image = imOut;

    return (PyObject*) imagep;
}

static void
_dealloc(ImagingObject* imagep)
{

#ifdef VERBOSE
    printf("imaging %p deleted\n", imagep);
#endif

    ImagingDelete(imagep->image);
    PyMem_DEL(imagep);
}

#define PyImaging_Check(op) ((op)->ob_type == &Imaging_Type)

Imaging PyImaging_AsImaging(PyObject *op)
{
    if (!PyImaging_Check(op)) {
	PyErr_BadInternalCall();
	return NULL;
    }

    return ((ImagingObject *)op)->image;
}


/* -------------------------------------------------------------------- */
/* EXCEPTION REROUTING							*/
/* -------------------------------------------------------------------- */

void *
ImagingError_Argument(const char *message)
{
    PyErr_SetString(PyExc_ValueError,
		    (message) ? (char*) message : "PIL: illegal argument");
    return NULL;
}

void *
ImagingError_IOError(void)
{
    PyErr_SetString(PyExc_IOError,
		    "PIL: file I/O error");
    return NULL;
}

void *
ImagingError_Mismatch()
{
    PyErr_SetString(PyExc_ValueError,
		    "PIL: images do not match");
    return NULL;
}

void *
ImagingError_NoMemory(void)
{
    return PyErr_NoMemory();
}


/* -------------------------------------------------------------------- */
/* FACTORIES								*/
/* -------------------------------------------------------------------- */

static PyObject* 
_fill(PyObject* self, PyObject* args)
{
    char* mode;
    int xsize, ysize;
    int colour;
    char buffer[4];

    xsize = ysize = 256;
    colour = 0;

    if (!PyArg_ParseTuple(args, "s|(ii)i", &mode, &xsize, &ysize, &colour))
	return NULL;

    /* Unpack integer value (note the order for multilayer images: ABGR)*/
    buffer[0] = (UINT8) colour;
    buffer[1] = (UINT8) (colour >> 8);
    buffer[2] = (UINT8) (colour >> 16);
    buffer[3] = (UINT8) (colour >> 24);

    return PyImagingNew(ImagingFill(mode, xsize, ysize, buffer));
}

static PyObject* 
_new(PyObject* self, PyObject* args)
{
    char* mode;
    int xsize, ysize;

    if (!PyArg_ParseTuple(args, "s(ii)", &mode, &xsize, &ysize))
	return NULL;

    return PyImagingNew(ImagingNew(mode, xsize, ysize));
}

static PyObject* 
_new_array(PyObject* self, PyObject* args)
{
    char* mode;
    int xsize, ysize;

    if (!PyArg_ParseTuple(args, "s(ii)", &mode, &xsize, &ysize))
	return NULL;

    return PyImagingNew(ImagingNewArray(mode, xsize, ysize));
}

static PyObject* 
_new_block(PyObject* self, PyObject* args)
{
    char* mode;
    int xsize, ysize;

    if (!PyArg_ParseTuple(args, "s(ii)", &mode, &xsize, &ysize))
	return NULL;

    return PyImagingNew(ImagingNewBlock(mode, xsize, ysize));
}

static PyObject* 
_linear_gradient(PyObject* self, PyObject* args)
{
    char* mode;

    if (!PyArg_ParseTuple(args, "s", &mode))
	return NULL;

    return PyImagingNew(ImagingFillLinearGradient(mode));
}

static PyObject* 
_radial_gradient(PyObject* self, PyObject* args)
{
    char* mode;

    if (!PyArg_ParseTuple(args, "s", &mode))
	return NULL;

    return PyImagingNew(ImagingFillRadialGradient(mode));
}

static PyObject* 
_open_ppm(PyObject* self, PyObject* args)
{
    char* filename;

    if (!PyArg_ParseTuple(args, "s", &filename))
	return NULL;

    return PyImagingNew(ImagingOpenPPM(filename));
}

static PyObject* 
_blend(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep1;
    ImagingObject* imagep2;
    double alpha;
    
    alpha = 0.5;
    if (!PyArg_ParseTuple(args, "O!O!|d",
			  &Imaging_Type, &imagep1,
			  &Imaging_Type, &imagep2,
			  &alpha))
	return NULL;

    return PyImagingNew(ImagingBlend(imagep1->image, imagep2->image,
				     (float) alpha));
}

/* -------------------------------------------------------------------- */
/* METHODS								*/
/* -------------------------------------------------------------------- */

static PyObject* 
_convert(ImagingObject* self, PyObject* args)
{
    char* mode;

    if (!PyArg_ParseTuple(args, "s", &mode))
	return NULL;

    return PyImagingNew(ImagingConvert(self->image, mode, NULL));
}

static PyObject* 
_convert2(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep1;
    ImagingObject* imagep2;
    
    if (!PyArg_ParseTuple(args, "O!O!",
			  &Imaging_Type, &imagep1,
			  &Imaging_Type, &imagep2))
	return NULL;

    if (!ImagingConvert2(imagep1->image, imagep2->image))
        return NULL;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* 
_convert_matrix(ImagingObject* self, PyObject* args)
{
    char* mode;
    float m[12];

    if (!PyArg_ParseTuple(args, "s(ffff)", &mode, m+0, m+1, m+2, m+3)) {
	PyErr_Clear();
	if (!PyArg_ParseTuple(args, "s(ffffffffffff)", &mode,
			      m+0, m+1, m+2, m+3,
			      m+4, m+5, m+6, m+7,
			      m+8, m+9, m+10, m+11))
	    return NULL;
    }

    return PyImagingNew(ImagingConvertMatrix(self->image, mode, m));
}

static PyObject* 
_copy(ImagingObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args, ""))
	return NULL;

    return PyImagingNew(ImagingCopy(self->image));
}

static PyObject* 
_copy2(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep1;
    ImagingObject* imagep2;
    
    if (!PyArg_ParseTuple(args, "O!O!",
			  &Imaging_Type, &imagep1,
			  &Imaging_Type, &imagep2))
	return NULL;

    if (!ImagingCopy2(imagep1->image, imagep2->image))
        return NULL;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* 
_crop(ImagingObject* self, PyObject* args)
{
    int x0, y0, x1, y1;

    if (!PyArg_ParseTuple(args, "(iiii)", &x0, &y0, &x1, &y1))
	return NULL;

    return PyImagingNew(ImagingCrop(self->image, x0, y0, x1, y1));
}

static PyObject* 
_filter(ImagingObject* self, PyObject* args)
{
    int id;
    Imaging im;

    if (!PyArg_ParseTuple(args, "i", &id))
	return NULL;

    /* FIXME: replace with user-defined filter kernels */
    switch (id) {
      case 0:
	im = ImagingFilterBlur(self->image);
	break;
      case 1:
	im = ImagingFilterContour(self->image);
	break;
      case 2:
	im = ImagingFilterDetail(self->image);
	break;
      case 3:
	im = ImagingFilterEdgeEnhance(self->image);
	break;
      case 4:
	im = ImagingFilterEdgeEnhanceMore(self->image);
	break;
      case 5:
	im = ImagingFilterEmboss(self->image);
	break;
      case 6:
	im = ImagingFilterFindEdges(self->image);
	break;
      case 7:
	im = ImagingFilterSmooth(self->image);
	break;
      case 8:
	im = ImagingFilterSmoothMore(self->image);
	break;
      case 9:
	im = ImagingFilterSharpen(self->image);
	break;
      default:
	PyErr_SetString(PyExc_ValueError, "no such builtin kernel");
	return NULL;
    }

    return PyImagingNew(im);
}

static PyObject* 
_getpalette(ImagingObject* self, PyObject* args)
{
    PyObject* palette;
    int palettesize = 256;
    int bits;
    ImagingShuffler pack;

    char* mode = "RGB";
    char* rawmode = "RGB";
    if (!PyArg_ParseTuple(args, "|ss", &mode, &rawmode))
	return NULL;

    if (!self->image->palette) {
	PyErr_SetString(PyExc_ValueError, "image has no palette");
	return NULL;
    }

    pack = ImagingFindPacker(mode, rawmode, &bits);
    if (!pack) {
	PyErr_SetString(PyExc_ValueError, "unknown raw mode");
	return NULL;
    }

    palette = PyString_FromStringAndSize(NULL, palettesize * bits / 8);
    if (!palette)
	return NULL;

    pack((UINT8*) PyString_AsString(palette),
	 self->image->palette->palette, palettesize);

    return palette;
}

static PyObject*
_histogram(ImagingObject* self, PyObject* args)
{
    ImagingObject* maskp;
    ImagingHistogram h;
    PyObject* list;
    int i;

    maskp = NULL;

    if (!PyArg_ParseTuple(args, "|O!", &Imaging_Type, &maskp))
	return NULL;

    h = ImagingGetHistogram(self->image, (maskp) ? maskp->image : NULL);
    if (!h)
	return NULL;

    /* Build an integer list containing the histogram */
    list = PyList_New(h->bands * 256);
    for (i = 0; i < h->bands * 256; i++) {
	PyObject* item;
	item = PyInt_FromLong(h->histogram[i]);
	if (item == NULL) {
	    Py_DECREF(list);
	    list = NULL;
	    break;
	}
	PyList_SetItem(list, i, item);
    }

    ImagingHistogramDelete(h);

    return list;
}

static PyObject* 
_offset(ImagingObject* self, PyObject* args)
{
    int xoffset, yoffset;

    if (!PyArg_ParseTuple(args, "ii", &xoffset, &yoffset))
	return NULL;

    return PyImagingNew(ImagingOffset(self->image, xoffset, yoffset));
}

static PyObject* 
_paste(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep;
    ImagingObject* maskp;
    int x0, y0, x1, y1;

    maskp = NULL;

    if (!PyArg_ParseTuple(args, "O!(iiii)|O!",
			  &Imaging_Type, &imagep,
			  &x0, &y0, &x1, &y1,
			  &Imaging_Type, &maskp))
	return NULL;

    if (maskp) {
	/* Paste with transparency layer */
	if (ImagingPaste(self->image, imagep->image, maskp->image,
			 x0, y0, x1, y1) < 0)
	    return NULL;
    } else {
	if (ImagingPaste(self->image, imagep->image, NULL,
			 x0, y0, x1, y1) < 0)
	    return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject*
_point(ImagingObject* self, PyObject* args)
{
    PyObject* list;
    UINT8 lut[1024];
    int n, i;
    char* mode;

    if (!PyArg_ParseTuple(args, "Oz", &list, &mode))
	return NULL;

    if (!PySequence_Check(list)) {
	PyErr_SetString(PyExc_TypeError, "Argument must be sequence");
	return NULL;
    }

    n = PyObject_Length(list);
    if (n != 256*(int) self->image->bands) {
	PyErr_SetString(PyExc_TypeError, "Wrong number of lut entries");
	return NULL;
    }

    /* Copy table to lut array (clip values to proper range) */
    for (i = 0; i < n; i++) {
	PyObject *op = PySequence_GetItem(list, i);
	int temp = PyInt_AsLong(op);
	Py_DECREF(op);
	if (temp <= 0)
	    lut[i] = 0;
	else if (temp >= 255)
	    lut[i] = 255;
	else
	    lut[i] = temp;
    }

    return PyImagingNew(ImagingPoint(self->image, mode, lut));
}

static PyObject*
_point_transform(ImagingObject* self, PyObject* args)
{
    double scale = 1.0;
    double offset = 0.0;
    if (!PyArg_ParseTuple(args, "|dd", &scale, &offset))
	return NULL;

    return PyImagingNew(ImagingPointTransform(self->image, scale, offset));
}

static PyObject*
_putdata(ImagingObject* self, PyObject* args)
{
    PyObject* data;
    Imaging image;
    int n, i, x, y;

    double scale = 1.0;
    double offset = 0.0;

    if (!PyArg_ParseTuple(args, "O|dd", &data, &scale, &offset))
	return NULL;

    if (!PySequence_Check(data)) {
	PyErr_SetString(PyExc_TypeError, "Argument must be sequence");
	return NULL;
    }

    image = self->image;

    if (!image->image8) {
	PyErr_SetString(PyExc_TypeError, "Wrong image mode");
	return NULL;
    }

    n = PyObject_Length(data);
    if (n > (int) (image->xsize * image->ysize)) {
	PyErr_SetString(PyExc_TypeError, "Too many data entries");
	return NULL;
    }

    if (PyString_Check(data)) {
	unsigned char* p;
	p = (unsigned char*) PyString_AS_STRING((PyStringObject*) data);
	if (scale == 1.0 && offset == 0.0)
	    /* Plain string data */
	    for (i = y = 0; i < n; i += image->xsize, y++) {
		x = n - i;
		if (x > (int) image->xsize)
		    x = image->xsize;
		memcpy(image->image8[y], p+i, x);
	    }
	else 
	    /* Scaled and clipped string data */
	    for (i = x = y = 0; i < n; i++) {
		int temp = (int) (p[i] * scale + offset);
		if (temp <= 0)
		    image->image8[y][x] = 0;
		else if (temp >= 255)
		    image->image8[y][x] = 255;
		else
		    image->image8[y][x] = temp;
		if (++x >= (int) image->xsize)
		    x = 0, y++;
	    }
    } else {
	if (scale == 1.0 && offset == 0.0)
	    /* Clipped data */
	    for (i = x = y = 0; i < n; i++) {
		PyObject *op = PySequence_GetItem(data, i);
		int temp = PyInt_AsLong(op);
		Py_DECREF(op);
		if (temp <= 0)
		    image->image8[y][x] = 0;
		else if (temp >= 255)
		    image->image8[y][x] = 255;
		else
		    image->image8[y][x] = temp;
		if (++x >= (int) image->xsize)
		    x = 0, y++;
	    }
	else
	    /* Scaled and clipped data */
	    for (i = x = y = 0; i < n; i++) {
		PyObject *op = PySequence_GetItem(data, i);
		int temp = (int) (PyFloat_AsDouble(op) * scale + offset);
		Py_DECREF(op);
		if (temp <= 0)
		    image->image8[y][x] = 0;
		else if (temp >= 255)
		    image->image8[y][x] = 255;
		else
		    image->image8[y][x] = temp;
		if (++x >= (int) image->xsize)
		    x = 0, y++;
	    }
	PyErr_Clear(); /* Avoid weird exceptions */
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* 
_quantize(ImagingObject* self, PyObject* args)
{
    int colours;
    ImagingPalette p;

    colours = 256;

    if (!PyArg_ParseTuple(args, "|i", &colours))
	return NULL;

    p = ImagingQuantize(self->image, colours);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* 
_putpalette(ImagingObject* self, PyObject* args)
{
    char* rawmode;
    UINT8* palette;
    int palettesize;
    int bits;
    ImagingShuffler unpack;

    if (!PyArg_ParseTuple(args, "ss#", &rawmode, &palette, &palettesize))
	return NULL;

    if (strcmp(self->image->mode, "L") != 0 && strcmp(self->image->mode, "P")) {
	PyErr_SetString(PyExc_ValueError, "illegal image mode");
	return NULL;
    }

    unpack = ImagingFindUnpacker(rawmode, &bits);
    if (!unpack) {
	PyErr_SetString(PyExc_ValueError, "unknown raw mode");
	return NULL;
    }

    ImagingPaletteDelete(self->image->palette);

    strcpy(self->image->mode, "P");

    self->image->palette = ImagingPaletteNew("RGB");

    unpack(self->image->palette->palette, palette, palettesize * 8 / bits);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* 
_putpalettealpha(ImagingObject* self, PyObject* args)
{
    int index;

    int alpha = 0;
    if (!PyArg_ParseTuple(args, "i|i", &index, &alpha))
	return NULL;

    if (!self->image->palette) {
	PyErr_SetString(PyExc_ValueError, "image has no palette");
	return NULL;
    }

    if (index < 0 || index >= 256) {
	PyErr_SetString(PyExc_ValueError, "illegal palette index");
	return NULL;
    }

    strcpy(self->image->palette->mode, "RGBA");
    self->image->palette->palette[index*4+3] = (UINT8) alpha;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* 
_resize(ImagingObject* self, PyObject* args)
{
    int xsize, ysize;

    if (!PyArg_ParseTuple(args, "(ii)", &xsize, &ysize))
	return NULL;

    return PyImagingNew(ImagingResize(self->image, xsize, ysize));
}

static PyObject* 
_rotate(ImagingObject* self, PyObject* args)
{
    double theta;

    if (!PyArg_ParseTuple(args, "d", &theta))
	return NULL;

    return PyImagingNew(ImagingRotate(self->image, theta));
}


static PyObject* 
_transform(ImagingObject* self, PyObject* args)
{
    int xsize, ysize;
    double a[6];

    if (!PyArg_ParseTuple(args, "(ii)(dddddd)", &xsize, &ysize,
			  a+1, a+2, a+0, a+4, a+5, a+3))
	return NULL;

    return PyImagingNew(ImagingTransformAffine(self->image, xsize, ysize,
					       a, 1));
}

static PyObject* 
_transpose(ImagingObject* self, PyObject* args)
{
    int op;
    Imaging im;

    if (!PyArg_ParseTuple(args, "i", &op))
	return NULL;

    switch (op) {
      case 0:
	im = ImagingFlipLeftRight(self->image);
	break;
      case 1:
	im = ImagingFlipTopBottom(self->image);
	break;
      case 2:
	im = ImagingRotate90(self->image);
	break;
      case 3:
	im = ImagingRotate180(self->image);
	break;
      case 4:
	im = ImagingRotate270(self->image);
	break;
      default:
	PyErr_SetString(PyExc_ValueError, "no such transpose operation");
	return NULL;
    }
    return PyImagingNew(im);
}

/* -------------------------------------------------------------------- */

static PyObject* 
_isblock(ImagingObject* self, PyObject* args)
{
    return Py_BuildValue("l", (long) self->image->block);
}

static PyObject* 
_getbbox(ImagingObject* self, PyObject* args)
{
    int bbox[4];

    if (!ImagingGetBBox(self->image, bbox)) {
	Py_INCREF(Py_None);
	return Py_None;
    }

    return Py_BuildValue("iiii", bbox[0], bbox[1], bbox[2], bbox[3]);
}

static PyObject* 
_getprojection(ImagingObject* self, PyObject* args)
{
    unsigned char* xprofile;
    unsigned char* yprofile;
    PyObject* result;

    xprofile = malloc(self->image->xsize);
    yprofile = malloc(self->image->ysize);

    if (xprofile == NULL || yprofile == NULL) {
	free(xprofile);
	free(yprofile);
	PyErr_NoMemory();
	return NULL;
    }

    ImagingGetProjection(self->image, (unsigned char *)xprofile, (unsigned char *)yprofile);

    result = Py_BuildValue("s#s#", xprofile, self->image->xsize,
			   yprofile, self->image->ysize);

    free(xprofile);
    free(yprofile);

    return result;
}

/* -------------------------------------------------------------------- */

static PyObject* 
_getband(ImagingObject* self, PyObject* args)
{
    int band;

    if (!PyArg_ParseTuple(args, "i", &band))
	return NULL;

    return PyImagingNew(ImagingGetBand(self->image, band));
}

static PyObject* 
_putband(ImagingObject* self, PyObject* args)
{
    int band;
    ImagingObject* imagep;

    if (!PyArg_ParseTuple(args, "O!i",
			  &Imaging_Type, &imagep,
			  &band))
	return NULL;

    if (!ImagingPutBand(self->image, imagep->image, band))
	return NULL;

    Py_INCREF(Py_None);
    return Py_None;
}

/* -------------------------------------------------------------------- */

#ifdef WITH_IMAGECHOPS

static PyObject* 
_chop_invert(ImagingObject* self, PyObject* args)
{
    return PyImagingNew(ImagingNegative(self->image));
}

static PyObject* 
_chop_lighter(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep;

    if (!PyArg_ParseTuple(args, "O!", &Imaging_Type, &imagep))
	return NULL;

    return PyImagingNew(ImagingChopLighter(self->image, imagep->image));
}

static PyObject* 
_chop_darker(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep;

    if (!PyArg_ParseTuple(args, "O!", &Imaging_Type, &imagep))
	return NULL;

    return PyImagingNew(ImagingChopDarker(self->image, imagep->image));
}

static PyObject* 
_chop_difference(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep;

    if (!PyArg_ParseTuple(args, "O!", &Imaging_Type, &imagep))
	return NULL;

    return PyImagingNew(ImagingChopDifference(self->image, imagep->image));
}

static PyObject* 
_chop_multiply(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep;

    if (!PyArg_ParseTuple(args, "O!", &Imaging_Type, &imagep))
	return NULL;

    return PyImagingNew(ImagingChopMultiply(self->image, imagep->image));
}

static PyObject* 
_chop_screen(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep;

    if (!PyArg_ParseTuple(args, "O!", &Imaging_Type, &imagep))
	return NULL;

    return PyImagingNew(ImagingChopScreen(self->image, imagep->image));
}

static PyObject* 
_chop_add(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep;
    float scale;
    int offset;

    scale = 1.0;
    offset = 0;

    if (!PyArg_ParseTuple(args, "O!|fi", &Imaging_Type, &imagep,
			  &scale, &offset))
	return NULL;

    return PyImagingNew(ImagingChopAdd(self->image, imagep->image,
				       scale, offset));
}

static PyObject* 
_chop_subtract(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep;
    float scale;
    int offset;

    scale = 1.0;
    offset = 0;

    if (!PyArg_ParseTuple(args, "O!|fi", &Imaging_Type, &imagep,
			  &scale, &offset))
	return NULL;

    return PyImagingNew(ImagingChopSubtract(self->image, imagep->image,
					    scale, offset));
}

static PyObject* 
_chop_and(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep;

    if (!PyArg_ParseTuple(args, "O!", &Imaging_Type, &imagep))
	return NULL;

    return PyImagingNew(ImagingChopAnd(self->image, imagep->image));
}

static PyObject* 
_chop_or(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep;

    if (!PyArg_ParseTuple(args, "O!", &Imaging_Type, &imagep))
	return NULL;

    return PyImagingNew(ImagingChopOr(self->image, imagep->image));
}

static PyObject* 
_chop_xor(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep;

    if (!PyArg_ParseTuple(args, "O!", &Imaging_Type, &imagep))
	return NULL;

    return PyImagingNew(ImagingChopXor(self->image, imagep->image));
}

static PyObject* 
_chop_add_modulo(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep;

    if (!PyArg_ParseTuple(args, "O!", &Imaging_Type, &imagep))
	return NULL;

    return PyImagingNew(ImagingChopAddModulo(self->image, imagep->image));
}

static PyObject* 
_chop_subtract_modulo(ImagingObject* self, PyObject* args)
{
    ImagingObject* imagep;

    if (!PyArg_ParseTuple(args, "O!", &Imaging_Type, &imagep))
	return NULL;

    return PyImagingNew(ImagingChopSubtractModulo(self->image, imagep->image));
}

#endif


/* -------------------------------------------------------------------- */

#ifdef	WITH_IMAGEDRAW

static PyObject* 
_draw_line(ImagingObject* self, PyObject* args)
{
    int x0, y0, x1, y1;
    int ink;
    if (!PyArg_ParseTuple(args, "(ii)(ii)i", &x0, &y0, &x1, &y1, &ink))
	return NULL;

    if (ImagingDrawLine(self->image, x0, y0, x1, y1, ink) < 0)
	return NULL;

    Py_INCREF(Py_None);
    return Py_None;
}

extern int PyPath_Flatten(PyObject* data, double **xy);

static PyObject* 
_draw_lines(ImagingObject* self, PyObject* args)
{
    double *xy;
    int i, n;

    PyObject *data;
    int ink;
    if (!PyArg_ParseTuple(args, "Oi", &data, &ink))
	return NULL;

    n = PyPath_Flatten(data, &xy);
    if (n < 0)
	return NULL;

    for (i = 0; i < n-1; i++) {
	double *p = &xy[i+i];
	if (ImagingDrawLine(self->image, (int) p[0], (int) p[1],
			    (int) p[2], (int) p[3], ink) < 0) {
	    free(xy);
	    return NULL;
	}
    }

    free(xy);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* 
_draw_point(ImagingObject* self, PyObject* args)
{
    int x, y;
    int ink;
    if (!PyArg_ParseTuple(args, "(ii)i", &x, &y, &ink))
	return NULL;

    if (ImagingDrawPoint(self->image, x, y, ink) < 0)
	return NULL;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* 
_draw_points(ImagingObject* self, PyObject* args)
{
    double *xy;
    int i, n;

    PyObject *data;
    int ink;
    if (!PyArg_ParseTuple(args, "Oi", &data, &ink))
	return NULL;

    n = PyPath_Flatten(data, &xy);
    if (n < 0)
	return NULL;

    for (i = 0; i < n; i++) {
	double *p = &xy[i+i];
	if (ImagingDrawPoint(self->image, (int) p[0], (int) p[1], ink) < 0) {
	    free(xy);
	    return NULL;
	}
    }

    free(xy);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* 
_draw_polygon(ImagingObject* self, PyObject* args)
{
    double *xy;
    int *ixy;
    int n, i;

    PyObject* data;
    int ink;
    int fill = 0;
    if (!PyArg_ParseTuple(args, "Oi|i", &data, &ink, &fill))
	return NULL;

    n = PyPath_Flatten(data, &xy);
    if (n < 0)
	return NULL;

    /* Copy list of vertices to array */
    ixy = (int*) malloc(n * 2 * sizeof(int));

    for (i = 0; i < n; i++) {
	ixy[i+i] = (int) xy[i+i];
	ixy[i+i+1] = (int) xy[i+i+1];
    }

    free(xy);

    if (ImagingDrawPolygon(self->image, n, ixy, ink, fill) < 0) {
	free(ixy);
	return NULL;
    }

    free(ixy);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* 
_draw_rectangle(ImagingObject* self, PyObject* args)
{
    float x0, y0, x1, y1;
    int ink;
    int fill = 0;
    if (!PyArg_ParseTuple(args, "(ffff)i|i", &x0, &y0, &x1, &y1, &ink, &fill))
	return NULL;

    if (ImagingDrawRectangle(self->image, x0, y0, x1, y1, ink, fill) < 0)
	return NULL;

    Py_INCREF(Py_None);
    return Py_None;
}

#endif

/* -------------------------------------------------------------------- */
/* EFFECTS (experimental)        					*/
/* -------------------------------------------------------------------- */

#ifdef WITH_EFFECTS

static PyObject* 
_effect_mandelbrot(ImagingObject* self, PyObject* args)
{
    int xsize = 512;
    int ysize = 512;
    double extent[4];
    int quality = 100;

    extent[0] = -3; extent[1] = -2.5;
    extent[2] = 2;  extent[3] = 2.5;

    if (!PyArg_ParseTuple(args, "|(ii)(dddd)i", &xsize, &ysize,
                          &extent[0], &extent[1], &extent[2], &extent[3],
                          &quality))
	return NULL;

    return PyImagingNew(ImagingEffectMandelbrot(xsize, ysize, extent, quality));
}

static PyObject* 
_effect_noise(ImagingObject* self, PyObject* args)
{
    int xsize, ysize;
    float sigma = 128;
    if (!PyArg_ParseTuple(args, "(ii)|f", &xsize, &ysize, &sigma))
	return NULL;

    return PyImagingNew(ImagingEffectNoise(xsize, ysize, sigma));
}

static PyObject* 
_effect_spread(ImagingObject* self, PyObject* args)
{
    int dist;

    if (!PyArg_ParseTuple(args, "i", &dist))
	return NULL;

    return PyImagingNew(ImagingEffectSpread(self->image, dist));
}

#endif

/* -------------------------------------------------------------------- */
/* UTILITIES								*/
/* -------------------------------------------------------------------- */

static PyObject* 
_crc32(PyObject* self, PyObject* args)
{
    unsigned char* buffer;
    int bytes;
    int hi, lo;
    UINT32 crc;

    hi = lo = 0;

    if (!PyArg_ParseTuple(args, "s#|(ii)", &buffer, &bytes, &hi, &lo))
	return NULL;

    crc = ((UINT32) (hi & 0xFFFF) << 16) + (lo & 0xFFFF);

    crc = ImagingCRC32(crc, (unsigned char *)buffer, bytes);

    return Py_BuildValue("ii", (crc >> 16) & 0xFFFF, crc & 0xFFFF);
}

static PyObject* 
_getcodecstatus(PyObject* self, PyObject* args)
{
    int status;
    char* msg;

    if (!PyArg_ParseTuple(args, "i", &status))
	return NULL;

    switch (status) {
    case IMAGING_CODEC_OVERRUN:
	msg = "Buffer overrun."; break;
    case IMAGING_CODEC_BROKEN:
	msg = "Broken data stream."; break;
    case IMAGING_CODEC_UNKNOWN:
	msg = "Unknown data stream contents."; break;
    case IMAGING_CODEC_CONFIG:
	msg = "Codec configuration error."; break;
    case IMAGING_CODEC_MEMORY:
	msg = "Out of memory."; break;
    default:
	Py_INCREF(Py_None);
	return Py_None;
    }

    return Py_BuildValue("s", msg);
}

/* -------------------------------------------------------------------- */
/* DEBUGGING HELPERS							*/
/* -------------------------------------------------------------------- */


#ifdef WITH_DEBUG

static PyObject* 
_save_ppm(ImagingObject* self, PyObject* args)
{
    char* filename;

    if (!PyArg_ParseTuple(args, "s", &filename))
	return NULL;

    ImagingSavePPM(self->image, filename);

    Py_INCREF(Py_None);
    return Py_None;
}

#endif

/* -------------------------------------------------------------------- */

/* methods */

static struct PyMethodDef methods[] = {

    /* Standard processing methods (Image) */
    {"convert", (PyCFunction)_convert, 1},
    {"convert2", (PyCFunction)_convert2, 1},
    {"convert_matrix", (PyCFunction)_convert_matrix, 1},
    {"copy", (PyCFunction)_copy, 1},
    {"copy2", (PyCFunction)_copy2, 1},
    {"crop", (PyCFunction)_crop, 1},
    {"filter", (PyCFunction)_filter, 1},
    {"histogram", (PyCFunction)_histogram, 1},
    {"offset", (PyCFunction)_offset, 1},
    {"paste", (PyCFunction)_paste, 1},
    {"point", (PyCFunction)_point, 1},
    {"point_transform", (PyCFunction)_point_transform, 1},
    {"putdata", (PyCFunction)_putdata, 1},
    {"quantize", (PyCFunction)_quantize, 1},
    {"resize", (PyCFunction)_resize, 1},
    {"rotate", (PyCFunction)_rotate, 1},
    {"transform", (PyCFunction)_transform, 1},
    {"transpose", (PyCFunction)_transpose, 1},

    {"isblock", (PyCFunction)_isblock, 1},

    {"getbbox", (PyCFunction)_getbbox, 1},
    {"getprojection", (PyCFunction)_getprojection, 1},

    {"getband", (PyCFunction)_getband, 1},
    {"putband", (PyCFunction)_putband, 1},

    {"getpalette", (PyCFunction)_getpalette, 1},
    {"putpalette", (PyCFunction)_putpalette, 1},
    {"putpalettealpha", (PyCFunction)_putpalettealpha, 1},

#ifdef WITH_IMAGECHOPS
    /* Channel operations (ImageChops) */
    {"chop_invert", (PyCFunction)_chop_invert, 1},
    {"chop_lighter", (PyCFunction)_chop_lighter, 1},
    {"chop_darker", (PyCFunction)_chop_darker, 1},
    {"chop_difference", (PyCFunction)_chop_difference, 1},
    {"chop_multiply", (PyCFunction)_chop_multiply, 1},
    {"chop_screen", (PyCFunction)_chop_screen, 1},
    {"chop_add", (PyCFunction)_chop_add, 1},
    {"chop_subtract", (PyCFunction)_chop_subtract, 1},
    {"chop_add_modulo", (PyCFunction)_chop_add_modulo, 1},
    {"chop_subtract_modulo", (PyCFunction)_chop_subtract_modulo, 1},
    {"chop_and", (PyCFunction)_chop_and, 1},
    {"chop_or", (PyCFunction)_chop_or, 1},
    {"chop_xor", (PyCFunction)_chop_xor, 1},
#endif

#ifdef WITH_IMAGEDRAW
    /* Graphics (ImageDraw) */
    {"draw_line", (PyCFunction)_draw_line, 1},
    {"draw_lines", (PyCFunction)_draw_lines, 1},
    {"draw_point", (PyCFunction)_draw_point, 1},
    {"draw_points", (PyCFunction)_draw_points, 1},
    {"draw_polygon", (PyCFunction)_draw_polygon, 1},
    {"draw_rectangle", (PyCFunction)_draw_rectangle, 1},
#endif

#ifdef WITH_EFFECTS
    /* Special effects */
    {"effect_spread", (PyCFunction)_effect_spread, 1},
#endif

    /* Misc. */
    {"new_array", (PyCFunction)_new_array, 1},
    {"new_block", (PyCFunction)_new_block, 1},
    {"save_ppm", (PyCFunction)_save_ppm, 1},

    {NULL, NULL} /* sentinel */
};


/* attributes */

static PyObject*  
_getattr(ImagingObject* self, char* name)
{
    PyObject* res;

    res = Py_FindMethod(methods, (PyObject*) self, name);
    if (res)
	return res;
    PyErr_Clear();
    if (strcmp(name, "mode") == 0)
	return Py_BuildValue("s", self->image->mode);
    if (strcmp(name, "size") == 0)
	return Py_BuildValue("ii", self->image->xsize, self->image->ysize);
    if (strcmp(name, "bands") == 0)
	return Py_BuildValue("i", self->image->bands);
    if (strcmp(name, "id") == 0)
	return Py_BuildValue("l", (long) self->image);
    PyErr_SetString(PyExc_AttributeError, name);
    return NULL;
}


/* basic sequence semantics */

static int
image_length(ImagingObject *self)
{
    Imaging im = self->image;

    return im->xsize * im->ysize;
}

static PyObject *
image_item(ImagingObject *self, int i)
{
    unsigned x, y;
    UINT8 *p;
    Imaging im = self->image;

    x = i % im->xsize;
    y = i / im->xsize;

    if (x >= (unsigned) im->xsize || y >= (unsigned) im->ysize) {
	PyErr_SetString(PyExc_IndexError, "image index out of range");
	return NULL;
    }

    /* single layer */
    if (im->image8)
	return Py_BuildValue("i", im->image8[y][x]);

    /* multilayer */
    p = (UINT8*) &im->image32[y][x];
    switch (im->type) {
        case 0:
            /* unsigned integer */
            if (im->bands == 3)
                return Py_BuildValue("iii", p[0], p[1], p[2]);
            return Py_BuildValue("iiii", p[0], p[1], p[2], p[3]);
        case 1:
            /* signed integer */
            return Py_BuildValue("i", *(INT32*) p);
        case 2:
            /* floating point */
            return Py_BuildValue("d", *(FLOAT32*) p);
    }

    /* unknown type */
    Py_INCREF(Py_None);
    return Py_None;
}

static PySequenceMethods image_as_sequence = {
    (inquiry)image_length, /*sq_length*/
    (binaryfunc)0, /*sq_concat*/
    (intargfunc)0, /*sq_repeat*/
    (intargfunc)image_item, /*sq_item*/
    (intintargfunc)0, /*sq_slice*/
    (intobjargproc)0, /*sq_ass_item*/
    (intintobjargproc)0, /*sq_ass_slice*/
};


/* type description */

statichere PyTypeObject Imaging_Type = {
    PyObject_HEAD_INIT(NULL)
    0,				/*ob_size*/
    "ImagingCore",		/*tp_name*/
    sizeof(ImagingObject),	/*tp_size*/
    0,				/*tp_itemsize*/
    /* methods */
    (destructor)_dealloc,	/*tp_dealloc*/
    0,				/*tp_print*/
    (getattrfunc)_getattr,	/*tp_getattr*/
    0,				/*tp_setattr*/
    0,				/*tp_compare*/
    0,				/*tp_repr*/
    0,                          /*tp_as_number */
    &image_as_sequence,         /*tp_as_sequence */
    0,                          /*tp_as_mapping */
    0                           /*tp_hash*/
};

/* Decoders (in decode.c) */
extern PyObject* PyImaging_BitDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_FliDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_GifDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_HexDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_JpegDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_TiffLzwDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_MspDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_PackbitsDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_PcdDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_PcxDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_RawDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_SunRleDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_TgaRleDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_XbmDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_ZipDecoderNew(PyObject* self, PyObject* args);

/* Encoders (in encode.c) */
extern PyObject* PyImaging_EpsEncoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_GifEncoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_JpegEncoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_RawEncoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_XbmEncoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_ZipEncoderNew(PyObject* self, PyObject* args);

/* Display support (in display.c) */
extern PyObject* PyImaging_DisplayWin32(PyObject* self, PyObject* args);
extern PyObject* PyImaging_DisplayModeWin32(PyObject* self, PyObject* args);

/* Experimental path stuff (in path.c) */
extern PyObject* PyPath_Create(ImagingObject* self, PyObject* args);

extern PyObject* PyImaging_Mapper(PyObject* self, PyObject* args);

static PyMethodDef functions[] = {

    /* Object factories */
    {"blend", (PyCFunction)_blend, 1},
    {"fill", (PyCFunction)_fill, 1},
    {"new", (PyCFunction)_new, 1},

    /* Functions */
    {"convert", (PyCFunction)_convert, 1},
    {"copy", (PyCFunction)_copy, 1},

    /* Codecs */
    {"bit_decoder", (PyCFunction)PyImaging_BitDecoderNew, 1},
    {"eps_encoder", (PyCFunction)PyImaging_EpsEncoderNew, 1},
    {"fli_decoder", (PyCFunction)PyImaging_FliDecoderNew, 1},
#ifdef WITH_LZW
    {"gif_decoder", (PyCFunction)PyImaging_GifDecoderNew, 1},
#endif
    {"gif_encoder", (PyCFunction)PyImaging_GifEncoderNew, 1},
    {"hex_decoder", (PyCFunction)PyImaging_HexDecoderNew, 1},
    {"hex_encoder", (PyCFunction)PyImaging_EpsEncoderNew, 1}, /* EPS=HEX! */
#ifdef HAVE_LIBJPEG
    {"jpeg_decoder", (PyCFunction)PyImaging_JpegDecoderNew, 1},
    {"jpeg_encoder", (PyCFunction)PyImaging_JpegEncoderNew, 1},
#endif
#ifdef WITH_LZW
    {"tiff_lzw_decoder", (PyCFunction)PyImaging_TiffLzwDecoderNew, 1},
#endif
    {"msp_decoder", (PyCFunction)PyImaging_MspDecoderNew, 1},
    {"packbits_decoder", (PyCFunction)PyImaging_PackbitsDecoderNew, 1},
    {"pcd_decoder", (PyCFunction)PyImaging_PcdDecoderNew, 1},
    {"pcx_decoder", (PyCFunction)PyImaging_PcxDecoderNew, 1},
    {"raw_decoder", (PyCFunction)PyImaging_RawDecoderNew, 1},
    {"raw_encoder", (PyCFunction)PyImaging_RawEncoderNew, 1},
    {"sun_rle_decoder", (PyCFunction)PyImaging_SunRleDecoderNew, 1},
    {"tga_rle_decoder", (PyCFunction)PyImaging_TgaRleDecoderNew, 1},
    {"xbm_decoder", (PyCFunction)PyImaging_XbmDecoderNew, 1},
    {"xbm_encoder", (PyCFunction)PyImaging_XbmEncoderNew, 1},
#ifdef HAVE_LIBZ
    {"zip_decoder", (PyCFunction)PyImaging_ZipDecoderNew, 1},
    {"zip_encoder", (PyCFunction)PyImaging_ZipEncoderNew, 1},
#endif

    /* Memory mapping */
#ifdef WITH_MAPPING
#ifdef WIN32
    {"map", (PyCFunction)PyImaging_Mapper, 1},
#endif
#endif

    /* Display support */
#ifdef WIN32
    {"display", (PyCFunction)PyImaging_DisplayWin32, 1},
    {"display_mode", (PyCFunction)PyImaging_DisplayModeWin32, 1},
#endif

    /* Utilities */
    {"crc32", (PyCFunction)_crc32, 1},
    {"getcodecstatus", (PyCFunction)_getcodecstatus, 1},

    /* Debugging stuff */
    {"open_ppm", (PyCFunction)_open_ppm, 1},

    /* Special effects (experimental) */
#ifdef WITH_EFFECTS
    {"effect_mandelbrot", (PyCFunction)_effect_mandelbrot, 1},
    {"effect_noise", (PyCFunction)_effect_noise, 1},
    {"linear_gradient", (PyCFunction)_linear_gradient, 1},
    {"radial_gradient", (PyCFunction)_radial_gradient, 1},
    {"wedge", (PyCFunction)_linear_gradient, 1}, /* Compatibility */
#endif

    /* Experimental path stuff */
#ifdef WITH_IMAGEPATH
    {"path", (PyCFunction)PyPath_Create, 1},
#endif

    {NULL, NULL} /* sentinel */
};

void
init_imaging()
{
    /* Patch object type */
    Imaging_Type.ob_type = &PyType_Type;

    Py_InitModule("_imaging", functions);
}
