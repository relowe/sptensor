/*
    This is a collection of functions for performing basic tensor transformations.
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
#include <sptensor/transform.h>

/* Allocate a tensor shape */
sptensor_shape_t* sptensor_shape_alloc(int modes)
{
    sptensor_shape_t *result;
    
    /* attempt to allocate the structure */
    result = malloc(sizeof(sptensor_shape_t));
    if(!result) {
        return NULL;
    }

    /* attempt to allocate the storage for the dimensions */
    result->dim = sptensor_index_alloc(modes);
    if(!result->dim) {
        sptensor_shape_free(result);
        return NULL;
    }

    /* populate the modes */
    result->modes = modes;

    return result; 
}


/* Free the tensor shape */
void sptensor_shape_free(sptensor_shape_t *s)
{
    /* free the dimensions if they exist */
    if(s->dim) {
        sptensor_index_free(s->dim);
    }

    /* free the shape structure */
    free(s);
}


/* Return a newly allocated shape of a tensor after transposing modes i and j. */
sptensor_shape_t* sptensor_transpose_shape(sptensor_t *t, unsigned int i, unsigned int j)
{
    sptensor_shape_t *result;
    sptensor_index_t tmp;

    /* allocate the shape and populate it with the original modes */
    result = sptensor_shape_alloc(t->modes);
    if(!result) {
        return NULL;
    }
    sptensor_index_cpy(t->modes, result->dim, t->dim);

    /* swap modes i and j */
    tmp = result->dim[i];
    result->dim[i] = result->dim[j];
    result->dim[j] = tmp;

    return result;
}


/* Transpose modes i and j of t and store the result in rop. */
void sptensor_transpose(sptensor_t *rop, sptensor_t *t, unsigned int i, unsigned int j)
{
    sptensor_index_t *idx;
    sptensor_index_t tmp;
    mpf_t v;
    sptensor_iterator_t *itr;

    /* allocate space for the index and the value */
    idx = sptensor_index_alloc(t->modes);
    mpf_init(v);

    /* iterate over the non-zero elements and copy the value to a
     * transposed entry. */
    for(itr = sptensor_nz_iterator(t);
        sptensor_iterator_valid(itr);
        sptensor_iterator_next(itr))
    {
        /* transpose the index */
        sptensor_index_cpy(t->modes, idx, itr->index);
        tmp = idx[i];
        idx[i] = idx[j];
        idx[j] = tmp;

        /* get the value and assign it to rop */
        sptensor_get(t, itr->index, v);
        sptensor_set(rop, idx, v);
    }

    /* clean up */
    sptensor_index_free(idx);
    sptensor_iterator_free(itr);
    mpf_clear(v);
}


/* Return a newly allocated shape of a tensor after it has been
 * unfolded along mode  n. 
 */
sptensor_shape_t* sptensor_unfold_shape(sptensor_t *t, unsigned int n)
{
    sptensor_shape_t* shape;
    int m;

    /* attempt to create the shape */
    shape = sptensor_shape_alloc(2);
    if(!shape) {
        return NULL;
    }

    /* compute the dimensions of the unfolded matrix */
    shape->dim[0]=t->dim[n];
    shape->dim[1]=1;
    for(m=0; m < t->modes; m++) {
        /* skip the unfolding mode */
        if(m == n) {
            continue;
        }
        shape->dim[1] *= t->dim[m];
    }

    return shape;
}


/* A static helper function to compute the Jk coeffecients for an unfolded 
   tensor.*/
static void sptensor_unfold_jk(sptensor_index_t *jk, sptensor_index_t *dim, unsigned int modes, unsigned int n)
{
    unsigned int k;
    sptensor_index_t product=1;

    /* compute each JK coeffecient */
    for(k=0; k<modes; k++) {
        jk[k] = product;

        /* multiply in this dimension (if it is not n) */
        if(k != n) {
            product *= dim[k];
        }
    }
}


/* A static helper function to compute the j component of an index
   given the jk coeffecients and index information */
static sptensor_index_t sptensor_unfold_j(sptensor_index_t *jk, sptensor_index_t *index, unsigned int modes, unsigned int n)
{
    sptensor_index_t j = 1;
    unsigned int k;

    for(k=0; k<modes; k++) {
        /* skip the unfolding mode */
        if(k==n) { continue; }

        j += (index[k]-1)*jk[k];
    }

    return j;
}


/* Unfold tensor t along mode n and store the result in rop */
void sptensor_unfold(sptensor_t *rop, sptensor_t *t, unsigned int n)
{
    sptensor_index_t *jk;
    sptensor_iterator_t *itr;
    sptensor_index_t *index;
    mpf_t v;

    /* allocate some memory */
    index = sptensor_index_alloc(2);
    jk = sptensor_index_alloc(t->modes);
    mpf_init(v);

    /* compute the jk indexes */
    sptensor_unfold_jk(jk, t->dim, t->modes, n);

    /* populate rop with everything at the folded coordinates */
    for(itr = sptensor_nz_iterator(t); 
        sptensor_iterator_valid(itr); 
        sptensor_iterator_next(itr)) 
    {
        index[0] = itr->index[n];
        index[1] = sptensor_unfold_j(jk, itr->index, t->modes, n);
        sptensor_get(t, itr->index, v);
        sptensor_set(rop, index, v);
    }

    /* cleanup */
    sptensor_iterator_free(itr);
    mpf_clear(v);
    sptensor_index_free(jk);
    sptensor_index_free(index);
}


/* Return a newly allocated shape of a tensor after it has been sliced
 * according to the sptensor_index_t* spec. The spec works as follows:
 *   0  - This mode is freely varying and will therefore run the range
 *        from the tensor.
 *   ow - This mode is fixed, and only entries matching this value
 *        will be present.
 * The resultant slice will only contain the free modes.
 */
sptensor_shape_t* sptensor_slice_shape(sptensor_t *t, sptensor_index_t *spec)
{
    unsigned int fm=0; /* number of free modes */
    sptensor_shape_t *result; 
    int i;

    /* count the number of free modes */
    for(i=0; i<t->modes; i++) {
        if(spec[i] == 0) {
            fm++;
        }
    }

    /* create the shape */
    result = sptensor_shape_alloc(fm);

    /* copy the free mode dimensions */
    fm=0;
    for(i=0; i<t->modes; i++) {
        if(spec[i] == 0) {
            result->dim[fm] = t->dim[i];
            fm++;
        }
    }

    return result;
}


/* Slice tensor t according to the spec and store the result in rop.
 * The spec works as follows:
 *   0  - This mode is freely varying and will therefore run the range
 *        from the tensor.
 *   ow - This mode is fixed, and only entries matching this value
 *        will be present.
 * The resultant slice will only contain the free modes.
 */
void sptensor_slice(sptensor_t *rop, sptensor_t *t, sptensor_index_t *spec)
{
    sptensor_index_t *index;
    sptensor_iterator_t *itr;
    unsigned int i, j;
    unsigned int in_slice;
    mpf_t v;

    /* create the index and value */
    index = sptensor_index_alloc(t->modes);
    mpf_init(v);

    /* Copy non-zero elements in the slice */
    for(itr = sptensor_nz_iterator(t); 
        sptensor_iterator_valid(itr); 
        sptensor_iterator_next(itr))
    {
        /* check for the modal match, and grab the free modes */
        j=0;
        in_slice = 1;
        for(i=0; i<t->modes; i++) {
            /* Grab Free Modes */
            if(spec[i]==0) {
                index[j++] = itr->index[i];
            } else if(itr->index[i] != spec[i]){
                /* this means it is not in our fiber/slice */
                in_slice=0;
                break;
            }
        }

        if(in_slice) {
            sptensor_get(t, itr->index, v);
            sptensor_set(rop, index, v);
        }
    }

    /* clean up */
    mpf_clear(v);
    sptensor_index_free(index);
    sptensor_iterator_free(itr);
}