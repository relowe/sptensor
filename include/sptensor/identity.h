/*
    This is the header file for identity tensors in sptensor.
    The sptensor library provides a set of functions and data type for
    dealing with sparse tensors.

    Copyright (C) 2021  Robert Lowe <pngwen@acm.org>

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
#include <sptensor/sptensor.h>
#ifndef SPTENSOR_IDENTITY_H
#define SPTENSOR_IDENTITY_H

/* An identity tensor is just a tensor with 1s along its super diagonal,
   thus the default sptensor_t will suffice to store them. */
typedef sptensor_t sptensor_identity_t;

/* allocation functions */
sptensor_t *sptensor_identity_alloc(sptensor_index_t *modes, int nmodes);
void sptensor_identity_free(sptensor_identity_t *t);

/* get function (read only) */
void sptensor_identity_get(sptensor_identity_t *t, sptensor_index_t *idx, mpf_t v);

/* create an nz iterator */
sptensor_iterator_t *sptensor_identity_nz_iterator(sptensor_identity_t *t);
#endif