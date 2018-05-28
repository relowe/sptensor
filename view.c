/*
    This is a collection of functions for dealing with tensor views.  
    Copyright (C) 2018 Robert Lowe <pngwen@acm.org>

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
#include <string.h>
#include <view.h>

/********************************
 * generic tensor view functions 
 ********************************/

/* coordinate list print */
void
tensor_view_clprint(tensor_view *v)
{
    sp_index_t *idx;
    int i,j;
    int nnz;
    
    /* get the preliminary info */
    nnz = TVNNZ(v);
    idx=TVIDX_ALLOC(v);

    /* print the header */
    printf("%u", v->nmodes);
    for(j=0; j<v->nmodes; j++) {
	printf("\t%u", v->dim[j]);
    }
    printf("\n");

    /* go through the list, printing the tensor */
    for(i=0; i<nnz; i++) {
	TVIDX(v, i, idx);
	
	/* print the index */
	for(j=0; j<v->nmodes; j++) {
	    printf("%d\t", idx[j]);
	}

	/* print the value */
	printf("%g\n", TVGET(v,idx));
    }

    free(idx);
}


/* pretty print */
void
tensor_view_print(tensor_view *v)
{
    /* TODO */
}


/* create an sptensor copy of a tensor view */
sptensor *
tensor_view_sptensor(tensor_view *v)
{
    sptensor *t;
    sp_index_t *idx;
    int i;
    unsigned int nnz;

    /* allocate things */
    idx = TVIDX_ALLOC(v);
    t = sptensor_alloc(v->nmodes, v->dim);
    nnz = TVNNZ(v);

    /* copy elements */
    for(i=0; i<nnz; i++) {
	TVIDX(v, i, idx);
	sptensor_set(t, idx, TVGET(v, idx));
    }

    /* cleanup and return */
    free(idx);
    return t;
}


/* 
 * For most views, these generic functions will do.
 */
static unsigned int
base_view_nnz(tensor_view *v)
{
    /* The assumption is that our NNZ matches that of what we contain */
    return TVNNZ((tensor_view*)v->tns);
}


static void
base_view_get_idx(tensor_view *v, unsigned int i, sp_index_t *idx)
{
    sp_index_t *fidx;  /* idx of our contained view */

    /* get the internal view and the ith index thereof */
    fidx = TVIDX_ALLOC(v->tns);
    TVIDX(v->tns, i, fidx);

    /* convert and copy */
    TVFROM(v, fidx, idx);

    /* cleanup! */
    free(fidx);
}


static double
base_view_get(tensor_view *v, sp_index_t *idx)
{
    sp_index_t *tidx;  /* idx of our contained view */
    double result;

    /* convert the index */
    tidx = TVIDX_ALLOC(v->tns);
    TVTO(v, idx, tidx);

    /* retrieve the result */
    result = TVGET(v->tns, tidx);

    /* cleanup and return */
    free(tidx);
    return result;
}


static void
base_view_set(tensor_view *v, sp_index_t *idx, double value)
{
    sp_index_t *tidx;  /* idx of our contained view */
    double result;

    /* convert the index */
    tidx = TVIDX_ALLOC(v->tns);
    TVTO(v, idx, tidx);

    /* set the value */
    TVSET(v->tns, tidx, value);

    /* cleanup */
    free(tidx);
}


static void
base_view_free(tensor_view *v)
{
    /* generally, just deallocating our internal object 
       and then ourselves will do nicely. */
    if(v->tns) {
	free(v->tns);
    }
    free(v);
}


/********************************************************
 * sptensor view - A simple wrapper for sptensor objects
 ********************************************************/
/* Static functions for sptensor*/
static unsigned int
sptensor_view_nnz(tensor_view *v)
{
    /* just return the number of entries in the tensor */
    return ((sptensor*)v->tns)->ar->size;
}


static void
sptensor_view_get_idx(tensor_view *v, unsigned int i, sp_index_t *idx)
{
    sptensor *tns = (sptensor*) v->tns;
    
    /* no translation, just copy */
    memcpy(idx, VPTR(tns->idx, i), tns->idx->element_size);
}


static double
sptensor_view_get(tensor_view *v, sp_index_t *idx)
{
    /* passthru */
    return sptensor_get((sptensor*)v->tns, idx);
}


static void
sptensor_view_set(tensor_view *v, sp_index_t *idx, double value)
{
    /* passthru */
    sptensor_set((sptensor*)v->tns, idx, value);
}


static void
sptensor_view_idxcpy(tensor_view *v, sp_index_t *in, sp_index_t *out)
{
    /* just copy */
    memcpy(out, in, ((sptensor*)v->tns)->idx->element_size);
}


static void
sptensor_view_free(tensor_view *v)
{
    /* simply free the tensor wrapper */
    free(v);
}


/* allocate an sptensor view */
tensor_view *
sptensor_view_alloc(sptensor *tns)
{
    tensor_view *v;  /* the allocated view */

    /* allocate and populate the view */
    v = malloc(sizeof(tensor_view));
    v->tns = tns;
    v->dim = tns->dim;
    v->nmodes = tns->nmodes;
    v->nnz = sptensor_view_nnz;
    v->get_idx = sptensor_view_get_idx;
    v->get = sptensor_view_get;
    v->set = sptensor_view_set;
    v->to = sptensor_view_idxcpy;
    v->from = sptensor_view_idxcpy;
    v->tvfree = sptensor_view_free;
    return v;
}



/***************************************
 * Identity Tensor
 ***************************************/
static unsigned int
identity_nnz(tensor_view *v)
{
    int min=v->dim[0];
    int i;

    /* find the minimum mode */
    for(int i=0; i<v->nmodes; i++) {
	if(v->dim[i] < min) {
	    min = v->dim[i];
	}
    }

    /* The smallest mode is the number of elements on diagonal */
    return min;
}



static void
identity_get_idx(tensor_view *v, unsigned int i, sp_index_t *idx)
{
    int j;

    /* just repeat i+1 nmodes times */
    for(j=0; j<v->nmodes; j++) {
	idx[j] = i+1;
    }
}


static double
identity_get(tensor_view *v, sp_index_t *idx)
{
    int i;

    /* if they don't match, return 0.0 */
    for(i=1; i<v->nmodes; i++) {
	if(idx[i] != idx[0]) {
	    return 0.0;
	}
    }

    /* looks like they all match! */
    return 1.0;
}


static void
identity_tensor_free(tensor_view *v)
{
    free(v->dim);
    free(v);
}


tensor_view *
identity_tensor(int nmodes, sp_index_t *dim)
{
    tensor_view *v;

    v = malloc(sizeof(tensor_view));
    v->tns = 0x00;
    v->nmodes = nmodes;
    v->dim = TVIDX_ALLOC(v);
    memcpy(v->dim, dim, sizeof(sp_index_t) * nmodes);
    v->nnz = identity_nnz;
    v->get_idx = identity_get_idx;
    v->get = identity_get;
    v->set = 0x00;  /* identity tensors are immutable */
    v->to = sptensor_view_idxcpy; /* just copy */
    v->from = sptensor_view_idxcpy;
    v->tvfree = identity_tensor_free;
    
    return v;
}



/***************************************
 * Unfoleded Tensor View
 ***************************************/
struct unfold_view
{
    int n;             /* Dimension we unfold along */
    sp_index_t *jk;    /* jk coeffecients for unfolding */
    tensor_view *v;    /* the actual tensor view */
    sp_index_t dim[2]; /* This is always 2 mode! */
};


static void
unfold_to(tensor_view *v, sp_index_t *in, sp_index_t *out)
{
}


static void
unfold_from(tensor_view *v, sp_index_t *in, sp_index_t *out)
{
}


static void
unfold_free(tensor_view *v)
{
    struct unfold_view *uv;
    
    uv = (struct unfold_view *) v->tns;
    free(uv->jk);
    free(uv);
    free(v);
}


/* Unfold a tensor along dimension n */
tensor_view *
unfold_tensor(tensor_view *v, sp_index_t n)
{
    tensor_view *tv;
    struct unfold_view *uv;
    int i,k;

    /* allocate and populate the tensor view */
    uv = malloc(sizeof(struct unfold_view));
    uv->n = n;
    uv->v = v;
    tv = malloc(sizeof(tensor_view));
    tv->tns = uv;
    tv->nmodes = 2;
    tv->dim = uv->dim;
    tv->nnz = base_view_nnz;
    tv->get_idx = base_view_get_idx;
    tv->get = base_view_get;
    tv->set = base_view_set;
    tv->to = unfold_to;
    tv->from = unfold_from;
    tv->tvfree = unfold_free;

    /* compute the jk coeffecients */
    uv->jk = malloc(sizeof(sp_index_t) * (v->nmodes - 1));
    k=0;
    for(i=0; i<v->nmodes; i++) {
	/* skip the unfold dimension */
	if(i == n) continue;

	/* handle the special case */
	if(k == 0) {
	    uv->jk[k++] = 1;
	} else {
	    /* multiply current dim by previous multiples */
	    uv->jk[k] = v->dim[i] * uv->jk[k-1];
	    k++;
	}
    }

    return tv;
}
