/*
    This is an implementation of the INZT function described in the 
    paper "Space-Efficient 𝑘-d Tree-Based Storage Format for Sparse
    Tensors" by Ivan Simecek et al.  https://doi.org/10.1145/3369583.3392692

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
#ifndef SPTENSOR_INZT_H
#define SPTENSOR_INZT_H

#include <sptensor/sptensor.h>
#include <sptensor/index.h>
#include <sptensor/vector.h>

/*
 * Store the index along with the morton coding.  This is used to 
 * search for index values and ranges of values.
 */
typedef struct sptensor_inzt_index
{
    sptensor_index_t *index;    /* the actual index value */
    mpz_t morton;               /* the morton code of the index */
} sptensor_inzt_index_t;



/*
 * This is the primary structure. The sptensor_inzt structure is the search
 * structure which is created from a tensor.  It is used for both range
 * and nz index lookup.
 */
typedef struct sptensor_inzt
{
    int nnz;                        /* The number of non-zero indexes */
    int nmodes;                     /* The number of modes in the indexes */
    sptensor_vector *inzt_index;    /* Vector of inzt indexes */
} sptensor_inzt_t;


/* 
 * Create an sptensor_inzt structure from the non-zero iterator of a 
 * tensor.  All non-zero indexes are inserted into the list sorted by 
 * their morton codes.
 */
sptensor_inzt_t * sptensor_inzt_create(sptensor_iterator_t *nzitr);


/* 
 * Deallocates an sptensor_inzt structure.
 */
void sptensor_inzt_free(sptensor_inzt_t *inzt);


/*
 * Populate the mpz_t morton field with the morton encoding of the 
 * index.
 */
void sptensor_inzt_morton(int nmodes, sptensor_index_t *index, mpz_t morton);


/* 
 * Perform the inzt lookup.  This function returns 1 if there is a non-zero
 * element in the given index range. If the range contains only zeroes, it
 * return 0.
 */
int sptensor_inzt(sptensor_inzt_t *inzt, sptensor_index_t *min, sptensor_index_t *max);
#endif
