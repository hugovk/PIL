/*
 * THIS IS WORK IN PROGRESS
 *
 * The Python Imaging Library.
 * $Id: Draw.c,v 1.6 1996/11/07 12:11:30 fredrik Exp $
 *
 * a minimal drawing package for the Imaging library
 *
 * history:
 *	96-04-13 fl	Created.
 *	96-04-30 fl	Added transforms and polygon support.
 *	96-08-12 fl	Added filled polygons.
 *	96-11-05 fl	Fixed float/int confusion in polygon filler
 *
 * Copyright (c) Fredrik Lundh 1996.  All rights reserved.
 */

#include <math.h>

#include "Imaging.h"

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
draw_point(Imaging im, int x, int y, int ink)
{
    if (x >= 0 && x < im->xsize && y >= 0 && y < im->ysize)
	im->image8[y][x] = (UINT8) ink;
}


static inline void
draw_hline(Imaging im, int x0, int y0, int x1, int ink)
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
draw_line(Imaging im, int x0, int y0, int x1, int y1, int ink)
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
	    draw_point(im, x0, y0, ink);
	    y0 += ys;
	}

    else if (dy == 0)

	/* horizontal */
	draw_hline(im, x0, y0, x1, ink);

    else if (dx > dy) {

	/* bresenham, horizontal slope */
	n = dx;
	dy += dy;
	e = dy - dx;
	dx += dx;

	for (i = 0; i < n; i++) {
	    draw_point(im, x0, y0, ink);
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
	    draw_point(im, x0, y0, ink);
	    if (e >= 0) {
		x0 += xs;
		e -= dy;
	    }
	    e += dx;
	    y0 += ys;
	}

    }
}

static int
_cmp(const float *y0, const float *y1)
{
    return y0[0] - y1[0];
}

static inline void
draw_polygon(Imaging im, int n, Edge *e, int ink)
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
		    draw_hline(im, e[i].xmin, (int) floor(y), e[i].xmax, ink);
		else
		    xx[j++] = (y-e[i].y0) * e[i].slope + e[i].x0;
	if (j == 2) {
	    draw_hline(im, (int) ceil(xx[0]-0.5), (int) floor(y),
		       (int) floor(xx[1]+0.5), ink);
	} else {
	    /* Even-odd polygon fill. */
	    qsort(xx, j, sizeof(float), _cmp);
	    for (i = 0; i < j-1 ; i += 2)
		draw_hline(im, (int) ceil(xx[i]-0.5), (int) floor(y),
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


/* -------------------------------------------------------------------- */
/* Interface								*/
/* -------------------------------------------------------------------- */

int
ImagingDrawPoint(Imaging im, int x0, int y0, int ink)
{
    if (!im->image8) {
	ImagingError_Mismatch();
	return -1;
    }

    draw_point(im, x0, y0, ink);

    return 0;
}

int
ImagingDrawLine(Imaging im, int x0, int y0, int x1, int y1, int ink)
{
    if (!im->image8) {
	ImagingError_Mismatch();
	return -1;
    }

    draw_line(im, x0, y0, x1, y1, ink);

    return 0;
}

int
ImagingDrawRectangle(Imaging im, int x0, int y0, int x1, int y1,
		     int ink, int fill)
{
    int y;
    int tmp;

    if (!im->image8) {
	ImagingError_Mismatch();
	return -1;
    }

    if (y0 > y1)
	tmp = y0, y0 = y1, y1 = tmp;

    if (fill) {

	for (y = y0; y < y1; y++)
	    draw_hline(im, x0, y, x1, ink);

    } else {

	/* outline */
	draw_line(im, x0, y0, x1, y0, ink);
	draw_line(im, x1, y0, x1, y1, ink);
	draw_line(im, x1, y1, x0, y1, ink);
	draw_line(im, x0, y1, x0, y0, ink);

    }

    return 0;
}

int
ImagingDrawPolygon(Imaging im, int count, int* xy, int ink, int fill)
{
    int i, n;
    Edge *e;

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

	draw_polygon(im, n, e, ink);

	free(e);

    } else {

	/* Outline */
	for (i = 0; i < count-1; i++)
	    draw_line(im, xy[i+i], xy[i+i+1], xy[i+i+2], xy[i+i+3], ink);
	draw_line(im, xy[i+i], xy[i+i+1], xy[0], xy[1], ink);

    }
	
    return 0;
}
