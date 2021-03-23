/*
    This is the implementation file for the constant tensors in sptensor.
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
#include <stdlib.h>
#include <sptensor/sptensor.h>
#include <sptensor/constant.h>
#include <sptensor/index_iterator.h>

/* constant allocation functions */
sptensor_t *sptensor_constant_alloc(sptensor_index_t *modes, int nmodes, mpf_t value)
{
    sptensor_constant_t *result;

    /* allocate the result */
    result = malloc(sizeof(sptensor_constant_t));
    if(!result) { return NULL; }
    result->dim = NULL;

    /* initialize the modes */
    result->dim = sptensor_index_alloc(nmodes);
    if(!result->dim) {
        sptensor_constant_free(result);
        return NULL;
    }

    /* copy the dimensions */
    sptensor_index_cpy(nmodes, result->dim, modes);

    /* populate the fields */
    result->modes = nmodes; 
    result->get = (sptensor_get_f) sptensor_constant_get;
    result->set = NULL;
    result->iterator = sptensor_index_iterator_alloc;
    result->nz_iterator = sptensor_index_iterator_alloc;
    result->free = (sptensor_free_f) sptensor_constant_free;
    mpf_init_set(result->value, value);

    return result;
}


void sptensor_constant_free(sptensor_constant_t *t) 
{
    /* deallocate the index and the mpf value*/
    if(t->dim) {
        sptensor_index_free(t->dim);
        mpf_clear(t->value);
    }

    /* deallocate the outer struct */
    free(t);
}


/* constant element access functions (read only) */
void sptensor_constant_get(sptensor_constant_t *t, sptensor_index_t *i, mpf_t v)
{
    /* simplicity itself */
    mpf_set(v, t->value);
}