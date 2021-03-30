/*
    This is the header file for the Compressed Sparse Fiber implementation.
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
#include <stdio.h>
#include <sptensor/sptensor.h>
#include <sptensor/coo.h>
#include <sptensor/vector.h>

#ifndef SPTENSOR_CSF_H
#define SPTENSOR_CSF_H

/* COO storage type */
typedef struct sptensor_csf
{
    /* sptensor fields */
    int modes;
    sptensor_index_t *dim;
    sptensor_get_f get;
    sptensor_set_f set;
    sptensor_iterator_f iterator;
    sptensor_iterator_f nz_iterator;
    sptensor_free_f free;

    /* csf specific fields */
    sptensor_vector* values;
    sptensor_vector* fids;
    sptensor_vector* fptr;
} sptensor_csf_t;

/* CSF allocation functions */
sptensor_t* sptensor_csf_alloc(sptensor_index_t *modes, int nmodes);
void sptensor_csf_free(sptensor_csf_t* t);

/* CSF element access functions */
void sptensor_csf_get(sptensor_csf_t * t, sptensor_index_t *i, mpf_t v);
void sptensor_csf_set(sptensor_csf_t * t, sptensor_index_t *i, mpf_t v);

/**
 * @brief Generate a csf format tensor from a coo format tensor
 * 
 * @param coo The sptensor_coo_t format tensor
 * @return sptensor_t* The sptensor_csf_t format tensor
 */
sptensor_csf_t* sptensor_csf_from_coo(sptensor_coo_t* coo){
    
}

/* Iterator Functions */
sptensor_iterator_t* sptensor_csf_iterator(sptensor_csf_t *t);
sptensor_iterator_t* sptensor_csf_nz_iterator(sptensor_csf_t *t);

/* I/O Functions */
sptensor_t * sptensor_csf_read(FILE *file);
void sptensor_csf_write(FILE *file, sptensor_t *tns);

#endif
