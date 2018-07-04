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
#ifndef VIEW_H
#define VIEW_H
#include <stdlib.h>
#include <stdio.h>
#include <sptensor/storage.h>

/* struct prototype */
struct tensor_view;
typedef struct tensor_view tensor_view;

/* function pointer types */
typedef unsigned int (*nnz_func)(tensor_view *);
typedef void (*index_func)(tensor_view*, unsigned int i, sp_index_t *);
typedef double (*geti_func)(tensor_view*, unsigned int i);
typedef double (*get_func)(tensor_view*, sp_index_t*);
typedef void (*set_func)(tensor_view*, sp_index_t*, double);
typedef void (*index_trans_func)(tensor_view*, sp_index_t*, sp_index_t*);
typedef void (*tensor_view_free_func)(tensor_view*);

struct tensor_view {
    tensor_view *tns;      /* The underlying object */
    void *data;            /* Data to help the view */
    sp_index_t *dim;       /* The dimneison of view */
    unsigned int nmodes;   /* The mumber of modes in the view */
    nnz_func nnz;          /* Get the number of nonzero entries */
    index_func get_idx;    /* Get the ith index */
    geti_func geti;        /* Get the ith non-negative value */
    get_func get;          /* Get the value at index */
    set_func set;          /* Set the value at index */
    index_trans_func to;   /* translate an index to the base tns index */
    index_trans_func from; /* translate an index from the base tns index */
    tensor_view_free_func tvfree;
};

typedef struct tensor_slice_spec {
    sp_index_t *fixed; /* 0 if free fixed index if this is a fixed 
                          dimension (and therefore not part of the 
			  tensor view dimension */
    sp_index_t *begin; /* beginning value for this slice spec*/
    sp_index_t *end;   /* ending value for this slice spec */
    /* NOTE: Begin and end are inclusive.  Every index in the spec is:
             begin <= idx <= end 
    */
} tensor_slice_spec;

/* Macro wrappers for function pointers and others */
#define TVNNZ(v) (*((tensor_view*)(v))->nnz)((tensor_view*)(v))
#define TVIDX(v,i, idx) (*((tensor_view*)(v))->get_idx)((tensor_view*)(v), (i), (idx))
#define TVGETI(v,i) (*((tensor_view*)(v))->geti)((tensor_view*)(v), (i))
#define TVGET(v,i) (*((tensor_view*)(v))->get)((tensor_view*)(v), (i))
#define TVSET(v,i,value) (*((tensor_view*)(v))->set)((tensor_view*)(v),(i),(value))
#define TVTO(v,in,out) (*((tensor_view*)(v))->to)((tensor_view*)(v), (in), (out))
#define TVFROM(v,in,out) (*((tensor_view*)(v))->from)((tensor_view*)(v),(in),(out))
#define TVFREE(v) (*((tensor_view*)(v))->tvfree)(((tensor_view*)v))
#define TVIDX_ALLOC(v) malloc(sizeof(sp_index_t) * ((tensor_view*)(v))->nmodes)
#define TVNMODES(v) (((tensor_view*)(v))->nmodes)

/********************************
 * generic tensor view functions 
 ********************************/
/* write coordinate list to file */
void tensor_view_write(FILE *file, tensor_view *v);

/* coordinate list print */
void tensor_view_clprint(tensor_view *v);

/* pretty print */
void tensor_view_print(tensor_view *v, unsigned int precision);   

/* create an sptensor copy of a tensor view */
sptensor *tensor_view_sptensor(tensor_view *v);

/* allocate an sptensor view and create a new sptensor to fill it.
   This uses the base free, which will deallocate the underlying sptensor
   when the view is freed.

   nmodes - The number of modes
   dim    - The dimensions of the tensor
*/
tensor_view *sptensor_view_tensor_alloc(int nmodes, sp_index_t *dim);

/* Copy a tensor view as a newly allocated tensor with underlying sptensor */
tensor_view *tensor_view_deep_copy(tensor_view *t);


/* 
 * VIEW - sptensor view.  Wraps an sptensor and does no translation.  
 *        The intention here is to work as a base of a chain of views or
 *        to use raw sptensors with all the math functions that use views.
 */
tensor_view *sptensor_view_alloc(sptensor *tns);

/* Unfold a tensor along dimension n */
tensor_view *unfold_tensor(tensor_view* v, sp_index_t n);

/* An identity tensor (with 1's along its superdiagonal) */
tensor_view *identity_tensor(int nmodes, sp_index_t *dim);

/*
 * Allocate a tensor slice spec for the given view.
 */
tensor_slice_spec *tensor_slice_spec_alloc(tensor_view *v);

/*
 * Free a tensor slice spec
 */
void tensor_slice_spec_free(tensor_slice_spec *spec);

/* A slice of a tensor 
 *   v    - The view to slice
 *   spec - The slice spec
 */
tensor_view *tensor_slice(tensor_view *v, tensor_slice_spec *spec);

 
/* A view with two indices transposed
 *   v - The view to transpose
 *   i - The first mode to swap
 *   j - The second mode to swap
 */
tensor_view *tensor_transpose(tensor_view *v, unsigned int i, unsigned int j);


#endif
