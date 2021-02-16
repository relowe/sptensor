/*
    This is a collection of functions for performing basic tensor transformations.
    Copyright (C) 2020  Robert Lowe <pngwen@acm.org>

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
#ifndef SPTENSOR_TRANSFORM_H
#define SPTENSOR_TRANSFORM_H
#include <sptensor/sptensor.h>

/* 
 * Many transformations change the shape of the tensor. A tensor's
 * shape depnds on its dimension and number of modes. This structure
 * is used for communicating tensor shape.
 */
typedef struct sptensor_shape {
    int modes;
    sptensor_index_t *dim;
} sptensor_shape_t;


/* Allocate a tensor shape */
sptensor_shape_t* sptensor_shape_alloc(int modes);

/* Free the tensor shape */
void sptensor_shape_free(sptensor_shape_t *s);

/* Return a newly allocated shape of a tensor after transposing modes i and j. */
sptensor_shape_t* sptensor_transpose_shape(sptensor_t *t, unsigned int i, unsigned int j);

/* Transpose modes i and j of t and store the result in rop. */
void sptensor_transpose(sptensor_t *rop, sptensor_t *t, unsigned int i, unsigned int j);

/* Return a newly allocated shape of a tensor after it has been
 * unfolded along mode  n. 
 */
sptensor_shape_t* sptensor_unfold_shape(sptensor_t *t, unsigned int n);

/* Unfold tensor t along mode n and store the result in rop */
void sptensor_unfold(sptensor_t *rop, sptensor_t *t, unsigned int n);

/* Return a newly allocated shape of a tensor after it has been sliced
 * according to the sptensor_index_t* spec. The spec works as follows:
 *   0  - This mode is freely varying and will therefore run the range
 *        from the tensor.
 *   ow - This mode is fixed, and only entries matching this value
 *        will be present.
 * The resultant slice will only contain the free modes.
 */
sptensor_shape_t* sptensor_slice_shape(sptensor_t *t, sptensor_index_t *spec);

/* Slice tensor t according to the spec and store the result in rop.
 * The spec works as follows:
 *   0  - This mode is freely varying and will therefore run the range
 *        from the tensor.
 *   ow - This mode is fixed, and only entries matching this value
 *        will be present.
 * The resultant slice will only contain the free modes.
 */
void sptensor_slice(sptensor_t *rop, sptensor_t *t, sptensor_index_t *spec);

#endif
