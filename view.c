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
#include <math.h>
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


/* 
 * Find the maximum number of digits to the left 
 * of the decimal place in the view (including -)
 */
static unsigned int
tensor_view_max_digits(tensor_view* v)
{
    double maxVal=0.0;
    double val, absval;
    int i;
    unsigned add = 0;
    unsigned int nnz;

    /* find the biggest absolute value */
    nnz = TVNNZ(v);
    for(i=0; i<nnz; i++) {
	val = TVGETI(v, i);
	absval = fabs(val);
	if( absval > maxVal) {
	    if(val < 0) {
		add = 1;
	    } else {
		add = 0;
	    }
	    maxVal = val;
	}
    }

    return (unsigned int) ceil(log10(maxVal))+add;
}


/* pretty print */
void
tensor_view_print(tensor_view *v, unsigned int precision)
{
    unsigned int width;
    int ui;  /* index being updated */
    sp_index_t *idx = TVIDX_ALLOC(v);
    short unsigned int done = 0;

    /* get the field width */
    width = tensor_view_max_digits(v) + 1;
    if(precision) {
	width += 1 + precision; /* allow for . and fractional part */
    }

    /* start at index 1...1 */
    for(ui=0; ui<v->nmodes; ui++) {
	idx[ui]=1;
    }

    /* iterate over all of the indexes and print in a row/slice 
     * major way
     */
    while(!done) {
	/* print the value */
	printf("\t%*.*lf", width, precision,TVGET(v, idx));

	/* detect if we are done */
	if(sptensor_indexcmp(v->nmodes, idx, v->dim) == 0) {
	    done = 1;
	}

	/* update the index */
	if(v->nmodes >= 2) {
	    ui = 1; /* columns first! */
	} else {
	    ui = 0;
	}

	/* look for the next index to increment */
	while(ui < v->nmodes && idx[ui] == v->dim[ui]) {
	    /* rollover! */
	    idx[ui] = 1;

	    if(ui == 0) {
		ui = 2; 
	    } else if(ui == 1) {
		ui = 0;
	    } else {
		ui++;
	    }
	}

	if(ui >= v->nmodes) {
	    ui = 0;
	}
	
	/* increment */
	idx[ui]++;

	/* terminate rows */
	if(ui == 0) {
	    printf("\n");
	} else if(ui != 1) {
	    printf("\n\n"); /* blank line separator */
	}
    }

    free(idx);
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
base_view_geti(tensor_view *v, unsigned int i)
{
    /* just pass it along! */
    return TVGETI(v->tns, i);
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


static tensor_view *
base_view_alloc()
{
    tensor_view *v;

    /* basic allocation with base pointers to functions */
    v = malloc(sizeof(tensor_view));
    v->tns = NULL;
    v->dim = NULL;
    v->nmodes = 0;
    v->nnz = base_view_nnz;
    v->get_idx = base_view_get_idx;
    v->geti = base_view_geti;
    v->get = base_view_get;
    v->set = base_view_set;
    v->to = NULL;
    v->from = NULL;
    v->tvfree = base_view_free;
    return v;
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
sptensor_view_geti(tensor_view *v, unsigned int i)
{
    sptensor *tns = (sptensor*) v->tns;

    return VVAL(double, tns->ar, i);
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
    v = base_view_alloc();
    v->tns = tns;
    v->dim = tns->dim;
    v->nmodes = tns->nmodes;
    v->nnz = sptensor_view_nnz;
    v->get_idx = sptensor_view_get_idx;
    v->geti = sptensor_view_geti;
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


static double
identity_tensor_geti(tensor_view *v, unsigned int i)
{
    return 1.0;
}


tensor_view *
identity_tensor(int nmodes, sp_index_t *dim)
{
    tensor_view *v;

    v = base_view_alloc();
    v->nmodes = nmodes;
    v->dim = TVIDX_ALLOC(v);
    memcpy(v->dim, dim, sizeof(sp_index_t) * nmodes);
    v->nnz = identity_nnz;
    v->get_idx = identity_get_idx;
    v->geti = identity_tensor_geti;
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
    tv = base_view_alloc();
    tv->tns = uv;
    tv->nmodes = 2;
    tv->dim = uv->dim;
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
