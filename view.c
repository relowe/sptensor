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


/* 
 * For most views, these generic functions will do.
 */
static int
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
static int
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
