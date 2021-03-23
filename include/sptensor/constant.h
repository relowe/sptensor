/*
    This is the header file for the constant tensors in sptensor.
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
#include<sptensor/sptensor.h>
#ifndef SPTENSOR_CONSTANT_H
#define SPTENSOR_CONSTANT_H

/* Constant tensor  (all elements are set to the same value) */
typedef struct sptensor_constant
{
    /* sptensor fields */
    int modes;
    sptensor_index_t *dim;
    sptensor_get_f get;
    sptensor_set_f set;
    sptensor_iterator_f iterator;
    sptensor_iterator_f nz_iterator;
    sptensor_free_f free;

    /* the value of all the elements */
    mpf_t value;
} sptensor_constant_t;


/* constant allocation functions */
sptensor_t *sptensor_constant_alloc(sptensor_index_t * modes, int nmodes, mpf_t value);
void sptensor_constant_free(sptensor_constant_t *t);

/* constant element access functions (read only) */
void sptensor_constant_get(sptensor_constant_t *t, sptensor_index_t *i, mpf_t v);

#endif