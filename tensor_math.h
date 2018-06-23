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
#ifndef TENSOR_MATH_H
#define TENSOR_MATH_H
#include <view.h>

/* subtract two tensors, elementwise (returns a-b) */
tensor_view *tensor_sub(tensor_view *a, tensor_view *b);

/* add two tensors. elmentwise (returns a+b) */
tensor_view *tensor_add(tensor_view *a, tensor_view *b);

/* multiply the tensor by a scalar (returns t*s) */
tensor_view *scalar_mul(tensor_view *t, double s);

/* divide the tensor by a scalar (returns t/s) */
tensor_view *scalar_div(tensor_view *t, double s);

/* inplace addition of two tensors (a-=b) */
void tensor_increase(tensor_view *a, tensor_view *b);

/* inplace subtraction of two tensors (a+=b) */
void tensor_decrease(tensor_view *a, tensor_view *b);

/* inplace scaling of a tensor by a scalar (t*=s) */
void tensor_scale(tensor_view *t, double s);

#endif
