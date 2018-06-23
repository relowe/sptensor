/*
    This is a collection of functions for performing basic tensor arithmetic.
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
#include <tensor_math.h>

/* The operations that create a new tensor_view need to produce a copy */
static tensor_view *
tensor_alloc_cpy(tensor_view *t)
{
    tensor_view *result;
    int i;
    int nnz;
    sp_index_t *idx;

    /* allocate the result */
    result = sptensor_view_tensor_alloc(t->nmodes, t->dim);
    idx = malloc(sizeof(sp_index_t)*t->nmodes);

    /* copy the non-zero elements of a */
    nnz = TVNNZ(t);
    for(i=0; i<nnz; i++) {
	TVIDX(t, i, idx);
	TVSET(result, idx, TVGETI(t, i));
    }

    /* cleanup and return */
    free(idx);
    return result;
}


/* add two tensors. elmentwise (returns a+b) */
tensor_view *
tensor_add(tensor_view *a, tensor_view *b)
{
    tensor_view *result;

    /* first, copy the tensor */
    result = tensor_alloc_cpy(a);

    /* apply the tensor increase */
    tensor_increase(result, b);

    return result;
}


/* subtract two tensors, elementwise (returns a-b) */
tensor_view *
tensor_sub(tensor_view *a, tensor_view *b)
{
    tensor_view *result;

    /* first, copy the tensor */
    result = tensor_alloc_cpy(a);

    /* apply the tensor decrease */
    tensor_decrease(result, b);

    return result;
}


/* multiply the tensor by a scalar (returns t*s) */
tensor_view *
scalar_mul(tensor_view *t, double s)
{
    tensor_view *result;

    /* first, copy the tensor */
    result = tensor_alloc_cpy(t);

    /* apply the tensor scale */
    tensor_scale(result, s);

    return result;

}


/* divide the tensor by a scalar (returns t/s) */
tensor_view *
scalar_div(tensor_view *t, double s)
{
    tensor_view *result;

    /* first, copy the tensor */
    result = tensor_alloc_cpy(t);

    /* apply the tensor scale */
    tensor_scale(result, 1.0/s);

    return result;
}


/* inplace addition of two tensors (a+=b) */
void
tensor_increase(tensor_view *a, tensor_view *b)
{
    sp_index_t *idx;
    int i;
    int nnz;

    /* allocate the index */
    idx = malloc(sizeof(sp_index_t) * a->nmodes);

    /* modify all the elements */
    nnz = TVNNZ(b);
    for(i=0; i<nnz; i++) {
	TVIDX(b, i, idx);
	TVSET(a, idx, TVGET(a,idx)+TVGETI(b,i));
    }

    /* cleanup */
    free(idx);
}


/* inplace subtraction of two tensors (a-=b) */
void
tensor_decrease(tensor_view *a, tensor_view *b)
{
    sp_index_t *idx;
    int i;
    int nnz;

    /* allocate the index */
    idx = malloc(sizeof(sp_index_t) * a->nmodes);

    /* modify all the elements */
    nnz = TVNNZ(b);
    for(i=0; i<nnz; i++) {
	TVIDX(b, i, idx);
	TVSET(a, idx, TVGET(a,idx)-TVGETI(b,i));
    }

    /* cleanup */
    free(idx);
}


/* inplace scaling of a tensor by a scalar (t*=s) */
void
tensor_scale(tensor_view *t, double s)
{
    sp_index_t *idx;
    int i;
    int nnz;

    /* allocate the index */
    idx = malloc(sizeof(sp_index_t) * t->nmodes);

    /* modify all the elements */
    nnz = TVNNZ(t);
    for(i=0; i<nnz; i++) {
	TVIDX(t, i, idx);
	TVSET(t, idx, TVGET(t,idx)*s);
    }

    /* cleanup */
    free(idx);
}


/* compute the LP norm of the tensor */
double
tensor_lpnorm(tensor_view *t, double p)
{
    double result = 0.0;
    int nnz;
    int i;

    /* sum the absolute values raised to the p power */
    nnz = TVNNZ(t);
    for(i=0; i<nnz; i++) {
	result += pow(fabs(TVGETI(t, i)), p);
    }

    /* return the norm */
    return pow(result, 1.0/p);
}
