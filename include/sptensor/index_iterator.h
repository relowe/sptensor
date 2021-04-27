/*
    This header file contains the interface for a generic index-based
    iterator. This iterator configuration should be almost complete for
    most tensor types.
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
#ifndef INDEX_ITERATOR_H
#define INDEX_ITERATOR_H

#include <sptensor/sptensor.h>

/* Allocate an sptensor index iterator */
sptensor_iterator_t* sptensor_index_iterator_alloc(sptensor_t *t);

/* Free an sptensor index iterator */
void sptensor_index_iterator_free(sptensor_iterator_t *itr);

/* Advance the iterator */
int sptensor_index_iterator_next(sptensor_iterator_t *itr);

/* Go back one step */
int sptensor_index_iterator_prev(sptensor_iterator_t *itr);

/* Check that the iterator is valid */
int sptensor_index_iterator_valid(sptensor_iterator_t *itr);

/* Perform an index based get */
void sptensor_index_iterator_get(sptensor_iterator_t *itr, mpf_t v);

/* Perform an index based set */
void sptensor_index_iterator_set(sptensor_iterator_t *itr, mpf_t v);
#endif