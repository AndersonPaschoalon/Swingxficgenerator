#ifndef __HASHTABLE_PRIVATE_CWC22_H__
#define __HASHTABLE_PRIVATE_CWC22_H__

#include "hashtable_cwc22.h"

/*****************************************************************************/
struct entry
{
    void *k, *v;
    unsigned int h;
    struct entry *next;
};

struct hashtable {
    unsigned int tablelength;
    struct entry **table;
    unsigned int entrycount;
    unsigned int loadlimit;
    unsigned int (*hashfn) (void *k);
    int (*eqfn) (void *k1, void *k2);
};

unsigned int
hash(struct hashtable *h, void *k);

unsigned int
indexFor(unsigned int tablelength, unsigned int hashvalue);

#endif /* __HASHTABLE_PRIVATE_CWC22_H__*/

/*
 * Copyright (C) 2002 Christopher Clark <firstname.lastname@cl.cam.ac.uk>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software and its documentation and acknowledgment shall be
 * given in the documentation and software packages that this Software was
 * used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * */
