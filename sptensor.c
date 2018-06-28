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
#include <sptensor.h>



/* static helper prototypes */
static void sptensor_index_sort(sptensor *tns, int left, int right);
static int  sptensor_index_partition(sptensor *tns, int left, int right);
static void sptensor_insert(sptensor *tns, const sp_index_t *idx, double val);


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
sptensor_alloc(int nmodes, const sp_index_t *dim)
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
sptensor_get(sptensor *tns, const sp_index_t *idx)
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
sptensor_set(sptensor *tns, const sp_index_t *idx, double val)
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
	sptensor_insert(tns, idx, val);
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
sptensor_find_index(sptensor *tns, const sp_index_t *idx)
{
    int left = 0;
    int right = tns->ar->size - 1;
    int mid;
    int cmp;

    /* do a binary search on the list of indexes */
    while(left <= right) {
	mid = (right+left) / 2;
	cmp = sptensor_indexcmp(tns->nmodes, idx, VPTR(tns->idx, mid));
	if(cmp == 0) {
	    return mid;
	} else if(cmp < 0) {
	    right = mid-1;
	} else {
	    left = mid+1;
	}
    }

    return -1;
}


static void
sptensor_index_sort(sptensor *tns, int left, int right)
{
    int p;
    
    /* quicksort! */
    if(left < right) {
	p = sptensor_index_partition(tns, left, right);
	sptensor_index_sort(tns, left, p);
	sptensor_index_sort(tns, p+1, right);
    }
}


static int
sptensor_index_partition(sptensor *tns, int left, int right)
{
    int i, j;
    int pivot;
    
    /* use the midpoint pivot */
    pivot = (left+right)/2;
    i = left - 1;
    j = right + 1;

    /* here we go! */
    for(;;) {
	do {
	    i++;
	} while(sptensor_indexcmp(tns->nmodes, VPTR(tns->idx,i), VPTR(tns->idx,pivot)) < 0);

	do {
	    j--;
	} while(sptensor_indexcmp(tns->nmodes, VPTR(tns->idx,j), VPTR(tns->idx,pivot)) > 0);

	if(i >= j) {
	    return j;
	}

	/* swap */
	vector_swap(tns->idx, i, j);
	vector_swap(tns->ar, i, j);
    }
}


static void
sptensor_insert(sptensor *tns, const sp_index_t *idx, double val)
{
    /* put the value and index in the list */
    vector_push_back(tns->idx, idx);
    vector_push_back(tns->ar, &val);

    /* cleanup! */
    sptensor_index_sort(tns, 0, tns->ar->size-1);
}
