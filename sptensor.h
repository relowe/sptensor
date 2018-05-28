/*
    This is a collection of functions for dealing with sparse tensors.
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
#ifndef SPTENSOR_H
#define SPTENSOR_H
#include <vector.h>

#define SPTENSOR_DEFAULT_CAPACITY 128


typedef unsigned int sp_index_t;

typedef struct sptensor
{
    vector *ar;          /* the tensor values */
    vector *idx;         /* the index list (maintained in sorted order) */
    sp_index_t *dim;     /* The dimension of each tensor mode */
    unsigned int nmodes; /* The number of tensor modes */
} sptensor;


/*
 * Allocate a sparse tensor.  Sparse tensors are allocated using
 * malloc, and have a SPTENSOR_DEFAULT_CAPACITY capacity.
 * 
 * Parameters: nmodes - The number of modes
 *             dim    - The dimension of the tensor
 *
 * Return: A pointer to the newly created tensor.
 */ 
sptensor* sptensor_alloc(int nmodes, const sp_index_t *dim);


/*
 * Free the memory allocated for a sparse tensor.
 */
void sptensor_free(sptensor *tns);


/*
 * Get a value from a sparse tensor.
 *
 * Parameters: tns - The sparse tensor to retrieve from
 *             idx - The index of the item to retrieve
 */
double sptensor_get(sptensor *tns, const sp_index_t *idx);


/* 
 * Set a value in the sparse tensor.
 * 
 * Parameters: tns - The sparse tensor to write to
 *             idx - The index of the item to retrieve
 *             val - The value to write to the tensor
 */
void sptensor_set(sptensor *tns, const sp_index_t *idx, double val);


/* 
 * Compare two indexes for a given tensor.  Comparison is 
 * performed from left to right.  Pretty much exactly as 
 * strcmp, except with ints.
 * 
 * Parameter: nmodes - The number of modes
 *            a   - The left hand index being compared
 *            b   - The right hand index being compared
 *  
 * Return:  < 0 if a < b
 *            0 if a == b
 *          > 0 if a > b
 */
int sptensor_indexcmp(unsigned int nmodes, const sp_index_t *a, const sp_index_t *b);



/*
 * Find the index into ar of the tensor struct.
 * 
 * Parameter: tns - The sparse tensor to search
 *            idx - The index to find
 *
 * Return: The index into ar of element idx.  If the index is
 *         not of a non-zero value, return -1.
 */
int sptensor_find_index(sptensor *tns, const sp_index_t *idx);

#endif
