/* $Id: display.c,v 1.3 1996/12/08 15:32:19 fl Exp $
 *
 * The Python Imaging Library.
 *
 * File:
 *	display.c -- display support
 *
 * History:
 *	96-05-13 fl	Windows DIB support
 *	96-05-21 fl	Added palette stuff
 *	96-05-28 fl	Added display_mode stuff
 *
 * Copyright (c) Fredrik Lundh 1996.  All rights reserved.
 */


#include "Python.h"
#include "Imaging.h"


/* -------------------------------------------------------------------- */
/* Windows DIB support							*/
/* -------------------------------------------------------------------- */

#ifdef	WIN32

#include "ImDib.h"

typedef struct {
    PyObject_HEAD
    ImagingDIB dib;
} ImagingDisplayObject;

staticforward PyTypeObject ImagingDisplayType;

static ImagingDisplayObject*
_new(const char* mode, int xsize, int ysize)
{
    ImagingDisplayObject *display;

    display = PyObject_NEW(ImagingDisplayObject, &ImagingDisplayType);
    if (display == NULL)
	return NULL;

    display->dib = ImagingNewDIB(mode, xsize, ysize);
    if (!display->dib) {
	Py_DECREF(display);
	return NULL;
    }

    return display;
}

static void
_delete(ImagingDisplayObject* display)
{
    if (display->dib)
	ImagingDeleteDIB(display->dib);
    PyMem_DEL(display);
}

static PyObject* 
_expose(ImagingDisplayObject* display, PyObject* args)
{
    int hdc;

    if (!PyArg_ParseTuple(args, "i", &hdc))
	return NULL;

    ImagingExposeDIB(display->dib, hdc);

    Py_INCREF(Py_None);
    return Py_None;
}

extern Imaging PyImaging_AsImaging(PyObject *op);

static PyObject*
_paste(ImagingDisplayObject* display, PyObject* args)
{
    PyObject* op;
    Imaging im;
    int x0, y0, x1, y1;

    x0 = y0 = x1 = y1 = 0;

    /* FIXME: should publish the ImagingType descriptor */
    if (!PyArg_ParseTuple(args, "O|(iiii)", &op, &x0, &y0, &x1, &y1))
	return NULL;
    im = PyImaging_AsImaging(op);
    if (!im)
	return NULL;

    if (x1 <= x0)
	x1 = x0 + im->xsize;
    if (y1 <= y0)
	y1 = y0 + im->ysize;

    ImagingPasteDIB(display->dib, im, x0, y0, x1, y1);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* 
_query_palette(ImagingDisplayObject* display, PyObject* args)
{
    int hdc;
    int status;

    if (!PyArg_ParseTuple(args, "i", &hdc))
	return NULL;

    status = ImagingQueryPaletteDIB(display->dib, hdc);

    return Py_BuildValue("i", status);
}

static struct PyMethodDef methods[] = {
    {"expose", (PyCFunction)_expose, 1},
    {"paste", (PyCFunction)_paste, 1},
    {"query_palette", (PyCFunction)_query_palette, 1},
    {NULL, NULL} /* sentinel */
};

static PyObject*  
_getattr(ImagingDisplayObject* self, char* name)
{
    PyObject* res;

    res = Py_FindMethod(methods, (PyObject*) self, name);
    if (res)
	return res;
    PyErr_Clear();
    if (!strcmp(name, "mode"))
	return Py_BuildValue("s", self->dib->mode);
    if (!strcmp(name, "size"))
	return Py_BuildValue("ii", self->dib->xsize, self->dib->ysize);
    PyErr_SetString(PyExc_AttributeError, name);
    return NULL;
}

statichere PyTypeObject ImagingDisplayType = {
	PyObject_HEAD_INIT(NULL)
	0,				/*ob_size*/
	"ImagingDisplay",		/*tp_name*/
	sizeof(ImagingDisplayObject),	/*tp_size*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)_delete,		/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)_getattr,		/*tp_getattr*/
	0,				/*tp_setattr*/
	0,				/*tp_compare*/
	0,				/*tp_repr*/
	0,                              /*tp_hash*/
};

PyObject*
PyImaging_DisplayWin32(PyObject* self, PyObject* args)
{
    ImagingDisplayObject* display;
    char *mode;
    int xsize, ysize;

    if (!PyArg_ParseTuple(args, "s(ii)", &mode, &xsize, &ysize))
	return NULL;

    display = _new(mode, xsize, ysize);
    if (display == NULL)
	return NULL;

    return (PyObject*) display;
}

PyObject*
PyImaging_DisplayModeWin32(PyObject* self, PyObject* args)
{
    char *mode;
    int size[2];

    mode = ImagingGetModeDIB(size);

    return Py_BuildValue("s(ii)", mode, size[0], size[1]);
}

#endif /* WIN32 */
