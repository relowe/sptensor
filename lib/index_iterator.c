
/*
    This file contains the interface for a generic index-based
    iterator. This iterator configuration should be almost complete for
    most tensor types.
    Copyright (C) 2020  Robert Lowe <pngwen@acm.org>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */
#include <stdlib.h>
#include <sptensor/sptensor.h>
#include <sptensor/index_iterator.h>


/* Allocate an sptensor index iterator */
sptensor_iterator_t* sptensor_index_iterator_alloc(sptensor_t *t)
{
    sptensor_iterator_t *itr;
    int i;

    /* allocate the iterator */
    itr = malloc(sizeof(sptensor_iterator_t));
    if(!itr) {
        return NULL;
    }

    /* fill in the values we can so far */
    itr->t = t;

    /* create the index */
    itr->index = sptensor_index_alloc(t->modes);
    if(!itr->index) {
        sptensor_index_iterator_free(itr);
        return NULL;
    }

    /* initialize the index to point to the first index */
    for(i=0; i<t->modes; i++) {
        itr->index[i] = 1;
    }

    /* populate the functions */
    itr->valid = sptensor_index_iterator_valid;
    itr->next = sptensor_index_iterator_next;
    itr->prev = sptensor_index_iterator_prev;
    itr->free = (sptensor_free_f) sptensor_index_iterator_free;
}


/* Free an sptensor index iterator */
void sptensor_index_iterator_free(sptensor_iterator_t *itr)
{
    sptensor_index_free(itr->index);
    free(itr);
}


/* Advance the iterator */
int sptensor_index_iterator_next(sptensor_iterator_t *itr)
{
    /* increment the index */
    sptensor_index_inc(itr->t->modes, itr->t->dim, itr->index);

    /* return validity */
    return sptensor_iterator_valid(itr);
}


/* Go back one step */
int sptensor_index_iterator_prev(sptensor_iterator_t *itr)
{
    /* decrement the index */
    sptensor_index_dec(itr->t->modes, itr->t->dim, itr->index);

    /* return validity */
    return sptensor_iterator_valid(itr);
}


/* Check that the iterator is valid */
int sptensor_index_iterator_valid(sptensor_iterator_t *itr)
{
    int i;

    /* if index > dim , invalid! */
    if(sptensor_index_cmp(itr->t->modes, itr->index, itr->t->dim) > 0) {
        return 0;
    }

    /* if any index is less than 1, it is invalid */
    for(i=0; i<itr->t->modes; i++) {
        if(itr->index[i] < 1) {
            return 0;
        }
    }

    /* if we make it this far, all is well */
    return 1;
}