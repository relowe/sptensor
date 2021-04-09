/*
    This is the header file for dense matrix (2d array) storage for sptensor.
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
#include <stdio.h>
#include <sptensor/sptensor.h>
#include <sptensor/vector.h>

#ifndef SPTENSOR_MATRIX_H
#define SPTENSOR_MATRIX_H

/* Dense Matrix storage type */
typedef struct sptensor_matrix
{
    /* sptensor fields */
    int modes;
    sptensor_index_t *dim;
    sptensor_get_f get;
    sptensor_set_f set;
    sptensor_iterator_f iterator;
    sptensor_iterator_f nz_iterator;
    sptensor_free_f free;

    /* dense specific fields */
    mpf_t **m;
} sptensor_matrix_t;

/* Dense matrix allocation functions */
sptensor_t* sptensor_matrix_alloc(sptensor_index_t *modes);
void sptensor_matrix_free(sptensor_matrix_t* t);

/* matrix element access functions */
void sptensor_matrix_get(sptensor_matrix_t * t, sptensor_index_t *i, mpf_t v);
void sptensor_matrix_set(sptensor_matrix_t * t, sptensor_index_t *i, mpf_t v);

/* Iterator Functions */
sptensor_iterator_t* sptensor_matrix_iterator(sptensor_matrix_t *t);
sptensor_iterator_t* sptensor_matrix_nz_iterator(sptensor_matrix_t *t);

#endif
