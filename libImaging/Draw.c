/*
 * The Python Imaging Library.
 * $Id$
 *
 * a simple drawing package for the Imaging library
 *
 * history:
 *	96-04-13 fl	Created.
 *	96-04-30 fl	Added transforms and polygon support.
 *	96-08-12 fl	Added filled polygons.
 *	96-11-05 fl	Fixed float/int confusion in polygon filler
 *	97-07-04 fl	Support 32-bit images (C++ would have been nice)
 *
 * Copyright (c) Fredrik Lundh 1996-97.
 * Copyright (c) Secret Labs AB 1997.
 *
 * See the README file for information on usage and redistribution.
 */

#include <math.h>

#include "Imaging.h"

#ifdef macintosh
typedef int (*macqsortfuncptr)(const void *, const void *);
#define QSORT_CAST (macqsortfuncptr)
#else
#define QSORT_CAST /**/
#endif /* macintosh */

static inline void
ImagingAffineMatrixDo(ImagingAffineMatrix transform, int* x_out, int* y_out,
		      float X, float Y)
{
    *x_out = transform->a[0] * X + transform->a[1] * Y + transform->a[2] + 0.5;
    *y_out = transform->a[3] * X + transform->a[4] * Y + transform->a[5] + 0.5;
}

/* -------------------------------------------------------------------- */
/* Helpers								*/
/* -------------------------------------------------------------------- */

typedef struct {
    int x;
    int y;
} Pt;

typedef struct {
    int d;
    int x0, y0;
    int xmin, ymin, xmax, ymax;
    float slope;
} Edge;

static inline void
point8(Imaging im, int x, int y, int ink)
{
    if (x >= 0 && x < im->xsize && y >= 0 && y < im->ysize)
	im->image8[y][x] = (UINT8) ink;
}

static inline void
point32(Imaging im, int x, int y, int ink)
{
    if (x >= 0 && x < im->xsize && y >= 0 && y < im->ysize)
	im->image32[y][x] = ink;
}

static inline void
hline8(Imaging im, int x0, int y0, int x1, int ink)
{
    int tmp;

    if (y0 >= 0 && y0 < im->ysize) {
	if (x0 > x1)
	    tmp = x0, x0 = x1, x1 = tmp;
	if (x0 < 0)
	    x0 = 0;
	else if (x0 >= im->xsize)
	    return;
	if (x1 < 0)
	    return;
	else if (x1 >= im->xsize)
	    x1 = im->xsize-1;
	if (x0 <= x1)
	    memset(im->image8[y0] + x0, (UINT8) ink, x1 - x0 + 1);
    }
}

static inline void
hline32(Imaging im, int x0, int y0, int x1, int ink)
{
    int tmp;
    INT32* p;

    if (y0 >= 0 && y0 < im->ysize) {
	if (x0 > x1)
	    tmp = x0, x0 = x1, x1 = tmp;
	if (x0 < 0)
	    x0 = 0;
	else if (x0 >= im->xsize)
	    return;
	if (x1 < 0)
	    return;
	else if (x1 >= im->xsize)
	    x1 = im->xsize-1;
        p = im->image32[y0];
	while (x0 <= x1)
            p[x0++] = ink;
    }
}

static inline void
line8(Imaging im, int x0, int y0, int x1, int y1, int ink)
{
    int i, n, e;
    int dx, dy;
    int xs, ys;

    /* normalize coordinates */
    dx = x1-x0;
    if (dx < 0)
	dx = -dx, xs = -1;
    else
	xs = 1;
    dy = y1-y0;
    if (dy < 0)
	dy = -dy, ys = -1;
    else
	ys = 1;

    n = (dx > dy) ? dx : dy;

    if (dx == 0)

	/* vertical */
	for (i = 0; i < dy; i++) {
	    point8(im, x0, y0, ink);
	    y0 += ys;
	}

    else if (dy == 0)

	/* horizontal */
	hline8(im, x0, y0, x1, ink);

    else if (dx > dy) {

	/* bresenham, horizontal slope */
	n = dx;
	dy += dy;
	e = dy - dx;
	dx += dx;

	for (i = 0; i < n; i++) {
	    point8(im, x0, y0, ink);
	    if (e >= 0) {
		y0 += ys;
		e -= dx;
	    }
	    e += dy;
	    x0 += xs;
	}

    } else {

	/* bresenham, vertical slope */
	n = dy;
	dx += dx;
	e = dx - dy;
	dy += dy;

	for (i = 0; i < n; i++) {
	    point8(im, x0, y0, ink);
	    if (e >= 0) {
		x0 += xs;
		e -= dy;
	    }
	    e += dx;
	    y0 += ys;
	}

    }
}


static inline void
line32(Imaging im, int x0, int y0, int x1, int y1, int ink)
{
    int i, n, e;
    int dx, dy;
    int xs, ys;

    /* normalize coordinates */
    dx = x1-x0;
    if (dx < 0)
	dx = -dx, xs = -1;
    else
	xs = 1;
    dy = y1-y0;
    if (dy < 0)
	dy = -dy, ys = -1;
    else
	ys = 1;

    n = (dx > dy) ? dx : dy;

    if (dx == 0)

	/* vertical */
	for (i = 0; i < dy; i++) {
	    point32(im, x0, y0, ink);
	    y0 += ys;
	}

    else if (dy == 0)

	/* horizontal */
	hline32(im, x0, y0, x1, ink);

    else if (dx > dy) {

	/* bresenham, horizontal slope */
	n = dx;
	dy += dy;
	e = dy - dx;
	dx += dx;

	for (i = 0; i < n; i++) {
	    point32(im, x0, y0, ink);
	    if (e >= 0) {
		y0 += ys;
		e -= dx;
	    }
	    e += dy;
	    x0 += xs;
	}

    } else {

	/* bresenham, vertical slope */
	n = dy;
	dx += dx;
	e = dx - dy;
	dy += dy;

	for (i = 0; i < n; i++) {
	    point32(im, x0, y0, ink);
	    if (e >= 0) {
		x0 += xs;
		e -= dy;
	    }
	    e += dx;
	    y0 += ys;
	}

    }
}

typedef int (*cmpfunc)(const void *, const void *);

static int
_cmp(const float *y0, const float *y1)
{
    return y0[0] - y1[0];
}

static inline void
polygon8(Imaging im, int n, Edge *e, int ink)
{
    int i, j;
    float *xx;
    int ymin, ymax;
    float y;

    if (n <= 0)
	return;

    /* Find upper and lower polygon boundary (within image) */

    ymin = e[0].ymin;
    ymax = e[0].ymax;
    for (i = 1; i < n; i++) {
	if (e[i].ymin < ymin) ymin = e[i].ymin;
	if (e[i].ymax > ymax) ymax = e[i].ymax;
    }

    if (ymin < 0)
	ymin = 0;
    if (ymax >= im->ysize)
	ymax = im->ysize-1;

    /* Process polygon edges */

    xx = malloc(n * sizeof(float));
    if (!xx)
	return; /* FIXME: return error code */

    /* Pretty neat, in my humble opinion */
    for (;ymin <= ymax; ymin++) {
	y = ymin+0.5;
	for (i = j = 0; i < n; i++) 
	    if (y >= e[i].ymin && y <= e[i].ymax)
		if (e[i].d == 0)
		    hline8(im, e[i].xmin, (int) floor(y), e[i].xmax, ink);
		else
		    xx[j++] = (y-e[i].y0) * e[i].slope + e[i].x0;
	if (j == 2) {
	    hline8(im, (int) ceil(xx[0]-0.5), (int) floor(y),
                   (int) floor(xx[1]+0.5), ink);
	} else {
	    /* Even-odd polygon fill. */
	    qsort(xx, j, sizeof(float), (cmpfunc) _cmp);
	    for (i = 0; i < j-1 ; i += 2)
		hline8(im, (int) ceil(xx[i]-0.5), (int) floor(y),
                       (int) floor(xx[i+1]+0.5), ink);
	}
    }

    free(xx);
}

static inline void
polygon32(Imaging im, int n, Edge *e, int ink)
{
    int i, j;
    float *xx;
    int ymin, ymax;
    float y;

    if (n <= 0)
	return;

    /* Find upper and lower polygon boundary (within image) */

    ymin = e[0].ymin;
    ymax = e[0].ymax;
    for (i = 1; i < n; i++) {
	if (e[i].ymin < ymin) ymin = e[i].ymin;
	if (e[i].ymax > ymax) ymax = e[i].ymax;
    }

    if (ymin < 0)
	ymin = 0;
    if (ymax >= im->ysize)
	ymax = im->ysize-1;

    /* Process polygon edges */

    xx = malloc(n * sizeof(float));
    if (!xx)
	return; /* FIXME: return error code */

    /* Pretty neat, in my humble opinion */
    for (;ymin <= ymax; ymin++) {
	y = ymin+0.5;
	for (i = j = 0; i < n; i++) 
	    if (y >= e[i].ymin && y <= e[i].ymax)
		if (e[i].d == 0)
		    hline32(im, e[i].xmin, (int) floor(y), e[i].xmax, ink);
		else
		    xx[j++] = (y-e[i].y0) * e[i].slope + e[i].x0;
	if (j == 2) {
	    hline32(im, (int) ceil(xx[0]-0.5), (int) floor(y),
                    (int) floor(xx[1]+0.5), ink);
	} else {
	    /* Even-odd polygon fill. */
	    qsort(xx, j, sizeof(float), (cmpfunc) _cmp);
	    for (i = 0; i < j-1 ; i += 2)
                hline32(im, (int) ceil(xx[i]-0.5), (int) floor(y),
                        (int) floor(xx[i+1]+0.5), ink);
	}
    }

    free(xx);
}

static inline void
add_edge(Edge *e, int x0, int y0, int x1, int y1)
{
    /* FIXME: might use bresenham instead */

    if (x0 <= x1)
	e->xmin = x0, e->xmax = x1;
    else
	e->xmin = x1, e->xmax = x0;

    if (y0 <= y1)
	e->ymin = y0, e->ymax = y1;
    else
	e->ymin = y1, e->ymax = y0;
    
    if (y0 == y1) {
	e->d = 0;
	e->slope = 0.0;
    } else {
	e->slope = ((float)(x1-x0)) / (y1-y0);
	if (y0 == e->ymin)
	    e->d = 1;
	else
	    e->d = -1;
    }

    e->x0 = x0;
    e->y0 = y0;
}

typedef struct {
    void (*point)(Imaging im, int x, int y, int ink);
    void (*hline)(Imaging im, int x0, int y0, int x1, int ink);
    void (*line)(Imaging im, int x0, int y0, int x1, int y1, int ink);
    void (*polygon)(Imaging im, int n, Edge *e, int ink);
} DRAW;

DRAW draw8  = { point8,  hline8,  line8,  polygon8 };
DRAW draw32 = { point32, hline32, line32, polygon32 };


/* -------------------------------------------------------------------- */
/* Interface								*/
/* -------------------------------------------------------------------- */

int
ImagingDrawPoint(Imaging im, int x0, int y0, int ink)
{
    DRAW* draw = (im->image8) ? &draw8 : &draw32;

    draw->point(im, x0, y0, ink);

    return 0;
}

int
ImagingDrawLine(Imaging im, int x0, int y0, int x1, int y1, int ink)
{
    DRAW* draw = (im->image8) ? &draw8 : &draw32;

    draw->line(im, x0, y0, x1, y1, ink);

    return 0;
}

int
ImagingDrawRectangle(Imaging im, int x0, int y0, int x1, int y1,
		     int ink, int fill)
{
    int y;
    int tmp;
    DRAW* draw = (im->image8) ? &draw8 : &draw32;

    if (y0 > y1)
	tmp = y0, y0 = y1, y1 = tmp;

    if (fill) {

	for (y = y0; y < y1; y++)
	    draw->hline(im, x0, y, x1, ink);

    } else {

	/* outline */
	draw->line(im, x0, y0, x1, y0, ink);
	draw->line(im, x1, y0, x1, y1, ink);
	draw->line(im, x1, y1, x0, y1, ink);
	draw->line(im, x0, y1, x0, y0, ink);

    }

    return 0;
}

int
ImagingDrawPolygon(Imaging im, int count, int* xy, int ink, int fill)
{
    int i, n;
    Edge *e;
    DRAW* draw = (im->image8) ? &draw8 : &draw32;

    if (count <= 0)
	return 0;

    if (fill) {

	/* Build edge list */
	e = malloc(count * sizeof(Edge));
	if (!e) {
	    ImagingError_NoMemory();
	    return -1;
	}

	for (i = n = 0; i < count-1; i++)
	    add_edge(&e[n++], xy[i+i], xy[i+i+1], xy[i+i+2], xy[i+i+3]);

	if (xy[i+i] != xy[0] || xy[i+i+1] != xy[1])
	    /* close polygon */
	    add_edge(&e[n++], xy[i+i], xy[i+i+1], xy[0], xy[1]);

	draw->polygon(im, n, e, ink);

	free(e);

    } else {

	/* Outline */
	for (i = 0; i < count-1; i++)
	    draw->line(im, xy[i+i], xy[i+i+1], xy[i+i+2], xy[i+i+3], ink);
	draw->line(im, xy[i+i], xy[i+i+1], xy[0], xy[1], ink);

    }
	
    return 0;
}
