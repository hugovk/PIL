/* $Id: encode.c,v 1.5 1996/12/16 18:50:08 fl Exp fl $
 *
 * The Python Imaging Library.
 *
 * File:
 *	encoders.c -- experimental encoder interfaces for the Imaging library
 *
 * History:
 *	96-04-19 fl	Based on decoders.c
 *	96-05-12 fl	Compile cleanly as C++
 *
 * Copyright (c) Fredrik Lundh 1996.  All rights reserved.
 */


#include "Python.h"

#include "Imaging.h"


/* -------------------------------------------------------------------- */
/* Common								*/
/* -------------------------------------------------------------------- */

typedef struct {
    PyObject_HEAD
    int (*encode)(Imaging im, ImagingCodecState state,
		  UINT8* buffer, int bytes);
    struct ImagingCodecStateInstance state;
    Imaging im;
    PyObject* lock;
} ImagingEncoderObject;

staticforward PyTypeObject ImagingEncoderType;

static ImagingEncoderObject*
PyImaging_EncoderNew(int contextsize)
{
    ImagingEncoderObject *encoder;
    void *context;

    encoder = PyObject_NEW(ImagingEncoderObject, &ImagingEncoderType);
    if (encoder == NULL)
	return NULL;

    /* Clear the encoder state */
    memset(&encoder->state, 0, sizeof(encoder->state));

    /* Allocate encoder context */
    if (contextsize > 0) {
	context = (void*) calloc(1, contextsize);
	if (!context) {
	    Py_DECREF(encoder);
	    PyErr_NoMemory();
	    return NULL;
	}
    } else
	context = 0;

    /* Initialize encoder context */
    encoder->state.context = context;

    /* Target image */
    encoder->lock = NULL;
    encoder->im = NULL;

    return encoder;
}

static void
_dealloc(ImagingEncoderObject* encoder)
{
    free(encoder->state.buffer);
    free(encoder->state.context);
    Py_XDECREF(encoder->lock);
    PyMem_DEL(encoder);
}

static PyObject* 
_encode(ImagingEncoderObject* encoder, PyObject* args)
{
    PyObject* buf;
    int bufsize;
    int status;

    bufsize = 16384;

    if (!PyArg_ParseTuple(args, "|i", &bufsize))
	return NULL;

    buf = PyString_FromStringAndSize(NULL, bufsize);
    if (!buf)
	return NULL;

    status = encoder->encode(encoder->im, &encoder->state,
			     (UINT8*) PyString_AsString(buf), bufsize);

    return Py_BuildValue("iiO", status, encoder->state.errcode, buf);
}

extern Imaging PyImaging_AsImaging(PyObject *op);

static PyObject*
_setimage(ImagingEncoderObject* encoder, PyObject* args)
{
    PyObject* op;
    Imaging im;
    ImagingCodecState state;
    int x0, y0, x1, y1;

    x0 = y0 = x1 = y1 = 0;

    /* FIXME: should publish the ImagingType descriptor */
    if (!PyArg_ParseTuple(args, "O|(iiii)", &op, &x0, &y0, &x1, &y1))
	return NULL;
    im = PyImaging_AsImaging(op);
    if (!im)
	return NULL;

    encoder->im = im;

    state = &encoder->state;

    if (x0 == 0 && x1 == 0) {
	state->xsize = im->xsize;
	state->ysize = im->ysize;
    } else {
	state->xoff = x0;
	state->yoff = y0;
	state->xsize = x1 - x0;
	state->ysize = y1 - y0;
    }

    if (state->xsize <= 0 ||
	state->xsize + state->xoff > im->xsize ||
	state->ysize <= 0 ||
	state->ysize + state->yoff > im->ysize) {
	PyErr_SetString(PyExc_SystemError, "illegal tile extent");
	return NULL;
    }

    /* Allocate memory buffer (if bits field is set) */
    if (state->bits > 0) {
	state->bytes = (state->bits * state->xsize+7)/8;
	state->buffer = (UINT8*) malloc(state->bytes);
	if (!state->buffer) {
	    PyErr_NoMemory();
	    return NULL;
	}
    }

    /* Keep a reference to the image object, to make sure it doesn't
       go away before we do */
    Py_INCREF(op);
    encoder->lock = op;

    Py_INCREF(Py_None);
    return Py_None;
}

static struct PyMethodDef methods[] = {
    {"encode", (PyCFunction)_encode, 1},
    {"setimage", (PyCFunction)_setimage, 1},
    {NULL, NULL} /* sentinel */
};

static PyObject*  
_getattr(ImagingEncoderObject* self, char* name)
{
    return Py_FindMethod(methods, (PyObject*) self, name);
}

statichere PyTypeObject ImagingEncoderType = {
	PyObject_HEAD_INIT(NULL)
	0,				/*ob_size*/
	"ImagingEncoder",		/*tp_name*/
	sizeof(ImagingEncoderObject),	/*tp_size*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)_dealloc,		/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)_getattr,		/*tp_getattr*/
	0,				/*tp_setattr*/
	0,				/*tp_compare*/
	0,				/*tp_repr*/
	0,                              /*tp_hash*/
};

/* -------------------------------------------------------------------- */

int
get_packer(ImagingEncoderObject* encoder, const char* rawmode)
{
    int bits;
    ImagingShuffler pack;

    pack = ImagingFindPacker(rawmode, &bits);
    if (!pack) {
	Py_DECREF(encoder);
	PyErr_SetString(PyExc_SystemError, "unknown raw mode");
	return -1;
    }

    encoder->state.shuffle = pack;
    encoder->state.bits = bits;

    return 0;
}

/* -------------------------------------------------------------------- */
/* EPS									*/
/* -------------------------------------------------------------------- */

PyObject*
PyImaging_EpsEncoderNew(PyObject* self, PyObject* args)
{
    ImagingEncoderObject* encoder;

    encoder = PyImaging_EncoderNew(0);
    if (encoder == NULL)
	return NULL;

    encoder->encode = ImagingEpsEncode;

    return (PyObject*) encoder;
}


/* -------------------------------------------------------------------- */
/* RAW									*/
/* -------------------------------------------------------------------- */

PyObject*
PyImaging_RawEncoderNew(PyObject* self, PyObject* args)
{
    ImagingEncoderObject* encoder;
    char *rawmode;
    int stride, ystep;

    stride = 0;
    ystep = 1;

    if (!PyArg_ParseTuple(args, "s|ii", &rawmode, &stride, &ystep))
	return NULL;

    if (stride != 0) {
	PyErr_SetString(PyExc_SystemError,
			"non-zero stride not yet supported");
	return NULL;
    }

    encoder = PyImaging_EncoderNew(0);
    if (encoder == NULL)
	return NULL;

    if (get_packer(encoder, rawmode) < 0)
	return NULL;

    encoder->encode = ImagingRawEncode;

    encoder->state.ystep = ystep;

    return (PyObject*) encoder;
}

/* -------------------------------------------------------------------- */
/* XBM									*/
/* -------------------------------------------------------------------- */

PyObject*
PyImaging_XbmEncoderNew(PyObject* self, PyObject* args)
{
    ImagingEncoderObject* encoder;

    encoder = PyImaging_EncoderNew(0);
    if (encoder == NULL)
	return NULL;

    if (get_packer(encoder, "1;R") < 0)
	return NULL;

    encoder->encode = ImagingXbmEncode;

    return (PyObject*) encoder;
}


/* -------------------------------------------------------------------- */
/* JPEG									*/
/* -------------------------------------------------------------------- */

#ifdef HAVE_LIBJPEG

/* We better define this encoder last in this file, so the following
   undef's won't mess things up for the Imaging library proper. */

#undef	HAVE_PROTOTYPES
#undef	HAVE_STDDEF_H
#undef	HAVE_STDLIB_H
#undef	UINT8
#undef	UINT16
#undef	UINT32
#undef	INT16
#undef	INT32

#include "Jpeg.h"

PyObject*
PyImaging_JpegEncoderNew(PyObject* self, PyObject* args)
{
    ImagingEncoderObject* encoder;
    char *rawmode;

    if (!PyArg_ParseTuple(args, "s", &rawmode))
	return NULL;

    encoder = PyImaging_EncoderNew(sizeof(JPEGENCODERSTATE));
    if (encoder == NULL)
	return NULL;

    if (get_packer(encoder, rawmode) < 0)
	return NULL;

    encoder->encode = ImagingJpegEncode;

    return (PyObject*) encoder;
}

#endif
