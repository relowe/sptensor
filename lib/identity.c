/*
    This is the header file for identity tensors in sptensor.
    The sptensor library provides a set of functions and data type for
    dealing with sparse tensors.

    Copyright (C) 2021  Robert Lowe <pngwen@acm.org>

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
#include <sptensor/identity.h>
#include <sptensor/index_iterator.h>

/* iterator helper prototypes */
static int sptensor_identity_nz_iterator_next(sptensor_iterator_t *itr);
static int sptensor_identity_nz_iterator_prev(sptensor_iterator_t *itr);

/* allocation functions */
sptensor_t *sptensor_identity_alloc(sptensor_index_t *modes, int nmodes)
{
    sptensor_identity_t *result;

    /* allocate the result */
    result = malloc(sizeof(sptensor_identity_t));
    if(!result) { return NULL; }
    result->dim = NULL;

    /* initialize the modes */
    result->dim = sptensor_index_alloc(nmodes);
    if(!result->dim) {
        sptensor_identity_free(result);
        return NULL;
    }

    /* copy the dimensions */
    sptensor_index_cpy(nmodes, result->dim, modes);

    /* populate the fields */
    result->modes = nmodes; 
    result->get = (sptensor_get_f) sptensor_identity_get;
    result->set = NULL;
    result->iterator = sptensor_index_iterator_alloc;
    result->nz_iterator = (sptensor_iterator_f) sptensor_identity_nz_iterator;
    result->free = (sptensor_free_f) sptensor_identity_free;
}


void sptensor_identity_free(sptensor_identity_t *t)
{
    /* deallocate dimensions */
    if(t->dim) {
        sptensor_index_free(t->dim);
    }

    /* deallocate the struct itself */
    free(t);
}

/* get function (read only) */
void sptensor_identity_get(sptensor_identity_t *t, sptensor_index_t *idx, mpf_t v)
{
    int i;

    /* if it is a diagonal point, return 1.0, otherwise 0.0 */
    for(i=1; i<t->modes; i++) {
        if(idx[0] != idx[i]) {
            mpf_set_d(v, 0.0);
            return;
        }
    }
    mpf_set_d(v, 1.0);

    return;
}



/* create an nz iterator */
sptensor_iterator_t *sptensor_identity_nz_iterator(sptensor_identity_t *t)
{
    sptensor_iterator_t *result;

    /* the index iterator is almost it */
    result = sptensor_index_iterator_alloc(t);

    /* we want to move through the indexes differently, however */
    result->next = sptensor_identity_nz_iterator_next;
    result->prev = sptensor_identity_nz_iterator_prev;
}


static int sptensor_identity_nz_iterator_next(sptensor_iterator_t *itr)
{
    int i;

    /* increment each index */
    if(sptensor_iterator_valid(itr)) {
        for(i=0; i<itr->t->modes; i++) {
            itr->index[i]++;
        }
    }
    return sptensor_iterator_valid(itr);
}


static int sptensor_identity_nz_iterator_prev(sptensor_iterator_t *itr)
{
    int i;

    /* decrement each index */
    if(sptensor_iterator_valid(itr)) {
        for(i=0; i<itr->t->modes; i++) {
            itr->index[i]--;
        }
    }
    return sptensor_iterator_valid(itr);
}