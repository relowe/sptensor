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

/* 
 * The algorithm used in these files is described in the following
 * paper:
 * @article{liu2012sparse,
 *    title={Sparse non-negative tensor factorization using columnwise coordinate descent},
 *    author={Liu, Ji and Liu, Jun and Wonka, Peter and Ye, Jieping},
 *    journal={Pattern Recognition},
 *    volume={45},
 *    number={1},
 *    pages={649--656},
 *    year={2012},
 *    publisher={Elsevier}
 * }
 */
#ifndef CCD_H
#define CCD_H
#include <view.h>

typedef struct ccd_result {
    tensor_view *core;  /* core tensor */
    tensor_view **u;    /* factor matrices */
    int n;              /* number of factor matrices */
    unsigned int iter;  /* number of iterations ran */
    double final_error; /* how much the final iteration changed */
    double fit;         /* ||(A-A*)||/||A|| */
} ccd_result;

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
ccd_result *ccd_identity(tensor_view *a, unsigned int n, double lambda[],
			 int max_iter, double tol);

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
ccd_result *ccd_core(tensor_view *a, tensor_view *c, double lambda[],
		     int max_iter, double tol);


/* 
 * Constructs the tensor described by the ccd_result structure 
 */
tensor_view *ccd_construct(ccd_result *result);


/*
 * Free's the ccd result
 * Parameters;
 *     result - The result to free
 */
void ccd_free(ccd_result *result);

#endif
