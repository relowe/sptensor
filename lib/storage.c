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

#include <stdlib.h>
#include <string.h>
#include <sptensor/storage.h>
#include <sptensor/binsearch.h>


/* static helper prototypes */
static void sptensor_insert(sptensor *tns, int i, sp_index_t *idx, double val);


/*
 * Allocate a sparse tensor.  Sparse tensors are allocated using
 * malloc, and have a default capacity of 1024 non-zero items.
 * 
 * Parameters: nmodes - The number of modes
 *             dim    - The dimension of the tensor
 *
 * Return: A pointer to the newly created tensor.
 */ 
sptensor*
sptensor_alloc(int nmodes, sp_index_t *dim)
{
    sptensor *tns;

    /* allocate the tensor struct and initialize fields */
    tns = (sptensor*) malloc(sizeof(sptensor));
    tns->nmodes = nmodes;

    /* allocate and populate the tensor dimension */
    tns->dim = (int*) malloc(sizeof(sp_index_t) * tns->nmodes);
    memcpy(tns->dim, dim, sizeof(sp_index_t) * tns->nmodes);

    /* allocate space for the nonzeroes and indexes */
    tns->ar = vector_alloc(sizeof(double),
			   SPTENSOR_DEFAULT_CAPACITY);
    tns->idx = vector_alloc(sizeof(sp_index_t)*tns->nmodes,
			    SPTENSOR_DEFAULT_CAPACITY);

    return tns;
}


/*
 * Free the memory allocated for a sparse tensor.
 */
void
sptensor_free(sptensor *tns)
{
    vector_free(tns->ar);
    vector_free(tns->idx);
    free(tns->dim);
    free(tns);
}


/*
 * Get a value from a sparse tensor.
 *
 * Parameters: tns - The sparse tensor to retrieve from
 *             idx - The index of the item to retrieve
 */
double
sptensor_get(sptensor *tns, sp_index_t *idx)
{
    int i;

    /* get the index to the item */
    i = sptensor_find_index(tns, idx);

    /* return the appropriate value */
    if(i < 0) {
	return 0;
    }
    return VVAL(double, tns->ar, i);
}


/* 
 * Set a value in the sparse tensor.
 * 
 * Parameters: tns - The sparse tensor to write to
 *             idx - The index of the item to retrieve
 *             val - The value to write to the tensor
 */
void
sptensor_set(sptensor *tns, sp_index_t *idx, double val)
{
    int i;

    /* get the index to the item */
    i = sptensor_find_index(tns, idx);

    /* if it is zero, we either ignore it or remove it! */
    if(val == 0.0) {
	if(i<0) return;  /* nothing to do! */

	/* we need to remove an item */
	vector_remove(tns->ar, i);
	vector_remove(tns->idx, i);

	return;
    }

    /* set or insert as needed */
    if(i < 0) {
	sptensor_insert(tns, -(i+1), idx, val);
    } else {
	VVAL(double, tns->ar, i) = val;
    }
}
    


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
int
sptensor_indexcmp(unsigned int nmodes, const sp_index_t *a, const sp_index_t *b)
{
    int i;

    /* look for < or > comparisons */
    for(int i=0; i < nmodes; i++) {
	if(a[i] < b[i]) {
	    return -1;
	}

	if(a[i] > b[i]) {
	    return 1;
	}
    }

    return 0; /* must be equal! */
}


static int spindex_bincmp(int element_size, void *a, void *b)
{
    return sptensor_indexcmp(element_size / sizeof(sp_index_t), a, b);
}


/*
 * Find the index into ar of the tensor struct.
 * 
 * Parameter: tns - The sparse tensor to search
 *            idx - The index to find
 *
 * Return: The index into ar of element idx.  If the index is
 *         not of a non-zero value, return -1.
 */
int
sptensor_find_index(sptensor *tns, sp_index_t *idx)
{
    return vector_binsearch(tns->idx, idx, spindex_bincmp);
}



static void
sptensor_insert(sptensor *tns, int i, sp_index_t *idx, double val)
{
    /* put the value and index in the list */
    vector_insert(tns->idx, i, idx);
    vector_insert(tns->ar, i, &val);
}
