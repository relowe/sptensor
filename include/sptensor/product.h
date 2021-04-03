/*
    This is a collection of functions for various tensor products.
    Copyright (C) 2021 Robert Lowe <rlowe@semo.edu>

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
#ifndef SPTENSOR_PRODUCT_H
#define SPTENSOR_PRODUCT_H
#include <sptensor/sptensor.h>
#include <sptensor/transform.h>



/* Get the shape of the matrix product */
sptensor_shape_t *sptensor_matrix_product_shape(sptensor_t *op1, sptensor_t *op2);

/* Compute the matrix product rop = op1 x op2 */
void sptensor_matrix_product(sptensor_t *rop, sptensor_t *op1, sptensor_t *op2);

/* Get the shape of the nmode product */
sptensor_shape_t *sptensor_nmode_product_shape(sptensor_index_t n, sptensor_t *op1, sptensor_t *op2);

/* Compute the nmode prouct of tensor op1 by matrix op2.
    rop = op1 x_n op2
 */
void sptensor_nmode_product(sptensor_index_t n, sptensor_t *rop, sptensor_t *op1, sptensor_t *op2);

/* Compute the shape of the outer tensor product */
sptensor_shape_t *sptensor_outer_product_shape(sptensor_t *op1, sptensor_t *op2);

/* Compute the outer tensor product */
void sptensor_outer_product(sptensor_t *rop, sptensor_t *op1, sptensor_t *op2);

/* Shape of the Hadamard product */
sptensor_shape_t *sptensor_hadamard_product_shape(sptensor_t *op1, sptensor_t *op2);

/* Compute the Hadamard product */
void sptensor_hadamard_product(sptensor_t *rop, sptensor_t *op1, sptensor_t *op2);
#endif