/* $Id: _imagingmodule.c,v 1.9 1996/12/16 18:50:08 fl Exp fl $
 *
 * The Python Imaging Library.
 *
 * File:
 *	_imagingmodule.c -- the Imaging class bindings
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
 *	96-12-10 fl	Added crc32 interface
 *	96-12-14 fl	Added modulo arithmetics
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


#include "Python.h"

#include "Imaging.h"


#define WITH_CHOPS
#define	WITH_GRAPHICS
#define	WITH_PATH
#define	WITH_STRING
#define	WITH_DEBUG


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

    imagep->image = imOut;

    return (PyObject*) imagep;
}

static void
_dealloc(ImagingObject* imagep)
{
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
		    (message) ? (char*) message : "PIL: bad argument");
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

    xsize = ysize = 256;
    colour = 0;

    if (!PyArg_ParseTuple(args, "s|(ii)i", &mode, &xsize, &ysize, &colour))
	return NULL;

    return PyImagingNew(ImagingFill(mode, xsize, ysize, &colour));
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

    return PyImagingNew(ImagingConvert(self->image, mode));
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
_histogram(ImagingObject* self, PyObject* args)
{
    ImagingHistogram h;
    PyObject* list;
    int i;

    h = ImagingHisto(self->image);
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

    /* printf("quantize => %p\n", p); */

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

    unpack = ImagingFindUnpacker(rawmode, &bits);
    if (!unpack) {
	PyErr_SetString(PyExc_ValueError, "unknown raw mode");
	return NULL;
    }

    ImagingPaletteDelete(self->image->palette);

    self->image->palette = ImagingPaletteNew("RGB");

    unpack(self->image->palette->palette, palette, palettesize * 8 / bits);

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
_getbbox(ImagingObject* self, PyObject* args)
{
    int bbox[4];

    if (!ImagingGetBBox(self->image, bbox)) {
	Py_INCREF(Py_None);
	return Py_None;
    }

    return Py_BuildValue("iiii", bbox[0], bbox[1], bbox[2], bbox[3]);
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

    if (ImagingPutBand(self->image, imagep->image, band) < 0)
	return NULL;

    Py_INCREF(Py_None);
    return Py_None;
}

/* -------------------------------------------------------------------- */

#ifdef WITH_CHOPS

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

#ifdef WITH_STRING

static PyObject* 
_fromstring(ImagingObject* self, PyObject* args)
{
    char* buf;
    int bufsize;
    Imaging image;

    image = self->image;

    if (!PyArg_ParseTuple(args, "s#", &buf, &bufsize))
	return NULL;

    if (bufsize != (int) (image->ysize * image->linesize)) {
	PyErr_SetString(PyExc_ValueError, "illegal buffer size");
	return NULL;
    }

    if (ImagingFromString(image, -1, buf) < 0)
	return NULL;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* 
_tostring(ImagingObject* self, PyObject* args)
{
    PyObject* buf;
    Imaging image;

    image = self->image;

    buf = PyString_FromStringAndSize(NULL, image->ysize * image->linesize);
    if (!buf)
	return NULL;

    if (ImagingToString(image, -1, PyString_AsString(buf)) < 0)
	return NULL;

    return buf;
}

#endif

/* -------------------------------------------------------------------- */

#ifdef	WITH_GRAPHICS

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
    PyObject *data;
    double *xy;
    int i, n, ink;

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
    PyObject *data;
    double *xy;
    int i, n, ink;

    if (!PyArg_ParseTuple(args, "Oi", &data, &ink))
	return NULL;

    n = PyPath_Flatten(data, &xy);
    if (n < 0)
	return NULL;

    for (i = 0; i < n-1; i++) {
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
    PyObject* data;
    double *xy;
    int *ixy;
    int n, i;
    int ink, fill;

    fill = 0;
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
    int ink, fill;

    fill = 0;
    if (!PyArg_ParseTuple(args, "(ffff)i|i", &x0, &y0, &x1, &y1, &ink, &fill))
	return NULL;

    if (ImagingDrawRectangle(self->image, x0, y0, x1, y1, ink, fill) < 0)
	return NULL;

    Py_INCREF(Py_None);
    return Py_None;
}

#endif


/* -------------------------------------------------------------------- */
/* UTILITIES								*/
/* -------------------------------------------------------------------- */

static PyObject* 
_crc32(PyObject* self, PyObject* args)
{
    char* buffer;
    int bytes;
    int hi, lo;
    UINT32 crc;

    hi = lo = 0;

    if (!PyArg_ParseTuple(args, "s#|(ii)", &buffer, &bytes, &hi, &lo))
	return NULL;

    crc = ((UINT32) (hi & 0xFFFF) << 16) + (lo & 0xFFFF);

    crc = ImagingCRC32(crc, buffer, bytes);

    return Py_BuildValue("ii", (crc >> 16) & 0xFFFF, crc & 0xFFFF);
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
    {"convert_matrix", (PyCFunction)_convert_matrix, 1},
    {"copy", (PyCFunction)_copy, 1},
    {"crop", (PyCFunction)_crop, 1},
    {"filter", (PyCFunction)_filter, 1},
    {"histogram", (PyCFunction)_histogram, 1},
    {"offset", (PyCFunction)_offset, 1},
    {"paste", (PyCFunction)_paste, 1},
    {"point", (PyCFunction)_point, 1},
    {"putdata", (PyCFunction)_putdata, 1},
    {"quantize", (PyCFunction)_quantize, 1},
    {"resize", (PyCFunction)_resize, 1},
    {"rotate", (PyCFunction)_rotate, 1},
    {"transform", (PyCFunction)_transform, 1},
    {"transpose", (PyCFunction)_transpose, 1},

    {"getbbox", (PyCFunction)_getbbox, 1},

    {"getband", (PyCFunction)_getband, 1},
    {"putband", (PyCFunction)_putband, 1},

    {"putpalette", (PyCFunction)_putpalette, 1},

#ifdef WITH_CHOPS
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

#ifdef WITH_GRAPHICS
    /* Graphics (ImageDraw) */
    {"draw_line", (PyCFunction)_draw_line, 1},
    {"draw_lines", (PyCFunction)_draw_lines, 1},
    {"draw_point", (PyCFunction)_draw_point, 1},
    {"draw_points", (PyCFunction)_draw_points, 1},
    {"draw_polygon", (PyCFunction)_draw_polygon, 1},
    {"draw_rectangle", (PyCFunction)_draw_rectangle, 1},
#endif

#ifdef WITH_STRING
    /* String interface */
    {"fromstring", (PyCFunction)_fromstring, 1},
    {"tostring", (PyCFunction)_tostring, 1},
#endif

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
	y = i / im->ysize;

	if (x >= im->xsize || y >= im->ysize) {
	    PyErr_SetString(PyExc_IndexError, "image index out of range");
	    return NULL;
	}

	/* single layer */
	if (im->image8)
	    return Py_BuildValue("i", im->image8[y][x]);

	/* multilayer */
	p = (UINT8*) &im->image32[y][x];
	if (im->bands == 3)
	    return Py_BuildValue("iii", p[0], p[1], p[2]);
	return Py_BuildValue("iiii", p[0], p[1], p[2], p[3]);
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
	"ImagingCore",			/*tp_name*/
	sizeof(ImagingObject),		/*tp_size*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)_dealloc,		/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)_getattr,		/*tp_getattr*/
	0,				/*tp_setattr*/
	0,				/*tp_compare*/
	0,				/*tp_repr*/
	0,                              /*tp_as_number */
	&image_as_sequence,             /*tp_as_sequence */
	0,                              /*tp_as_mapping */
	0                               /*tp_hash */
};

/* Decoders (in decode.c) */
extern PyObject* PyImaging_GifDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_HexDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_JpegDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_LzwDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_PackbitsDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_PcdDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_PcxDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_RawDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_XbmDecoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_ZipDecoderNew(PyObject* self, PyObject* args);

/* Encoders (in encode.c) */
extern PyObject* PyImaging_EpsEncoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_GifEncoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_JpegEncoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_RawEncoderNew(PyObject* self, PyObject* args);
extern PyObject* PyImaging_XbmEncoderNew(PyObject* self, PyObject* args);

/* Display support (in display.c) */
extern PyObject* PyImaging_DisplayWin32(PyObject* self, PyObject* args);
extern PyObject* PyImaging_DisplayModeWin32(PyObject* self, PyObject* args);

/* Experimental path stuff (in path.c) */
extern PyObject* PyPath_Create(ImagingObject* self, PyObject* args);

static PyMethodDef functions[] = {

    /* Object factories */
    {"blend", (PyCFunction)_blend, 1},
    {"fill", (PyCFunction)_fill, 1},
    {"new", (PyCFunction)_new, 1},
    {"new_array", (PyCFunction)_new_array, 1},
    {"new_block", (PyCFunction)_new_block, 1},

    /* Codecs */
    {"eps_encoder", (PyCFunction)PyImaging_EpsEncoderNew, 1},
    {"gif_decoder", (PyCFunction)PyImaging_GifDecoderNew, 1},
    {"hex_decoder", (PyCFunction)PyImaging_HexDecoderNew, 1},
#ifdef HAVE_LIBJPEG
    {"jpeg_decoder", (PyCFunction)PyImaging_JpegDecoderNew, 1},
    {"jpeg_encoder", (PyCFunction)PyImaging_JpegEncoderNew, 1},
#endif
    {"lzw_decoder", (PyCFunction)PyImaging_LzwDecoderNew, 1},
    {"packbits_decoder", (PyCFunction)PyImaging_PackbitsDecoderNew, 1},
    {"pcd_decoder", (PyCFunction)PyImaging_PcdDecoderNew, 1},
    {"pcx_decoder", (PyCFunction)PyImaging_PcxDecoderNew, 1},
    {"raw_decoder", (PyCFunction)PyImaging_RawDecoderNew, 1},
    {"raw_encoder", (PyCFunction)PyImaging_RawEncoderNew, 1},
    {"xbm_decoder", (PyCFunction)PyImaging_XbmDecoderNew, 1},
    {"xbm_encoder", (PyCFunction)PyImaging_XbmEncoderNew, 1},
#ifdef HAVE_LIBZ
    {"zip_decoder", (PyCFunction)PyImaging_ZipDecoderNew, 1},
#endif

    /* Display support */
#ifdef WIN32
    {"display", (PyCFunction)PyImaging_DisplayWin32, 1},
    {"display_mode", (PyCFunction)PyImaging_DisplayModeWin32, 1},
#endif

    /* Debugging stuff */
    {"open_ppm", (PyCFunction)_open_ppm, 1},

    {"linear_gradient", (PyCFunction)_linear_gradient, 1},
    {"radial_gradient", (PyCFunction)_radial_gradient, 1},
    {"wedge", (PyCFunction)_linear_gradient, 1}, /* Compatibility */

    /* Experimental path stuff */
#ifdef WITH_PATH
    {"path", (PyCFunction)PyPath_Create, 1},
#endif

    /* Utilities */
#ifdef WITH_PATH
    {"crc32", (PyCFunction)_crc32, 1},
#endif

    {NULL, NULL} /* sentinel */
};

void
init_imaging()
{
    Py_InitModule("_imaging", functions);
}
