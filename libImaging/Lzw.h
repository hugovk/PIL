/* $Id: Lzw.h,v 1.1 1996/05/09 21:20:43 fredrik Exp $
 *
 * The Python Imaging Library.
 *
 * File:
 *	Lzw.h -- Declarations for a fast, suspendable LZW decoder.
 *
 * Copyright (c) Fredrik Lundh 1995-96.  All rights reserved.
 */


/* Max size for LZW code words */

#define	LZWBITS	    12

#define	LZWTABLE    (1<<LZWBITS)
#define	LZWSTACK    (1<<LZWBITS)


typedef struct {

    /* PRIVATE CONTEXT (set by decoder) */

    /* Input bit buffer */
    INT32 bitbuffer;
    int bitcount;

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
    unsigned char stack[LZWTABLE];

    /* Symbol table */
    unsigned INT16 link[LZWTABLE];
    unsigned char data[LZWTABLE];
    int next;

} LZWSTATE;


extern int LzwDecode(LZWSTATE *state);

