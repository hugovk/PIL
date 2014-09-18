/*
 * The Python Imaging Library.
 * $Id: //modules/pil/path.c#4 $
 *
 * 2D path utilities
 *
 * history:
 * 1996-11-04 fl   Added to PIL (incomplete)
 * 1996-11-05 fl   Added sequence semantics
 * 1997-02-28 fl   Fixed getbbox
 * 1997-06-12 fl   Added id attribute
 * 1997-06-14 fl   Added slicing and setitem
 * 1998-12-29 fl   Improved sequence handling (from Richard Jones)
 * 1999-01-10 fl   Fixed IndexError test for 1.5 (from Fred Drake)
 * 2000-10-12 fl   Added special cases for tuples and lists
 * 2002-10-27 fl   Added clipping boilerplate
 *
 * notes:
 * FIXME: fill in remaining slots in the sequence api
 *
 * Copyright (c) 1997-2002 by Secret Labs AB
 * Copyright (c) 1997-2002 by Fredrik Lundh
 *
 * See the README file for information on usage and redistribution.
 */


#include <math.h>

#include "Python.h"

#if PY_VERSION_HEX < 0x01060000
#define PyObject_DEL(op) PyMem_DEL((op))
#endif


/* -------------------------------------------------------------------- */
/* Class								*/
/* -------------------------------------------------------------------- */

typedef struct {
    PyObject_HEAD
    int count;
    double *xy;
    int index; /* temporary use, e.g. in decimate */
} PyPathObject;

staticforward PyTypeObject PyPathType;

static PyPathObject*
path_new(int count, double* xy, int duplicate)
{
    PyPathObject *path;

    if (duplicate) {
        /* duplicate path */
        double* p;
        p = malloc(count * 2 * sizeof(double));
        if (!p) {
            PyErr_NoMemory();
            return NULL;
        }
        memcpy(p, xy, count * 2 * sizeof(double));
        xy = p;
    }

    path = PyObject_NEW(PyPathObject, &PyPathType);
    if (path == NULL)
	return NULL;

    path->count = count;
    path->xy = xy;

    return path;
}

static void
path_dealloc(PyPathObject* path)
{
    free(path->xy);
    PyObject_DEL(path);
}

/* -------------------------------------------------------------------- */
/* Helpers								*/
/* -------------------------------------------------------------------- */

#define PyPath_Check(op) ((op)->ob_type == &PyPathType)

int
PyPath_Flatten(PyObject* data, double **pxy)
{
    int i, j, n;
    double *xy;

    if (PyPath_Check(data)) {
	/* This was a path object. */

	PyPathObject *path = (PyPathObject*) data;

	n = 2 * path->count * sizeof(double);

	xy = malloc(n);
	if (!xy) {
	    PyErr_NoMemory();
	    return -1;
	}

	memcpy(xy, path->xy, n);

	*pxy = xy;
	return path->count;
    }
	
    if (!PySequence_Check(data)) {
	PyErr_SetString(PyExc_TypeError, "argument must be sequence");
	return -1;
    }

    j = 0;
    n = PyObject_Length(data);
    /* Just in case __len__ breaks (or doesn't exist) */
    if (PyErr_Occurred())
        return -1;

    /* Allocate for worst case */
    xy = malloc(2 * n * sizeof(double));
    if (!xy) {
	PyErr_NoMemory();
	return -1;
    }

    /* Copy table to path array */
    if (PyList_Check(data)) {
        for (i = 0; i < n; i++) {
            double x, y;
            PyObject *op = PyList_GET_ITEM(data, i);
            if (PyFloat_Check(op))
		xy[j++] = PyFloat_AS_DOUBLE(op);
            else if (PyInt_Check(op))
		xy[j++] = (float) PyInt_AS_LONG(op);
            else if (PyNumber_Check(op))
                xy[j++] = PyFloat_AsDouble(op);
            else if (PyArg_ParseTuple(op, "dd", &x, &y)) {
                xy[j++] = x;
                xy[j++] = y;
            } else {
                free(xy);
                return -1;
            }
        }
    } else if (PyTuple_Check(data)) {
        for (i = 0; i < n; i++) {
            double x, y;
            PyObject *op = PyTuple_GET_ITEM(data, i);
            if (PyFloat_Check(op))
		xy[j++] = PyFloat_AS_DOUBLE(op);
            else if (PyInt_Check(op))
		xy[j++] = (float) PyInt_AS_LONG(op);
            else if (PyNumber_Check(op))
                xy[j++] = PyFloat_AsDouble(op);
            else if (PyArg_ParseTuple(op, "dd", &x, &y)) {
                xy[j++] = x;
                xy[j++] = y;
            } else {
                free(xy);
                return -1;
            }
        }
    } else {
        for (i = 0; i < n; i++) {
            double x, y;
            PyObject *op = PySequence_GetItem(data, i);
            if (!op) {
                /* treat IndexError as end of sequence */
                if (PyErr_Occurred() &&
                    PyErr_ExceptionMatches(PyExc_IndexError)) {
                    PyErr_Clear();
                    break;
                } else {
                    free(xy);
                    return -1;
                }
            }
            if (PyFloat_Check(op))
		xy[j++] = PyFloat_AS_DOUBLE(op);
            else if (PyInt_Check(op))
		xy[j++] = (float) PyInt_AS_LONG(op);
            else if (PyNumber_Check(op))
                xy[j++] = PyFloat_AsDouble(op);
            else if (PyArg_ParseTuple(op, "dd", &x, &y)) {
                xy[j++] = x;
                xy[j++] = y;
            } else {
                Py_DECREF(op);
                free(xy);
                return -1;
            }
            Py_DECREF(op);
        }
    }

    if (j & 1) {
	PyErr_SetString(PyExc_ValueError, "wrong number of coordinates");
	free(xy);
	return -1;
    }

    *pxy = xy;
    return j/2;
}


/* -------------------------------------------------------------------- */
/* Factories								*/
/* -------------------------------------------------------------------- */

PyObject*
PyPath_Create(PyObject* self, PyObject* args)
{
    PyObject* data;
    int count;
    double *xy;

    if (PyArg_ParseTuple(args, "i", &count)) {

        /* number of vertices */
        xy = malloc(2 * count * sizeof(double));
        if (!xy) {
            PyErr_NoMemory();
            return NULL;
        }

    } else {

        /* sequence or other path */
        PyErr_Clear();
        if (!PyArg_ParseTuple(args, "O", &data))
            return NULL;

        count = PyPath_Flatten(data, &xy);
        if (count < 0)
            return NULL;
    }

    return (PyObject*) path_new(count, xy, 0);
}


/* -------------------------------------------------------------------- */
/* Methods								*/
/* -------------------------------------------------------------------- */

static PyObject*
path_compact(PyPathObject* self, PyObject* args)
{
    /* Simple-minded method to shorten path.  A point is removed if
       the city block distance to the previous point is less than the
       given distance */
    int i, j;
    double *xy;

    double cityblock = 2.0;

    if (!PyArg_ParseTuple(args, "|d", &cityblock))
	return NULL;

    xy = self->xy;

    /* remove bogus vertices */
    for (i = j = 1; i < self->count; i++) {
	if (fabs(xy[j+j-2]-xy[i+i]) + fabs(xy[j+j-1]-xy[i+i+1]) >= cityblock) {
	    xy[j+j] = xy[i+i];
	    xy[j+j+1] = xy[i+i+1];
	    j++;
	}
    }

    i = self->count - j;
    self->count = j;

    /* shrink coordinate array */
    realloc(self->xy, 2 * self->count * sizeof(double));

    return Py_BuildValue("i", i); /* number of removed vertices */
}

static PyObject*
path_clip_polygon(PyPathObject* self, PyObject* args)
{
    /* Clip path representing a single polygon */
    PyErr_SetString(PyExc_RuntimeError, "not yet implemented");
    return NULL;
}

static PyObject*
path_clip_polyline(PyPathObject* self, PyObject* args)
{
    /* Clip path representing a single polyline (outline) */
    PyErr_SetString(PyExc_RuntimeError, "not yet implemented");
    return NULL;
}

static PyObject*
path_getbbox(PyPathObject* self, PyObject* args)
{
    /* Find bounding box */
    int i;
    double *xy;
    double x0, y0, x1, y1;

    xy = self->xy;

    x0 = x1 = xy[0];
    y0 = y1 = xy[1];

    for (i = 1; i < self->count; i++) {
	if (xy[i+i] < x0)
	    x0 = xy[i+i];
	if (xy[i+i] > x1)
	    x1 = xy[i+i];
	if (xy[i+i+1] < y0)
	    y0 = xy[i+i+1];
	if (xy[i+i+1] > y1)
	    y1 = xy[i+i+1];
    }

    return Py_BuildValue("dddd", x0, y0, x1, y1);
}

static PyObject*
path_getitem(PyPathObject* self, int i)
{
    if (i < 0 || i >= self->count) {
	PyErr_SetString(PyExc_IndexError, "path index out of range");
	return NULL;
    }

    return Py_BuildValue("dd", self->xy[i+i], self->xy[i+i+1]);
}

static PyObject*
path_getslice(PyPathObject* self, int ilow, int ihigh)
{
    /* adjust arguments */
    if (ilow < 0)
        ilow = 0;
    else if (ilow >= self->count)
        ilow = self->count;
    if (ihigh < 0)
        ihigh = 0;
    if (ihigh < ilow)
        ihigh = ilow;
    else if (ihigh > self->count)
        ihigh = self->count;
    
    return (PyObject*) path_new(ihigh - ilow, self->xy + ilow * 2, 1);
}

static int
path_len(PyPathObject* self)
{
    return self->count;
}

static PyObject*
path_map(PyPathObject* self, PyObject* args)
{
    /* Map coordinate set through function */
    int i;
    double *xy;
    PyObject* function;

    if (!PyArg_ParseTuple(args, "O", &function))
	return NULL;

    xy = self->xy;

    /* apply function to coordinate set */
    for (i = 0; i < self->count; i++) {
	double x = xy[i+i];
	double y = xy[i+i+1];
	PyObject* item = PyObject_CallFunction(function, "dd", x, y);
	if (!item || !PyArg_ParseTuple(item, "dd", &x, &y)) {
	    Py_XDECREF(item);
	    return NULL;
	}
	xy[i+i] = x;
	xy[i+i+1] = y;
	Py_DECREF(item);
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static int
path_setitem(PyPathObject* self, int i, PyObject* op)
{
    double* xy;

    if (i < 0 || i >= self->count) {
        PyErr_SetString(PyExc_IndexError,
                        "path assignment index out of range");
        return -1;
    }

    if (op == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "cannot delete from path");
        return -1;
    }

    xy = &self->xy[i+i];

    if (!PyArg_ParseTuple(op, "dd", &xy[0], &xy[1]))
        return -1;

    return 0;
}

static PyObject*
path_tolist(PyPathObject* self, PyObject* args)
{
    PyObject *list;
    int i;

    list = PyList_New(self->count);

    for (i = 0; i < self->count; i++) {
	PyObject* item;
	item = Py_BuildValue("dd", self->xy[i+i], self->xy[i+i+1]);
	if (!item) {
	    Py_DECREF(list);
	    list = NULL;
	    break;
	}
	PyList_SetItem(list, i, item);
    }

    return list;
}

static PyObject*
path_transform(PyPathObject* self, PyObject* args)
{
    /* Apply affine transform to coordinate set */
    int i;
    double *xy;
    double a, b, c, d, e, f;

    double wrap = 0.0;

    if (!PyArg_ParseTuple(args, "(dddddd)|d",
			  &a, &b, &c, &d, &e, &f,
			  &wrap))
	return NULL;

    xy = self->xy;

    /* transform the coordinate set */
    if (b == 0.0 && d == 0.0)
	/* scaling */
	for (i = 0; i < self->count; i++) {
	    xy[i+i]   = a*xy[i+i]+c;
	    xy[i+i+1] = e*xy[i+i+1]+f;
	}
    else
	/* affine transform */
	for (i = 0; i < self->count; i++) {
	    double x = xy[i+i];
	    double y = xy[i+i+1];
	    xy[i+i]   = a*x+b*y+c;
	    xy[i+i+1] = d*x+e*y+f;
	}

    /* special treatment of geographical map data */
    if (wrap != 0.0)
	for (i = 0; i < self->count; i++)
	    xy[i+i] = fmod(xy[i+i], wrap);

    Py_INCREF(Py_None);
    return Py_None;
}

static struct PyMethodDef methods[] = {
    {"getbbox", (PyCFunction)path_getbbox, 1},
    {"tolist", (PyCFunction)path_tolist, 1},
    {"clip_polygon", (PyCFunction)path_clip_polygon, 1},
    {"clip_polyline", (PyCFunction)path_clip_polyline, 1},
    {"compact", (PyCFunction)path_compact, 1},
    {"map", (PyCFunction)path_map, 1},
    {"transform", (PyCFunction)path_transform, 1},
    {NULL, NULL} /* sentinel */
};

static PyObject*  
path_getattr(PyPathObject* self, char* name)
{
    PyObject* res;

    res = Py_FindMethod(methods, (PyObject*) self, name);
    if (res)
	return res;

    PyErr_Clear();

    if (strcmp(name, "id") == 0)
	return Py_BuildValue("l", (long) self->xy);

    PyErr_SetString(PyExc_AttributeError, name);
    return NULL;
}

static PySequenceMethods path_as_sequence = {
	(inquiry)path_len, /*sq_length*/
	(binaryfunc)0, /*sq_concat*/
	(intargfunc)0, /*sq_repeat*/
	(intargfunc)path_getitem, /*sq_item*/
	(intintargfunc)path_getslice, /*sq_slice*/
	(intobjargproc)path_setitem, /*sq_ass_item*/
	(intintobjargproc)0, /*sq_ass_slice*/
};

statichere PyTypeObject PyPathType = {
	PyObject_HEAD_INIT(NULL)
	0,				/*ob_size*/
	"Path",				/*tp_name*/
	sizeof(PyPathObject),		/*tp_size*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)path_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)path_getattr,	/*tp_getattr*/
	0,				/*tp_setattr*/
	0,				/*tp_compare*/
	0,				/*tp_repr*/
	0,                              /*tp_as_number */
	&path_as_sequence,              /*tp_as_sequence */
	0,                              /*tp_as_mapping */
	0,                              /*tp_hash*/
};
