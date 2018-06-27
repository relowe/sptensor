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
tensor_view **ccd_identity(tensor_view *a, unsigned int n, double lambda[],
			   tensor_view *un, int max_iter, double tol);

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
tensor_view **ccd_core(tensor_view *a, tensor_view *c, double lambda[],
		       tensor_view *un, int max_iter, double tol);

/*
 * Free's the ccd factor list.
 * Parameters;
 *     u - A factor list returned by a ccd composition
 */
void ccd_free(tensor_view **u);

#endif
