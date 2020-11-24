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
#include <stdlib.h>
#include <sptensor/sptensor.h>
#include <sptensor/index.h>
#include <sptensor/vector.h>
#include <sptensor/inzt.h>

/* Static Prototypes */
static void inzt_sort(sptensor_vector *ar, int low, int high);
static int inzt_partition(sptensor_vector *ar, int low, int high);
static int inzt_binsearch(sptensor_inzt_t *inzt, sptensor_index_t *index);


/* 
 * Create an sptensor_inzt structure from the non-zero iterator of a 
 * tensor.  All non-zero indexes are inserted into the list sorted by 
 * their morton codes.
 */
sptensor_inzt_t * sptensor_inzt_create(sptensor_iterator_t *nzitr)
{
    sptensor_inzt_t *result;
    sptensor_inzt_index_t index;

    /* allocate the inzt structure itself */
    result = malloc(sizeof(sptensor_inzt_t));
    if(!result) {
        return NULL;
    }
    result->nmodes = nzitr->t->modes;
    result->nnz = 0;

    /* allocate the index vector */
    result->inzt_index = sptensor_vector_alloc(sizeof(sptensor_inzt_index_t), 128);
    if(!result->inzt_index) {
        sptensor_inzt_free(result);
        return NULL;
    }

    /* Add all the non-zeroes to the search object. */
    while(sptensor_iterator_valid(nzitr)) {
        result->nnz++;

        /* copy the index */
        index.index = sptensor_index_alloc(result->nmodes);
        sptensor_index_cpy(result->nmodes, index.index, nzitr->index);

        /* compute morton code */
        mpz_init(index.morton);
        sptensor_inzt_morton(result->nmodes, index.index, index.morton);

        /* add it to the index structure */
        sptensor_vector_push_back(result->inzt_index, &index);
        
        /* next! */
        sptensor_iterator_next(nzitr);
    }

    /* put the array in sorted order (by morton code) */
    inzt_sort(result->inzt_index, 0, result->nnz - 1);

    return result;
}


/* 
 * Deallocates an sptensor_inzt structure.
 */
void sptensor_inzt_free(sptensor_inzt_t *inzt)
{
    int i;
    sptensor_inzt_index_t *index;

    /* clear the indexes (if they exist) */
    if(inzt->inzt_index) {
        for(i=0; i<inzt->nnz; i++) {
            index = VPTR(inzt->inzt_index, i);
            mpz_clear(index->morton);
            sptensor_index_free(index->index);
        }
        sptensor_vector_free(inzt->inzt_index);
    }

    /* deallocate the container */
    free(inzt);
}


/*
 * Populate the mpz_t morton field with the morton encoding of the 
 * index.
 */
void sptensor_inzt_morton(int nmodes, sptensor_index_t *index, mpz_t morton)
{
    unsigned int bit=0;
    sptensor_index_t mask=1;
    unsigned int i;

    /* start with an empty morton code */
    mpz_set_ui(morton, 0);

    while(mask) {
        /* run through the modes, interleave the masked bit */
        for(i=0; i<nmodes; i++, bit++) {
            /* set the bit if this bit is set in the mode index */
            if(index[i] & mask) {
                mpz_setbit(morton, bit);
            }
        }

        /* update the mask */
        mask <<= 1;
    }
}


/* 
 * Perform the inzt lookup.  This function returns 1 if there is a non-zero
 * element in the given index range. If the range contains only zeroes, it
 * return 0.
 */
int sptensor_inzt(sptensor_inzt_t *inzt, sptensor_index_t *min, sptensor_index_t *max)
{
    int min_index, max_index;

    /* look up the min and max indexes */
    min_index = inzt_binsearch(inzt, min);
    max_index = inzt_binsearch(inzt, max);

    /* if either was found, it is non-zero */
    if(min_index >= 0 || max_index >= 0) {
        return 1;
    }

    /* If there is room for another index, there must be non-zero
     * within the range. */
    return min_index != max_index;
}



/* Static Functions */
static void inzt_sort(sptensor_vector *ar, int low, int high)
{
    int p;

    if(low < high) {
        p = inzt_partition(ar, low, high);
        inzt_sort(ar, low, p);
        inzt_sort(ar, p+1, high);
    }
}


static int inzt_partition(sptensor_vector *ar, int low, int high)
{
    /* set everything up, using Hoare's partitioning scheme */
    int p = (low + high) / 2;
    int i = low - 1;
    int j = high + 1;
    sptensor_inzt_index_t *aridx = ar->ar;

    for(;;) {
        do {
            i = i+1;
        } while(mpz_cmp(aridx[i].morton, aridx[p].morton) < 0);
        do {
            j = j-1;
        } while(mpz_cmp(aridx[j].morton, aridx[p].morton) > 0);
        if(i >= j) {
            return j;
        }
        sptensor_vector_swap(ar, i, j);
    }
}


static int inzt_binsearch(sptensor_inzt_t *inzt, sptensor_index_t *index)
{
    sptensor_inzt_index_t *ar = inzt->inzt_index->ar;
    int left = 0;
    int right = inzt->inzt_index->size - 1;
    int mid;
    int cmp;
    mpz_t item;

    /* get the morton encoding of the search item */
    mpz_init(item);
    sptensor_inzt_morton(inzt->nmodes, index, item);

    /* do a binary search on the list of indexes */
    while(left <= right) {
	    mid = (right+left) / 2;
        cmp = mpz_cmp(item, ar[mid].morton);
	    if(cmp == 0) {
            mpz_clear(item);
	        return mid;
	    } else if(cmp < 0) {
	        right = mid-1;
	    } else {
	        left = mid+1;
	    }
    }

    /* return the new position */
    mpz_clear(item);
    return -left-1;
}
