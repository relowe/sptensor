/*
    This is a set of functions which implements a simple resizable vector.
    (Why should the C++ STL have all the fun? #void_is_life)
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

#ifndef VECTOR_H
#define VECTOR_H
#include <stdlib.h>

#define SPTENSOR_VECTOR_DEFAULT_CAPACITY 128

typedef struct sptensor_vector {
    void *ar;
    size_t element_size;
    unsigned int size;
    unsigned int capacity;
} sptensor_vector;

#define VPTR(v, i) ((v)->ar + (i) * (v)->element_size)
#define VVAL(type, v, i) *((type*)(VPTR((v),(i))))

/*
 * Allocate a sptensor_vector.
 * 
 * Parameters: element_size - The size of each sptensor_vector element
 *             capacity     - The initial capacity of the sptensor_vector
 * 
 * Returns: The newly allocated sptensor_vector
 */
sptensor_vector *sptensor_vector_alloc(size_t element_size, int capacity);


/*
 * Copy a sptensor_vector 
 *
 * Parameters: v - The sptensor_vector to copy
 *
 * Returns a copy of the sptensor_vector, perfectly sized to hold 
 * its contents.
 */
sptensor_vector *sptensor_vector_copy(sptensor_vector *v);


/*
 * Free all the memory associated with a sptensor_vector.
 * 
 * Parameters: v - The sptensor_vector to free
 */
void sptensor_vector_free(sptensor_vector *v);


/* 
 * Grow the sptensor_vector's capacity by a factor of two.
 * 
 * Parameters: v - The sptensor_vector to grow
 */
void sptensor_vector_grow(sptensor_vector *v);


/*
 * Append an item to the back of the sptensor_vector, growing if needed.
 * 
 * Paremters: v    - The sptensor_vector to append to
 *            item - The item to copy into the sptensor_vector
 */
void sptensor_vector_push_back(sptensor_vector *v, const void *item);

/*
 * Insert an item into a sptensor_vector at index i.  Everythine 
 * after this position is pushed one slot further down.
 *
 * Paraemters: v  -   The sptensor_vector to insert into
 *             i    - The index of the position to be inserted
 *             item - The item to copy into the sptensor_vector
 */
void sptensor_vector_insert(sptensor_vector *v, unsigned int i, const void *item);


/*
 * Remove the element at index i from the sptensor_vector, shifting everything
 * After i back one position.
 * 
 * Parameters: v - The sptensor_vector to be modified
 *             i - The index of the item to be removed
 */
void sptensor_vector_remove(sptensor_vector *v, unsigned int i);


/*
 * Swap two elements in a sptensor_vector.
 * Parameters: v - The sptensor_vector to be modified
 *             i - The index of the first item
 *             j - The index of the second item
 */
void sptensor_vector_swap(sptensor_vector *v, unsigned int i, unsigned int j);

#endif
