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
#include <sptensor/coo.h>
#include <sptensor/vector.h>
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
    int i;

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

    /* allocate the fids */
    result-> fids = sptensor_vector_alloc(sizeof(sptensor_vector*), SPTENSOR_VECTOR_DEFAULT_CAPACITY);

    /* allocate the fptr */
    result -> fptr = sptensor_vector_alloc(sizeof(sptensor_vector*), SPTENSOR_VECTOR_DEFAULT_CAPACITY);
    
    /* allocate values vector */
    result -> values = sptensor_vector_alloc(sizeof(mpf_t), SPTENSOR_VECTOR_DEFAULT_CAPACITY);

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
    int i;
    /* free the non-zero pointers */
    if(t->dim) free(t->dim);
    if(t->fids){
        /* free each vector in the vector */
        for(i = 0; i < t->fids->size; i++){
            sptensor_vector_free(VVAL(sptensor_vector*, t->fids, i));
        }
        /* then free the vector */
        sptensor_vector_free(t->fids);
    }
    if(t->fptr){
        /* free each vector in the vector */
        for(i = 0; i < t->fptr->size; i++){
            sptensor_vector_free(VVAL(sptensor_vector*, t->fptr, i));
        }
        /* then free the vector */
        sptensor_vector_free(t->fptr);
    }
    if(t->values){
        sptensor_vector_free(t->values);
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

}


/* set the element with the csf tensor  */
void sptensor_csf_set(sptensor_csf_t * t, sptensor_index_t *i, mpf_t v)
{
   
}

/**
 * @brief Generate a csf format tensor from a coo format tensor
 * 
 * @param coo The sptensor_coo_t format tensor
 * @return sptensor_t* The sptensor_csf_t format tensor
 */
sptensor_csf_t* sptensor_csf_from_coo(sptensor_coo_t* coo){
    int i, j, k = 0;

    /* These vectors will be pushed to the fids in reversed order */
    sptensor_vector** fidsVectors = malloc((coo->modes) * sizeof(sptensor_vector*));

    /* These vectors will be pushed to the fptrs in reversed order */
    sptensor_vector** fptrVectors = malloc(((coo->modes)-1) * sizeof(sptensor_vector*));

    sptensor_iterator_t* itr = sptensor_nz_iterator((sptensor_t*)coo);
    sptensor_csf_t* result = (sptensor_csf_t*) sptensor_csf_alloc(coo->dim, coo->modes);

    mpf_t temp;
    mpf_init(temp);

    /* The last index of fids (last dimension indices) */
    fidsVectors[coo->modes-1]  = sptensor_vector_alloc(sizeof(int), SPTENSOR_VECTOR_DEFAULT_CAPACITY);

    while(sptensor_iterator_valid(itr)){ 
        /* Get the value and add it to the value array */
        sptensor_get(itr->t, itr->index, temp);
        gmp_printf("%Ff\n", temp);
        sptensor_vector_push_back(result->values, temp);

        /* Insert the last dimension's indices in fids[last] */
        sptensor_vector_push_back(fidsVectors[(coo->modes)-1], &(itr->index[coo->modes-1]));
        /*printf("pushing back address: %d and value: %d in fidsVectors[%d]\n", &(itr->index[coo->modes-1]), *(&(itr->index[coo->modes-1])), coo->modes-1);*/

        sptensor_iterator_next(itr);
    }
    mpf_clear(temp);
    sptensor_iterator_free(itr);


    itr = sptensor_nz_iterator((sptensor_t*)coo);

    /* See how to slice each dimension and insert into fids and fptrs*/
    for(i = itr->t->modes-1; i > 0; i--){
        sptensor_iterator_t* itr = sptensor_nz_iterator((sptensor_t*)coo);
        fidsVectors[i-1] = sptensor_vector_alloc(sizeof(int), SPTENSOR_VECTOR_DEFAULT_CAPACITY);
        fptrVectors[i-1] = sptensor_vector_alloc(sizeof(int), SPTENSOR_VECTOR_DEFAULT_CAPACITY);
        
        /* This will store the every index other than the current dimension */
        sptensor_index_t* previous_lower_indices = NULL;
        sptensor_index_t* previous_even_lower_indices = NULL;
                
        k = 0;
        while(sptensor_iterator_valid(itr)){ 
            /* If just started, load the first valid index in the placeholder indices */
            if(previous_lower_indices == NULL){
                previous_lower_indices = malloc(sizeof(sptensor_index_t) * (i));
                previous_even_lower_indices = malloc(sizeof(sptensor_index_t) *(i+1));

                /* Copy the indices now */
                sptensor_index_cpy((i-1), previous_lower_indices, itr->index);

                /* Copy even lower indices */
                sptensor_index_cpy((i-1), previous_even_lower_indices, itr->index);
            }

            /* If index changed, add values to fids vector and fptr vector */
            if(sptensor_index_cmp(i, itr->index, previous_lower_indices) != 0){
                
                /*push back i-1th index nto fptr*/
                sptensor_vector_push_back(fidsVectors[i-1], &(itr->index[i-1]));

                /* there's only d - 1 fprs */
                sptensor_vector_push_back(fptrVectors[i-1], &(k));

                /* Then update the previous_lower_indices */
                sptensor_index_cpy(i, previous_lower_indices, itr->index);
               
            }

            /* if index changed, increment k which will be added to fptr [i-1] */
            if(sptensor_index_cmp(i+1, itr->index, previous_even_lower_indices) != 0){
                sptensor_index_cpy((i+1), previous_even_lower_indices, itr->index);
                k++;
            }

            sptensor_iterator_next(itr);
        }
        /* Insert imaginary next dimension to fptr[i-1] to finish the slice */
        sptensor_vector_push_back(fptrVectors[i-1], &(k));
        sptensor_iterator_free(itr);
        sptensor_index_free(previous_lower_indices);
        sptensor_index_free(previous_even_lower_indices);
    }

    /* push_back fids */
    for(i = 0; i < coo->modes; i++){
        sptensor_vector_push_back(result->fids, &fidsVectors[i]);
    }

    /* push_back fptrs */
    for(i = 0; i < (coo->modes)-1; i++){
        sptensor_vector_push_back(result->fptr, &fptrVectors[i]);
    }

    free(fptrVectors);
    free(fidsVectors);
    return result;
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
        // sptensor_index_cpy(t->modes, itr->index, VPTR(t->csf, itr->ci));
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
    itr->csfi = 0;

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
/**
 * @brief Print the fptrs, fids and the values of the csf object
 * 
 * @param tns The tensor to print
 */
void sptensor_csf_print(sptensor_t* tns){ 
    int i;
    int j;

    /* Typcaset generic to csf_t */
    sptensor_csf_t* c = (sptensor_csf_t*) tns;

    printf("\n\nfids:\n");
    for(i = 0; i < c->fids->size; i++){
        printf("fids[%d]:", i);
        for(j = 0; j <  (VVAL(sptensor_vector*, c->fids, i))->size; j++){
            printf("%d ", VVAL(int, VVAL(sptensor_vector*, c->fids, i), j));
        }
        printf("\n");
    }

    /*print fptrs */
    printf("\n\nfptrs:\n");
    for(i = 0; i < c->fptr->size; i++){
        printf("fptr[%d]:", i);
        for(j = 0; j < (VVAL(sptensor_vector*, c->fptr, i))->size; j++){
            printf("%d ", VVAL(int, VVAL(sptensor_vector*, c->fptr, i), j));
        }
        printf("\n");
    }

    /*print values */
    printf("\n\nvalues:\n");
    for(i = 0; i < c->values->size; i++){
        gmp_printf("%Ff\n", VVAL(mpf_t, c->values, i));
    }
}
