/*
    This is a collection of functions which implement various
    multiplications of tensor views.
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
#include <string.h>
#include <sptensor/multiply.h>


/* Matrix mulitplication between two tensor views, resulting in a
   sparse tensor view containing a newly allocated sparse tensor. */
tensor_view *
matrix_product(tensor_view *a, tensor_view *b)
{
    tensor_view *result;          /* the result */
    sp_index_t rdim[2];           /* result dimensions */
    int i, j;                     /* indexes for loooping over indexes */
    int annz, bnnz;               /* nnz numbers for both tensors */
    sp_index_t aidx[2], bidx[2];  /* a index and b index */
    sp_index_t ridx[2];           /* result index */
    double val;                   /* working value */

    /* compute the dimensions and allocate the tensor */
    rdim[0] = a->dim[0];
    rdim[1] = b->dim[1];
    result = tensor_alloc(2, rdim);

    /* perform the multiplication in an O(n^2 lg n) sort of way */
    annz = TVNNZ(a);
    bnnz = TVNNZ(b);
    for(i=0; i<annz; i++) {
	/* get the index */
	TVIDX(a, i, aidx);

	/* loop over b */
	for(j=0; j<bnnz; j++) {
	    /* get the index */
	    TVIDX(b, j, bidx);

	    /* see if this is something which needs multiplying */
	    if(aidx[1] == bidx[0]) {
		/* perform the multiplication and summation */
		ridx[0] = aidx[0];
		ridx[1] = bidx[1];
		val = TVGET(a, aidx) * TVGET(b, bidx);
		val += TVGET(result, ridx);
		TVSET(result, ridx, val);
	    }
	}
    }

    return result;
}


/* N-Mode multiplication of tensor a by matrix u */
tensor_view *
nmode_product(unsigned int n, tensor_view *a, tensor_view *u)
{
    tensor_view *result;      /* the resultant tensor */
    sp_index_t *idx;          /* general index a->nmodes entries */
    sp_index_t *aidx;         /* index into the tensor */
    sp_index_t uidx[2];       /* index into the matrix */
    int i, j;                 /* indexes */
    unsigned int annz, unnz;  /* non-zero counts for each tensor */
    double val;               /* product value */

    /* allocate the indexes */
    idx = malloc(sizeof(sp_index_t)*a->nmodes);
    aidx = malloc(sizeof(sp_index_t)*a->nmodes);

    /* create the dimensions of the result and allocatethe result. */
    memmove(idx, a->dim, sizeof(sp_index_t)*a->nmodes);
    idx[n] = u->dim[0];
    result = tensor_alloc(a->nmodes, idx);

    /* go through each index in a */
    annz = TVNNZ(a);
    unnz = TVNNZ(u);
    for(i=0; i<annz; i++) {
	/* get the index */
	TVIDX(a, i, aidx);

	/* loop over u */
	for(j=0; j<unnz; j++) {
	    TVIDX(u, j, uidx);

	    /* if this is a matched pair, add to the accumulated sum */
	    if(aidx[n] == uidx[1]) {
		memmove(idx, aidx, sizeof(sp_index_t)*a->nmodes);
		idx[n] = uidx[0];
		val = TVGET(a, aidx) * TVGET(u, uidx);
		val += TVGET(result, idx);
		TVSET(result, idx, val);
	    }
	}
    }

    /* cleanup and return */
    free(idx);
    free(aidx);
    return result;
}


/* Outer (tensor) product of two tensor views */
tensor_view *tensor_product(tensor_view *a, tensor_view *b)
{
    tensor_view *result;     /* the resultant tensor */
    sp_index_t *idx;         /* insertion index */
    sp_index_t *aidx, *bidx; /* tensor indexes */
    int i, j;                /* indexes */
    unsigned int annz, bnnz; /* non-zero counts for each tensor */

    /* create the dimension, and allocate the tensor */
    idx = malloc(sizeof(sp_index_t)*(a->nmodes + b->nmodes));
    memcpy(idx, a->dim, sizeof(sp_index_t) * a->nmodes);
    memcpy(idx+a->nmodes, b->dim, sizeof(sp_index_t) * b->nmodes);
    result = tensor_alloc(a->nmodes+b->nmodes, idx);

    /* allocate indexes */
    aidx = malloc(sizeof(sp_index_t) * a->nmodes);
    bidx = malloc(sizeof(sp_index_t) * b->nmodes);

    /* multiply each pairing */
    annz = TVNNZ(a);
    bnnz = TVNNZ(b);
    for(i=0; i<annz; i++) {
	/* get the index */
	TVIDX(a, i, aidx);
	
	for(j=0; j<bnnz; j++) {
	    /* get the index */
	    TVIDX(b, j, bidx);

	    /* concatenate the indexes to find the result index */
	    memcpy(idx, aidx, sizeof(sp_index_t) * a->nmodes);
	    memcpy(idx+a->nmodes, bidx, sizeof(sp_index_t) * b->nmodes);

	    /* do the multiplication and put it in the result */
	    TVSET(result, idx, TVGET(a, aidx) * TVGET(b, bidx));
	}
    }

    /* cleanup and return */
    free(aidx);
    free(bidx);
    free(idx);
    return result;
}
