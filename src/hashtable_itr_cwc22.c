#include "hashtable_cwc22.h"
#include "hashtable_private_cwc22.h"
#include "hashtable_itr_cwc22.h"
#include <stdlib.h> /* defines NULL */

/*struct hashtable_itr
{
    struct hashtable *h;
    struct entry *e;
    unsigned int index;
};
*/

/*****************************************************************************/
/* hashtable_iterator    - iterator constructor */

struct hashtable_itr *
hashtable_iterator(struct hashtable *h)
{
    unsigned int i, tablelength;
    struct hashtable_itr *itr = (struct hashtable_itr *)
        malloc(sizeof(struct hashtable_itr));
    if (NULL == itr) return NULL;
    itr->h = h;
    itr->e = NULL;
    tablelength = h->tablelength;
    itr->index = tablelength;
    if (0 == h->entrycount) return itr;

    for (i = 0; i < tablelength; i++)
    {
        if (NULL != h->table[i])
        {
            itr->e = h->table[i];
            itr->index = i;
            break;
        }
    }
    return itr;
}

/*****************************************************************************/
/* key - return the key of the (key,value) pair at the current position */

/*
void *
hashtable_iterator_key(struct hashtable_itr *i)
{
    return i->e->k;
}
*/

/*****************************************************************************/
/* value - return the value of the (key,value) pair at the current position */

/*
void *
hashtable_iterator_value(struct hashtable_itr *i)
{
    return i->e->v;
}
*/

/*****************************************************************************/
/* advance - advance the iterator to the next element
 *           returns zero if advanced to end of table */

int
hashtable_iterator_advance(struct hashtable_itr *itr)
{
    unsigned int j,tablelength;
    struct entry **table;
    struct entry *next;
    if (NULL == itr->e) return 0; /* stupidity check */

    next = itr->e->next;
    if (NULL != next)
    {
        itr->e = next;
        return -1;
    }
    tablelength = itr->h->tablelength;
    if (tablelength <= (j = ++(itr->index)))
    {
        itr->e = NULL;
        return 0;
    }
    table = itr->h->table;
    while (NULL == (next = table[j]))
    {
        if (++j >= tablelength)
        {
            itr->index = tablelength;
            return 0;
        }
    }
    itr->index = j;
    itr->e = next;
    return -1;
}

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
