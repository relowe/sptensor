/*
    This is a collection of functions which perform Columnwise 
    Coordinate Descent Non-Negative Decomposition on a Tensor
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
#include <math.h>
#include <ccd.h>
#include <tensor_math.h>
#include <multiply.h>
#include <binsearch.h>

#define MAX(a, b) ((a)>(b) ? (a) : (b))

/* static prototypes */
static void ccd_update(tensor_view *an, tensor_view *bn, double ln,
		       tensor_view *un, int max_iter, double tol);

/*
 * Perform CCD non-negative tensor factorization using an identity 
 * tensor of the specified sized as the core tensor.
 * Parameters:
 *      a      - The tensor to be factored
 *      n      - The number of elements in the core tensors 
 *               (ie the number of factors)
 *      lambda - A set of L1 sparsity constraints.  This array must 
 *               have one entry for each mode of a
 * Returns:  An array of tensor view pointerss representing the factors.
 *           One matrix is returned for each mode. The array is terminated
 *           by a null pointer.
 */
tensor_view **
ccd_identity(tensor_view *a, unsigned int n, double lambda[],
	     tensor_view *un, int max_iter, double tol)
{
    tensor_view *c;
    sp_index_t *dim;
    tensor_view **u;
    int i;

    /* create the core tensor */
    dim = malloc(sizeof(sp_index_t) * a->nmodes);
    for(i=0; i<a->nmodes; i++) {
	dim[i] = n;
    }
    c = identity_tensor(a->nmodes, dim);

    /* run the algorithm */
    u = ccd_core(a, c, lambda);

    /* clean up and return */
    free(dim);
    TVFREE(c);
    
    return u;
}


/*
 * Perform CCD non-negative tensor factorization using the specified 
 * core tensor.
 * Parameters:
 *      a      - The tensor to be factored
 *      n      - The number of elements in the core tensors 
 *               (ie the number of factors)
 *      lambda - A set of L1 sparsity constraints.  This array must 
 *               have one entry for each mode of a
 * Returns:  An array of tensor view pointerss representing the factors.
 *           One matrix is returned for each mode. The array is terminated
 *           by a null pointer.
 */
tensor_view **
ccd_core(tensor_view *a, tensor_view *c, double labmda[],
	 tensor_view *un, int max_iter, double tol)
{
    return 0x00;
}


/*
 * Free's the ccd factor list.
 * Parameters;
 *     u - A factor list returned by a ccd composition
 */
void
ccd_free(tensor_view **u)
{
    tensor_view **ptr;

    /* loop over the factor array, freeing as we go */
    for(ptr = u; *ptr; ptr++) {
	TVFREE(*ptr);
    }

    /* dispose of the list of pointers */
    free(ptr);
}


static
int setcmp(int element_isze, void *a, void *b)
{
    int *x = (int*)a;
    int *y = (int*)b;

    if(*x < *y) return -1;
    if(*x > *y) return 1;
    return 0;
}

static void
nzrows(vector *v, tensor_view *t)
{
    int i;
    sp_index_t r[2];
    int nnz = TVNNZ(t);

    for(i=0; i<nnz; i++) {
	TVIDX(t, i, r);
	vector_set_insert(v, r, setcmp);
    }
}


static void ccd_update(tensor_view *an, tensor_view *bn, double ln,
		       tensor_view *un, int max_iter, double tol)
{
    tensor_view *bnt;  /* transpose of bn */
    tensor_view *m;
    tensor_view *n;
    tensor_view *d;
    tensor_view *unm;
    tensor_view *mj;
    tensor_view *nj;
    tensor_view *unlast;
    vector *rows;
    sp_index_t idx[2];
    sp_index_t j;
    sp_index_t jmax;
    int iter=0;
    int i;
    int nnz;
    double val;
    double error = INFINITY;
    tensor_slice_spec *slice;

    /* get preliminary things set up */
    jmax = un->dim[1];
    bnt = tensor_transpose(bn, 0, 1);
    rows = vector_alloc(sizeof(sp_index_t), 128);
    slice = tensor_slice_spec_alloc(un);
    slice->begin[0] = 1;
    slice->begin[1] = 1;
    slice->end[0] = un->dim[0];
    slice->end[1] = un->dim[1];

    /* compute m and n */
    m = matrix_product(bn, bnt);
    n = matrix_product(an, bnt);
    nnz = TVNNZ(n);
    for(i=0; i<nnz; i++) {
	TVIDX(n, i, idx);
	TVSET(n, idx, TVGETI(n, i) - ln);
    }

    /* compute d and zero M's diagonal */
    idx[0] = jmax;
    d = sptensor_view_tensor_alloc(1, idx);
    for(j=1; j<=jmax; j++) {
	idx[0] = idx[1] = j;
	TVSET(d, idx, TVGET(m, idx));
	TVSET(m, idx, 0);
    }

    /* run the update loop */
    while(error > tol && iter < max_iter) {
	unlast = tensor_view_deep_copy(un);

	/* update each column */
	for(j=1; j <= jmax; j++) {
	    slice->fixed[1] = j;
	    nj = tensor_slice(n, slice);
	    mj = tensor_slice(m, slice);
	    unm = matrix_product(un, m);
	    
	    /* extract rows */
	    rows->size=0;
	    nzrows(rows, un);
	    nzrows(rows, unm);
	    nzrows(rows, nj);

	    /* update un */
	    for(i=0; i<rows->size; i++) {
		idx[0] = VVAL(sp_index_t, rows, i);
		idx[1] = j;
		val = TVGET(n, idx) - TVGET(unm, idx);
		val /= TVGET(d, &j);
		TVSET(un, idx, MAX(0, val));
	    }

	    TVFREE(nj);
	    TVFREE(mj);
	    TVFREE(unm);
	}

	/* compute the error and count the iterations */
	tensor_decrease(unlast, un);
	error = tensor_lpnorm(unlast, 2.0);
	iter++;

	/* cleanup */
	TVFREE(unlast);
    }

    /* cleanup! */
    TVFREE(m);
    TVFREE(n);
    TVFREE(d);
    vector_free(rows);
    tensor_slice_spec_free(slice);
}
