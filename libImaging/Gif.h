/* $Id: Gif.h,v 1.1 1996/04/14 18:58:28 fredrik Exp $
 *
 * The Python Imaging Library.
 *
 * File:
 *	Gif.h -- Declarations for a fast, suspendable GIF decoder.
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


/* Max size for a LZW code word. */

#define	GIFBITS	    12

#define	GIFTABLE    (1<<GIFBITS)
#define	GIFSTACK    (1<<GIFBITS)


typedef struct {

    /* CONFIGURATION */

    /* Initial number of bits. The caller should clear all fields in
       this structure and set this field before calling the decoder
       the first time. */
    int bits;

    /* If set, this is an interlaced image.  Process it the following way:
     * 1st pass: start at top line, lines are 8 pixels high, step 8 pixels
     * 2nd pass: start at line 4, lines are 4 pixels high, step 8 pixels
     * 3rd pass: start at line 2, lines are 2 pixels high, step 4 pixels
     * 4th pass: start at line 1, lines are 1 pixels high, step 2 pixels
     */
    /* FIXME: interlaced images are not yet supported */
    int interlace;

    /* PRIVATE CONTEXT (set by decoder) */

    /* Interlace parameters */
    int step, repeat;

    /* Input bit buffer */
    INT32 bitbuffer;
    int bitcount;
    int blocksize;

    /* Code buffer */
    int codesize;
    int codemask;

    /* Constant symbol codes */
    int clear, end;

    /* Symbol history */
    int lastcode;
    unsigned char lastdata;

    /* Stack */
    int stackindex;
    unsigned char stack[GIFTABLE];

    /* Symbol table */
    unsigned INT16 link[GIFTABLE];
    unsigned char data[GIFTABLE];
    int next;

} GIFSTATE;
