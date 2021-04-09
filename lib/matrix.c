/*
    This is the implementation file for dense matrix (2d array) storage for sptensor.
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
#include <sptensor/matrix.h>
#include <sptensor/index_iterator.h>


/* Dense matrix allocation functions */
sptensor_t* sptensor_matrix_alloc(sptensor_index_t *modes)
{
    sptensor_matrix_t *t;
    int i;
    int mn;

    /* allocate the tensor */
    t = malloc(sizeof(sptensor_matrix_t));
    t->dim = NULL;
    t->m = NULL;
    if(!t) return NULL;

    /* allocate the dimension */
    t->dim = sptensor_index_alloc(2);
    if(!t->dim) {
        sptensor_matrix_free(t);
        return NULL;
    }

    /* allocate the array */
    mn = modes[0] * modes[1];
    t->m = malloc(sizeof(mpf_t*) * modes[0]);
    if(t->m) t->m[0] = malloc(sizeof(mpf_t) * mn);
    if(!t->m || !t->m[0]) {
        sptensor_matrix_free(t);
        return NULL;
    }

    /* initialize mpf fields */
    for(i=0; i<mn; i++) {
        mpf_init(t->m[0][i]);
    }

    /* set up the row links */
    for(i=1; i<modes[0]; i++) {
        t->m[i] = t->m[i-1] + modes[1];
    }

    /* populate the fields */
    t->modes = 2;
    sptensor_index_cpy(t->modes, t->dim, modes);
    t->get = (sptensor_get_f) sptensor_matrix_get;
    t->set = (sptensor_get_f) sptensor_matrix_set;
    t->iterator = (sptensor_iterator_f) sptensor_matrix_iterator;
    t->nz_iterator = (sptensor_iterator_f) sptensor_matrix_nz_iterator;
    t->free = (sptensor_free_f) sptensor_matrix_free;

    return (sptensor_t *) t;
}



void sptensor_matrix_free(sptensor_matrix_t* t)
{
    int i;
    int mn;

    if(t->m && t->m[0]) { 
        mn = t->dim[0] * t->dim[1];
        for(i=0; i<mn; i++) {
            mpf_clear(t->m[0][i]);
        }
        free(t->m[0]);
    }
    if(t->m) { free(t->m); }
    if(t->dim) { sptensor_index_free(t->dim); }
    free(t);
}

/* matrix element access functions */
void sptensor_matrix_get(sptensor_matrix_t * t, sptensor_index_t *i, mpf_t v)
{
    mpf_set(v, t->m[i[0]][i[1]]);
}


void sptensor_matrix_set(sptensor_matrix_t * t, sptensor_index_t *i, mpf_t v)
{
    mpf_set(t->m[i[0]][i[1]], v);
}


/* Iterator Functions */
sptensor_iterator_t* sptensor_matrix_iterator(sptensor_matrix_t *t)
{
    return sptensor_index_iterator_alloc((sptensor_t*) t);
}


sptensor_iterator_t* sptensor_matrix_nz_iterator(sptensor_matrix_t *t)
{

    return sptensor_index_iterator_alloc((sptensor_t*) t);
}