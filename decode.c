/* $Id: decode.c,v 1.4 1996/12/16 18:50:08 fl Exp fl $
 *
 * The Python Imaging Library.
 *
 * File:
 *	decoders.c -- standard decoder interfaces for the Imaging library
 *
 * History:
 *	96-03-28 fl	Moved from _imagingmodule.c
 *	96-04-15 fl	Support subregions in setimage
 *	96-04-19 fl	Allocate decoder buffer (where appropriate)
 *	96-05-02 fl	Added jpeg decoder
 *	96-05-12 fl	Compile cleanly as C++
 *	96-05-16 fl	Added hex decoder
 *	96-05-26 fl	Added JPEG configuration parameters
 *	96-12-14 fl	Added zip decoder
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


#include "Python.h"

#include "Imaging.h"

#include "Gif.h"
#include "Lzw.h"
#include "Raw.h"


/* -------------------------------------------------------------------- */
/* Common								*/
/* -------------------------------------------------------------------- */

typedef struct {
    PyObject_HEAD
    int (*decode)(Imaging im, ImagingCodecState state,
		  UINT8* buffer, int bytes);
    struct ImagingCodecStateInstance state;
    Imaging im;
    PyObject* lock;
} ImagingDecoderObject;

staticforward PyTypeObject ImagingDecoderType;

static ImagingDecoderObject*
PyImaging_DecoderNew(int contextsize)
{
    ImagingDecoderObject *decoder;
    void *context;

    decoder = PyObject_NEW(ImagingDecoderObject, &ImagingDecoderType);
    if (decoder == NULL)
	return NULL;

    /* Clear the decoder state */
    memset(&decoder->state, 0, sizeof(decoder->state));

    /* Allocate decoder context */
    if (contextsize > 0) {
	context = (void*) calloc(1, contextsize);
	if (!context) {
	    Py_DECREF(decoder);
	    PyErr_NoMemory();
	    return NULL;
	}
    } else
	context = 0;

    /* Initialize decoder context */
    decoder->state.context = context;

    /* Target image */
    decoder->lock = NULL;
    decoder->im = NULL;

    return decoder;
}

static void
_dealloc(ImagingDecoderObject* decoder)
{
    free(decoder->state.buffer);
    free(decoder->state.context);
    Py_XDECREF(decoder->lock);
    PyMem_DEL(decoder);
}

static PyObject* 
_decode(ImagingDecoderObject* decoder, PyObject* args)
{
    UINT8* buffer;
    int bufsize, status;

    if (!PyArg_ParseTuple(args, "s#", &buffer, &bufsize))
	return NULL;

    status = decoder->decode(decoder->im, &decoder->state, buffer, bufsize);

    return Py_BuildValue("ii", status, decoder->state.errcode);
}

extern Imaging PyImaging_AsImaging(PyObject *op);

static PyObject*
_setimage(ImagingDecoderObject* decoder, PyObject* args)
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

    decoder->im = im;

    state = &decoder->state;

    /* Setup decoding tile extent */
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
	state->xsize + state->xoff > (int) im->xsize ||
	state->ysize <= 0 ||
	state->ysize + state->yoff > (int) im->ysize) {
	PyErr_SetString(PyExc_ValueError, "illegal tile extent");
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
    decoder->lock = op;

    Py_INCREF(Py_None);
    return Py_None;
}

static struct PyMethodDef methods[] = {
    {"decode", (PyCFunction)_decode, 1},
    {"setimage", (PyCFunction)_setimage, 1},
    {NULL, NULL} /* sentinel */
};

static PyObject*  
_getattr(ImagingDecoderObject* self, char* name)
{
    return Py_FindMethod(methods, (PyObject*) self, name);
}

statichere PyTypeObject ImagingDecoderType = {
	PyObject_HEAD_INIT(NULL)
	0,				/*ob_size*/
	"ImagingDecoder",		/*tp_name*/
	sizeof(ImagingDecoderObject),	/*tp_size*/
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
get_unpacker(ImagingDecoderObject* decoder, const char* rawmode)
{
    int bits;
    ImagingShuffler unpack;

    unpack = ImagingFindUnpacker(rawmode, &bits);
    if (!unpack) {
	Py_DECREF(decoder);
	PyErr_SetString(PyExc_ValueError, "unknown raw mode");
	return -1;
    }

    decoder->state.shuffle = unpack;
    decoder->state.bits = bits;

    return 0;
}


/* -------------------------------------------------------------------- */
/* GIF									*/
/* -------------------------------------------------------------------- */

PyObject*
PyImaging_GifDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;
    int bits, interlace;

    bits = 8;
    interlace = 0;

    if (!PyArg_ParseTuple(args, "|ii", &bits, &interlace))
	return NULL;

    decoder = PyImaging_DecoderNew(sizeof(GIFSTATE));
    if (decoder == NULL)
	return NULL;

    decoder->decode = ImagingGifDecode;

    ((GIFSTATE*)decoder->state.context)->bits = bits;
    ((GIFSTATE*)decoder->state.context)->interlace = interlace;

    return (PyObject*) decoder;
}


/* -------------------------------------------------------------------- */
/* HEX									*/
/* -------------------------------------------------------------------- */

PyObject*
PyImaging_HexDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;
    char* rawmode;

    if (!PyArg_ParseTuple(args, "s", &rawmode))
	return NULL;

    decoder = PyImaging_DecoderNew(0);
    if (decoder == NULL)
	return NULL;

    if (get_unpacker(decoder, rawmode) < 0)
	return NULL;

    decoder->decode = ImagingHexDecode;

    return (PyObject*) decoder;
}


/* -------------------------------------------------------------------- */
/* LZW									*/
/* -------------------------------------------------------------------- */

PyObject*
PyImaging_LzwDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;
    char *rawmode;

    if (!PyArg_ParseTuple(args, "s", &rawmode))
	return NULL;

    decoder = PyImaging_DecoderNew(sizeof(LZWSTATE));
    if (decoder == NULL)
	return NULL;

    if (get_unpacker(decoder, rawmode) < 0)
	return NULL;

    decoder->decode = ImagingLzwDecode;

    return (PyObject*) decoder;
}


/* -------------------------------------------------------------------- */
/* PackBits								*/
/* -------------------------------------------------------------------- */

PyObject*
PyImaging_PackbitsDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;
    char *rawmode;

    if (!PyArg_ParseTuple(args, "s", &rawmode))
	return NULL;

    decoder = PyImaging_DecoderNew(0);
    if (decoder == NULL)
	return NULL;

    if (get_unpacker(decoder, rawmode) < 0)
	return NULL;

    decoder->decode = ImagingPackbitsDecode;

    return (PyObject*) decoder;
}


/* -------------------------------------------------------------------- */
/* PCD									*/
/* -------------------------------------------------------------------- */

PyObject*
PyImaging_PcdDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;

    decoder = PyImaging_DecoderNew(0);
    if (decoder == NULL)
	return NULL;

    decoder->decode = ImagingPcdDecode;

    return (PyObject*) decoder;
}


/* -------------------------------------------------------------------- */
/* PCX									*/
/* -------------------------------------------------------------------- */

PyObject*
PyImaging_PcxDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;
    char *rawmode;

    if (!PyArg_ParseTuple(args, "s", &rawmode))
	return NULL;

    decoder = PyImaging_DecoderNew(0);
    if (decoder == NULL)
	return NULL;

    if (get_unpacker(decoder, rawmode) < 0)
	return NULL;

    decoder->decode = ImagingPcxDecode;

    return (PyObject*) decoder;
}


/* -------------------------------------------------------------------- */
/* RAW									*/
/* -------------------------------------------------------------------- */

PyObject*
PyImaging_RawDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;
    char *rawmode;
    int stride, ystep;

    stride = 0;
    ystep = 1;

    if (!PyArg_ParseTuple(args, "s|ii", &rawmode, &stride, &ystep))
	return NULL;

    decoder = PyImaging_DecoderNew(sizeof(RAWSTATE));
    if (decoder == NULL)
	return NULL;

    if (get_unpacker(decoder, rawmode) < 0)
	return NULL;

    decoder->decode = ImagingRawDecode;

    decoder->state.ystep = ystep;

    ((RAWSTATE*)decoder->state.context)->stride = stride;

    return (PyObject*) decoder;
}

/* -------------------------------------------------------------------- */
/* XBM									*/
/* -------------------------------------------------------------------- */

PyObject*
PyImaging_XbmDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;

    decoder = PyImaging_DecoderNew(0);
    if (decoder == NULL)
	return NULL;

    if (get_unpacker(decoder, "1;R") < 0)
	return NULL;

    decoder->decode = ImagingXbmDecode;

    return (PyObject*) decoder;
}


/* -------------------------------------------------------------------- */
/* ZIP									*/
/* -------------------------------------------------------------------- */

#ifdef HAVE_LIBZ

#include "Zip.h"

PyObject*
PyImaging_ZipDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;
    char *rawmode;

    if (!PyArg_ParseTuple(args, "s", &rawmode))
	return NULL;

    decoder = PyImaging_DecoderNew(sizeof(ZIPSTATE));
    if (decoder == NULL)
	return NULL;

    if (get_unpacker(decoder, rawmode) < 0)
	return NULL;

    decoder->decode = ImagingZipDecode;

    return (PyObject*) decoder;
}
#endif

/* -------------------------------------------------------------------- */
/* JPEG									*/
/* -------------------------------------------------------------------- */

#ifdef HAVE_LIBJPEG

/* We better define this decoder last in this file, so the following
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
PyImaging_JpegDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;
    char *rawmode;
    int scale, draft;

    scale = 1;
    draft = 0;

    if (!PyArg_ParseTuple(args, "s|iii", &rawmode, &scale, &draft))
	return NULL;

    decoder = PyImaging_DecoderNew(sizeof(JPEGSTATE));
    if (decoder == NULL)
	return NULL;

    if (get_unpacker(decoder, rawmode) < 0)
	return NULL;

    decoder->decode = ImagingJpegDecode;

    strncpy(((JPEGSTATE*)decoder->state.context)->rawmode, rawmode, 8);

    ((JPEGSTATE*)decoder->state.context)->scale = scale;
    ((JPEGSTATE*)decoder->state.context)->draft = draft;

    return (PyObject*) decoder;
}
#endif
