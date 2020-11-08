/*
    This is the header file for the COOrdinate storage for sptensor.
    The sptensor library provides a set of functions and data type for
    dealing with sparse tensors.

    Copyright (C) 2018  Robert Lowe <pngwen@acm.org>

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
#include <string.h>
#include <sptensor/coo.h>
#include <sptensor/index_iterator.h>

/* nonzero iterator */
/* sptensor iterator (base type) */
struct sptensor_coo_nz_iterator
{
    sptensor_t *t;
    sptensor_index_t *index;
    sptensor_iterator_valid_f valid;
    sptensor_iterator_next_f next;
    sptensor_iterator_prev_f prev;
    sptensor_free_f free;

    int ci;  /* The indx within the COO list */
};


/* COO allocation functions */
sptensor_t* sptensor_coo_alloc(sptensor_index_t *modes, int nmodes)
{
    sptensor_coo_t *result;

    /* allocate result */
    result = malloc(sizeof(sptensor_coo_t));
    if(!result) { return NULL; }
    result->dim = NULL;
    result->data = NULL;
    result->coo = NULL;

    /* allocate the modes */
    result->dim = malloc(sizeof(sptensor_index_t) * nmodes);
    if(!result->dim) {
        sptensor_coo_free(result);
        return NULL;
    }

    /* copy the dimensions */
    memcpy(result->dim, modes, nmodes * sizeof(sptensor_index_t));

    /* allocate the vectors */
    result->coo = sptensor_vector_alloc(sizeof(sptensor_index_t)*nmodes, 
                                        SPTENSOR_VECTOR_DEFAULT_CAPACITY);
    result->data = sptensor_vector_alloc(sizeof(mpf_t), 
                                         SPTENSOR_VECTOR_DEFAULT_CAPACITY);
    if(!result->coo || !result->data) {
        sptensor_coo_free(result);
        return NULL;
    }

    /* populate the fields */
    result->modes = nmodes;
    result->get = (sptensor_get_f) sptensor_coo_get;
    result->set = (sptensor_set_f) sptensor_coo_set;
    result->free = (sptensor_free_f) sptensor_coo_free;
    result->iterator = (sptensor_iterator_f) sptensor_coo_iterator;
    result->nz_iterator = (sptensor_iterator_f) sptensor_coo_nz_iterator;

    return (sptensor_t*) result;
}


/* free the memory associated with the */
void sptensor_coo_free(sptensor_coo_t* t)
{
    /* free the non-zero poitners */
    if(t->dim) free(t->dim);
    if(t->data) sptensor_vector_free(t->data);
    if(t->coo) sptensor_vector_free(t->coo);
}


/* search the tensor for an index.  Return the element number, -1 on failure */
static int sptensor_coo_search(sptensor_coo_t *t, sptensor_index_t *idx)
{
    int i;

    /* search for the index in the vector */
    for(i=0; i<t->coo->size; i++) {
        if(sptensor_index_cmp(t->modes, idx, (sptensor_index_t*) VPTR(t->coo, i)) == 0) {
            return i;
        }
    }

    return -1;
}

/* Retrieve an alement from the coo tensor */
void sptensor_coo_get(sptensor_coo_t * t, sptensor_index_t *i, mpf_t v)
{
    int index;

    /* find the index */
    index = sptensor_coo_search(t, i);

    /* return the value */
    if(index == -1) {
        mpf_set_d(v, 0.0);
    } else {
        mpf_set(v, VVAL(mpf_t, t->data, index));
    }
}


/* set the element with the coo tensor  */
void sptensor_coo_set(sptensor_coo_t * t, sptensor_index_t *i, mpf_t v)
{
    int index;
    
    /* find the index */
    index = sptensor_coo_search(t, i);

    if(index == -1) 
    {
        mpf_t nv;

        /* not there yet!  If this is zero, we are done */
        if(mpf_cmp_d(v, 0.0) == 0) {
            return;
        }

        /* ok, so we need to init and copy */
        mpf_init_set(nv, v);

        /* and then we add it to the vector */
        sptensor_vector_push_back(t->coo, i);
        sptensor_vector_push_back(t->data, nv);
    } else {
        /* if we are changing an existing element */
        if(mpf_cmp_d(v, 0.0) == 0) {
            /* remove the zero */
            sptensor_vector_remove(t->coo, index);
            sptensor_vector_remove(t->data, index);
        } else {
            /* set the element */
            mpf_set(VVAL(mpf_t, t->data, index), v);
        }
    }
}


/* Iterator Functions */
sptensor_iterator_t* sptensor_coo_iterator(sptensor_coo_t *t) 
{
    return sptensor_index_iterator_alloc((sptensor_t*) t);
}



/* non-zero iterator methods */
static int sptensor_coo_nz_valid(struct sptensor_coo_nz_iterator *itr)
{
    struct sptensor_coo *t = (struct sptensor_coo*) itr->t;

    return itr->ci >= 0 && itr->ci < t->coo->size;
}



static void sptensor_coo_nz_load_index(struct sptensor_coo_nz_iterator* itr) 
{
    struct sptensor_coo *t = (struct sptensor_coo*) itr->t;

    if(sptensor_coo_nz_valid(itr)) {
        sptensor_index_cpy(t->modes, itr->index, VPTR(t->coo, itr->ci));
    }
}



static void sptensor_coo_nz_free(struct sptensor_coo_nz_iterator *itr)
{
    sptensor_index_free(itr->index);
    free(itr);
}



static int sptensor_coo_nz_next(struct sptensor_coo_nz_iterator *itr)
{
    /* increment a valid iterator */
    if(sptensor_coo_nz_valid(itr)) { itr->ci++; }

    sptensor_coo_nz_load_index(itr);

    return sptensor_coo_nz_valid(itr);
}



static int sptensor_coo_nz_prev(struct sptensor_coo_nz_iterator *itr)
{
    /* decrement a valid iterator */
    if(sptensor_coo_nz_valid(itr)) { itr->ci--; }

    sptensor_coo_nz_load_index(itr);

    return sptensor_coo_nz_valid(itr);
}



sptensor_iterator_t* sptensor_coo_nz_iterator(sptensor_coo_t *t)
{
    struct sptensor_coo_nz_iterator *itr;

    /* try the allocation */
    itr = malloc(sizeof(struct sptensor_coo_nz_iterator));
    if(!itr) { return NULL; }

    /* allocate the index */
    itr->index = sptensor_index_alloc(t->modes);
    if(!itr->index) {
        sptensor_coo_nz_free(itr);
        return NULL;
    }

    /* populate the initial index */
    itr->ci = 0;

    /* populate the fields */
    itr->valid = (sptensor_iterator_valid_f) sptensor_coo_nz_valid;
    itr->next = (sptensor_iterator_next_f) sptensor_coo_nz_next;
    itr->prev = (sptensor_iterator_prev_f) sptensor_coo_nz_prev;
    itr->free = (sptensor_free_f) sptensor_coo_nz_free;
    itr->t = (sptensor_t *) t;
    sptensor_coo_nz_load_index(itr);

    return (sptensor_iterator_t*) itr;
}

