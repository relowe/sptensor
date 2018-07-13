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
#include <sptensor/ccd.h>
#include <sptensor/tensor_math.h>
#include <sptensor/multiply.h>
#include <sptensor/binsearch.h>
#define MAX(a, b) ((a)>(b) ? (a) : (b))

/* static prototypes */
static void ccd_update(tensor_view *an, tensor_view *bn, double ln,
		       tensor_view *un, int max_iter, double tol);
static void ccd_un_init(ccd_result *result, tensor_view *a, int n);
static tensor_view *ccd_compute_bn(ccd_result *result, int n);
static void ccd_bn_free(tensor_view *bn);


/*
 * Perform CCD non-negative tensor factorization using an identity 
 * tensor of the specified sized as the core tensor.
 * Parameters:
 *      a      - The tensor to be factored
 *      n      - The number of elements in the core tensors 
 *               (ie the number of factors)
 *      lambda - A set of L1 sparsity constraints.  This array must 
 *               have one entry for each mode of a
 * Returns:  A ccd_result structure.  This structure should be deallocated
 *           by ccd_free when it is no longer needed.
 */
ccd_result *
ccd_identity(tensor_view *a, unsigned int n, double lambda[],
	     int max_iter, double tol)
{
    tensor_view *c;
    sp_index_t *dim;
    ccd_result *result;
    int i;

    /* create the core tensor */
    dim = malloc(sizeof(sp_index_t) * a->nmodes);
    for(i=0; i<a->nmodes; i++) {
	dim[i] = n;
    }
    c = identity_tensor(a->nmodes, dim);

    /* run the algorithm */
    result = ccd_core(a, c, lambda, max_iter, tol);

    /* clean up and return */
    free(dim);
    TVFREE(c);
    
    return result;
}


/*
 * Perform CCD non-negative tensor factorization using the specified 
 * core tensor.
 * Parameters:
 *      a      - The tensor to be factored
 *      c      - The core tensor to use for factorization
 *      lambda - A set of L1 sparsity constraints.  This array must 
 *               have one entry for each mode of a
 * Returns:  A ccd_result structure.  This structure should be deallocated
 *           by ccd_free when it is no longer needed.
 */
ccd_result *
ccd_core(tensor_view *a, tensor_view *c, double lambda[],
	 int max_iter, double tol)
{
    ccd_result *result;
    tensor_view *unlast;
    tensor_view *bn, *b;
    tensor_view **a_unfold;
    double max_error;
    double error;
    int i;

    /* allocate and initialize the result */
    result = malloc(sizeof(ccd_result));
    result->core = tensor_view_deep_copy(c);
    result->n = a->nmodes;
    result->u = malloc(sizeof(tensor_view*)*result->n);
    result->iter = 0;
    result->final_error = HUGE_VAL;
    result->fit = 0;
    for(i=0; i<result->n; i++) {
	ccd_un_init(result, a, i);
    }

    /* create the unfolds for a */
    a_unfold = malloc(sizeof(tensor_view*) * result->n);
    for(i=0; i<result->n; i++) {
	a_unfold[i] = unfold_tensor(a, i);
    }

    /* run the iterations */
    while(result->final_error > tol && result->iter < max_iter) {
	/* run the updates */
	max_error = 0;
	printf("Iteration: %d\n", result->iter);
	for(i=0; i<result->n; i++) {
	    unlast = tensor_view_deep_copy(result->u[i]);
	    bn = ccd_compute_bn(result, i);
	    ccd_update(a_unfold[i], bn, lambda[i], result->u[i], max_iter, tol);
	    ccd_bn_free(bn);

	    /* compute the error */
	    tensor_decrease(unlast, result->u[i]);
	    error = tensor_lpnorm(unlast, 2.0);
	    if(error > max_error) {
		max_error = error;
	    }

	    /* cleanup */
	    TVFREE(unlast);
	}

	/* update iteration stats */
	result->final_error = max_error;
	result->iter++;
    }

    /* compute the fit */
    b = ccd_construct(result);
    tensor_decrease(b, a);
    result->fit = 1.0 - tensor_lpnorm(b, 2.0)/tensor_lpnorm(a, 2.0);
    TVFREE(b);

    /* cleanup */
    for(i=0; i<result->n; i++) {
	TVFREE(a_unfold[i]);
    }
    free(a_unfold);
    
    return result;
}


/* 
 * Constructs the tensor described by the ccd_result structure 
 */
tensor_view *
ccd_construct(ccd_result *result)
{
    tensor_view *t;
    tensor_view *prev;
    int i;        

    /* we begin with the core */
    t = tensor_view_deep_copy(result->core);

    for(i=0; i<result->n; i++) {
	/* perform the multiplication */
	prev = t;
	t = nmode_product(i, t, result->u[i]);
	TVFREE(prev);
    }

    return t;
}


/*
 * Free's the ccd result
 * Parameters;
 *     result - The result to free
 */
void
ccd_free(ccd_result *result)
{
    int i;

    /* loop over the factor array, freeing as we go */
    for(i=0; i<result->n; i++) {
	TVFREE(result->u[i]);
    }

    /* dispose of the rest */
    free(result->u);
    free(result->core);
    free(result);
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
    double error = HUGE_VAL;
    tensor_slice_spec *slice;
    double dj;

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
        printf("    Iteration: %d\n", iter);
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
		dj = TVGET(d, &j);
		if(dj != 0.0) {
		    val /= TVGET(d, &j);
		} else {
		    val = 0;
		}
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


static void
ccd_un_init(ccd_result *result, tensor_view *a, int n)
{
    sp_index_t dim[2];
    sp_index_t *idx;
    int i, j;
    int nnz;
    double min = HUGE_VAL;
    double max = -HUGE_VAL;
    double val;
    vector *rows;

    /* first, set up the dimensions of U[i] and allocate */
    dim[0] = a->dim[n];  /* I_n from the paper */
    dim[1] = result->core->dim[n]; /* J_n from the paper */
    result->u[n] = sptensor_view_tensor_alloc(2, dim);

    /* build a list of rows to populate in U_n, and find min and max */
    idx = malloc(sizeof(sp_index_t) * a->nmodes);
    nnz = TVNNZ(a);
    rows = vector_alloc(sizeof(sp_index_t), 128);
    for(i=0; i<nnz; i++) {
	TVIDX(a, i, idx);
	val = TVGETI(a, i);
	if(val < min) {
	    min = val;
	}
	if(val > max) {
	    max = val;
	}
	vector_set_insert(rows, idx+n, setcmp);
    }
    free(idx);

    /* populate the rows with random values between min and max */
    printf("U%d: %d\n", n, rows->size);
    for(i=0; i<rows->size; i++) {
	for(j=1; j<=result->u[n]->dim[1]; j++) {
	    val = ((double)rand()/RAND_MAX) * (max-min) + min;
	    dim[0] = VVAL(sp_index_t, rows,i);
	    dim[1] = j;
	    TVSET(result->u[n], dim, val);
	}
    }
    
    /* cleanup */
    vector_free(rows);
}


static tensor_view *
ccd_compute_bn(ccd_result *result, int n)
{
    tensor_view *bn;
    tensor_view *prev;
    int i;        

    /* we begin with the core */
    bn = tensor_view_deep_copy(result->core);

    for(i=0; i<result->n; i++) {
	/* skip the nth product */
	if(i == n) continue;
	

	/* perform the multiplication */
	prev = bn;
	bn = nmode_product(i, bn, result->u[i]);
	TVFREE(prev);
    }

    return unfold_tensor(bn, n);
}


static void
ccd_bn_free(tensor_view *bn)
{
    /* free both levels, the real tensor and the unfolded one */
    TVFREE(bn->tns);
    TVFREE(bn);
}
