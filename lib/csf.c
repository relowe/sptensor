/*
    This is the c file for the Compressed Sparse Fiber implementation.
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
#include <stdlib.h>
#include <string.h>
#include <sptensor/csf.h>
#include <sptensor/index_iterator.h>

/* nonzero iterator */
/* sptensor iterator (base type) */
struct sptensor_csf_nz_iterator
{
    sptensor_t *t;
    sptensor_index_t *index;
    sptensor_iterator_valid_f valid;
    sptensor_iterator_next_f next;
    sptensor_iterator_prev_f prev;
    sptensor_free_f free;

    int csfi;  /* The indx within the Csf list */
};


/* CSF allocation functions */
sptensor_t* sptensor_csf_alloc(sptensor_index_t *modes, int nmodes)
{
    sptensor_csf_t *result;

    /* allocate result */
    result = malloc(sizeof(sptensor_csf_t));
    if(!result) { return NULL; }
    result->dim = NULL;
    result->values = NULL;
    result->fids = NULL;
    result->fptr = NULL;

    /* allocate the modes */
    result->dim = malloc(sizeof(sptensor_index_t) * nmodes);
    if(!result->dim) {
        sptensor_csf_free(result);
        return NULL;
    }

    /* copy the dimensions */
    memcpy(result->dim, modes, nmodes * sizeof(sptensor_index_t));

    /* allocate the fptr and fids */
    

    /* populate the fields */
    result->modes = nmodes;
    result->get = (sptensor_get_f) sptensor_csf_get;
    result->set = (sptensor_set_f) sptensor_csf_set;
    result->free = (sptensor_free_f) sptensor_csf_free;
    result->iterator = (sptensor_iterator_f) sptensor_csf_iterator;
    result->nz_iterator = (sptensor_iterator_f) sptensor_csf_nz_iterator;

    return (sptensor_t*) result;
}


/* free the memory associated with any fields */
void sptensor_csf_free(sptensor_csf_t* t)
{
    /* free the non-zero pointers */
    if(t->dim) free(t->dim);
    if(t->fids){
        /* nested for loops and free each */

    } free(t->data);
    if(t->fptrs){
        /* nested for loops and free each */

    }
}


/* search the tensor for an index.  Return the element number, -1 on failure */
static int sptensor_csf_search(sptensor_csf_t *t, sptensor_index_t *idx)
{
    int i;

    /* search for the index in the structure */
    

    /* if index not found */
    return -1;
}

/* Retrieve an alement from the csf tensor */
void sptensor_csf_get(sptensor_csf_t * t, sptensor_index_t *i, mpf_t v)
{
    int index;

    /* find the index */
    index = sptensor_csf_search(t, i);

    /* return the value */
    if(index == -1) {
        mpf_set_d(v, 0.0);
    } else {
        mpf_set(v, VVAL(mpf_t, t->values, index));
    }
}


/* set the element with the csf tensor  */
void sptensor_csf_set(sptensor_csf_t * t, sptensor_index_t *i, mpf_t v)
{
    int index;
    
    /* find the index */
    index = sptensor_csf_search(t, i);

    if(index == -1) 
    {
        mpf_t nv;

        /* not there yet!  If this is zero, we are done */
        if(mpf_cmp_d(v, 0.0) == 0) {
            return;
        }

        /* ok, so we need to init and copy */
        mpf_init_set(nv, v);

        /* and then we add it to the fptr and fids */
        
    } else {
        /* if we are changing an existing element */
        if(mpf_cmp_d(v, 0.0) == 0) {
            /* remove the zero */
            
        } else {
            /* set the element */
            mpf_set(VVAL(mpf_t, t->data, index), v);
        }
    }
}


/* Iterator Functions */
sptensor_iterator_t* sptensor_csf_iterator(sptensor_csf_t *t) 
{
    return sptensor_index_iterator_alloc((sptensor_t*) t);
}



/* non-zero iterator methods */
static int sptensor_csf_nz_valid(struct sptensor_csf_nz_iterator *itr)
{
    struct sptensor_csf *t = (struct sptensor_csf*) itr->t;

    /* Return 1 if valid, 0 if not */
    
}



static void sptensor_csf_nz_load_index(struct sptensor_csf_nz_iterator* itr) 
{
    struct sptensor_csf *t = (struct sptensor_csf*) itr->t;

    if(sptensor_csf_nz_valid(itr)) {
        sptensor_index_cpy(t->modes, itr->index, VPTR(t->csf, itr->csfi));
    }
}



static void sptensor_csf_nz_free(struct sptensor_csf_nz_iterator *itr)
{
    sptensor_index_free(itr->index);
    free(itr);
}



static int sptensor_csf_nz_next(struct sptensor_csf_nz_iterator *itr)
{
    /* increment a valid iterator */
    if(sptensor_csf_nz_valid(itr)) { itr->csfi++; }

    sptensor_csf_nz_load_index(itr);

    return sptensor_csf_nz_valid(itr);
}



static int sptensor_csf_nz_prev(struct sptensor_csf_nz_iterator *itr)
{
    /* decrement a valid iterator */
    if(sptensor_csf_nz_valid(itr)) { itr->csfi--; }

    sptensor_csf_nz_load_index(itr);

    return sptensor_csf_nz_valid(itr);
}



sptensor_iterator_t* sptensor_csf_nz_iterator(sptensor_csf_t *t)
{
    struct sptensor_csf_nz_iterator *itr;

    /* try the allocation */
    itr = malloc(sizeof(struct sptensor_csf_nz_iterator));
    if(!itr) { return NULL; }

    /* allocate the index */
    itr->index = sptensor_index_alloc(t->modes);
    if(!itr->index) {
        sptensor_csf_nz_free(itr);
        return NULL;
    }

    /* populate the initial index */
    itr->ci = 0;

    /* populate the fields */
    itr->valid = (sptensor_iterator_valid_f) sptensor_csf_nz_valid;
    itr->next = (sptensor_iterator_next_f) sptensor_csf_nz_next;
    itr->prev = (sptensor_iterator_prev_f) sptensor_csf_nz_prev;
    itr->free = (sptensor_free_f) sptensor_csf_nz_free;
    itr->t = (sptensor_t *) t;
    sptensor_csf_nz_load_index(itr);

    return (sptensor_iterator_t*) itr;
}


/* 
 * Read a CSF tensor from a text file stream. The file stream is
 * expected to be in the form of:
 *   nmodes d0 d1 ... dk
 *   i0 i1 i2 ... ik v1
 *   i0 i1 i2 ... ik v2
 *   ...
 * Where nmodes is the number of modes, d0...dk are the dimensions,
 * and i0..ik are the indexes and v is the value at that index.
 * The file is read to the end, or until an unsucessful translation
 * occurs.
 *
 * Parameters: file - The file to read from
 *             
 * Return: The newly read and allocated tensor.
 */
sptensor_t * sptensor_csf_read(FILE *file)
{
    int nmodes;
    sptensor_index_t *idx;
    int i;
    mpf_t val;
    sptensor_t *tns;
    int done;

    /* a little bit of mpf allocation */
    mpf_init(val);

    /* get the dimensions */
    fscanf(file, "%u", &nmodes);
    idx = sptensor_index_alloc(nmodes);
    for(i=0; i<nmodes; i++) {
	    gmp_fscanf(file, "%u", idx + i);
    }

    /* allocate the tensor and free the dimension */
    tns = sptensor_csf_alloc(idx, nmodes);

    i=0;
    done = 0;
    while(!done) {
	    /* read the indexes */
	    for(i=0; i<nmodes; i++) {
	        if(gmp_fscanf(file, "%u", idx + i) != 1) {
		        done = 1;
		        break;
	        }
	    }

	    if(done) continue;
	    if(gmp_fscanf(file, "%Ff", val) != 1) {
	        done = 1;
	        break;
	    }

	    /* insert into the tensor */
	    sptensor_set(tns, idx, val);
    }

    /* cleanup and return! */
    free(idx);
    mpf_clear(val);

    return tns;
}


/*
 * Write a sparse tensor to a stream.  It is written in the format 
 * described in the sptensor_read function.
 * 
 * Parameter: file - The stream to write to.
 *            tns  - The tensor to write.
 */ 
void sptensor_csf_write(FILE *file, sptensor_t *tns)
{
    int i;
    mpf_t val;
    sptensor_iterator_t *itr;

    /* allocate the value */
    mpf_init(val);
    
    /* print the preamble */
    gmp_fprintf(file, "%u", tns->modes);
    for(i = 0; i < tns->modes; i++) {
	    gmp_fprintf(file, "\t%u", tns->dim[i]);
    }
    gmp_fprintf(file, "\n");

    /* print the non-zero values */
    for(itr=sptensor_nz_iterator(tns); sptensor_iterator_valid(itr); sptensor_iterator_next(itr)) {
        /* print the index */
	    for(i = 0; i < tns->modes; i++) {
	        gmp_fprintf(file, "%u\t", itr->index[i]);
	    }

        /* print the value */
        sptensor_get(tns, itr->index, val);
	    gmp_fprintf(file, "%Ff\n", val);
    }

    /* cleanup */
    sptensor_iterator_free(itr);
    mpf_clear(val);
}