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
#ifndef MULTIPLY_H
#define MULTIPLY_H
#include <sptensor/view.h>

/* Matrix mulitplication between two tensor views, resulting in a
   sparse tensor view containing a newly allocated sparse tensor. */
tensor_view *matrix_product(tensor_view *a, tensor_view *b);

/* N-Mode multiplication of tensor a by matrix u */
tensor_view *nmode_product(unsigned int n, tensor_view *a, tensor_view *u);

/* Outer (tensor) product of two tensor views */
tensor_view *tensor_product(tensor_view *a, tensor_view *b);
#endif
