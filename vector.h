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

typedef struct vector {
    void *ar;
    size_t element_size;
    unsigned int size;
    unsigned int capacity;
} vector;

#define VPTR(v, i) ((v)->ar + (i) * (v)->element_size)
#define VVAL(type, v, i) *((type*)(VPTR((v),(i))))

/*
 * Allocate a vector.
 * 
 * Parameters: element_size - The size of each vector element
 *             capacity     - The initial capacity of the vector
 * 
 * Returns: The newly allocated vector
 */
vector *vector_alloc(size_t element_size, int capacity);


/*
 * Free all the memory associated with a vector.
 * 
 * Parameters: v - The vector to free
 */
void vector_free(vector *v);


/* 
 * Grow the vector's capacity by a factor of two.
 * 
 * Parameters: v - The vector to grow
 */
void vector_grow(vector *v);


/*
 * Append an item to the back of the vector, growing if needed.
 * 
 * Paremters: v    - The vector to append to
 *            item - The item to copy into the vector
 */
void vector_push_back(vector *v, const void *item);

/*
 * Insert an item into a vector at index i.  Everythine 
 * after this position is pushed one slot further down.
 *
 * Paraemters: v  -   The vector to insert into
 *             i    - The index of the position to be inserted
 *             item - The item to copy into the vector
 */
void vector_insert(vector *v, unsigned int i, const void *item);


/*
 * Remove the element at index i from the vector, shifting everything
 * After i back one position.
 * 
 * Parameters: v - The vector to be modified
 *             i - The index of the item to be removed
 */
void vector_remove(vector *v, unsigned int i);


/*
 * Swap two elements in a vector.
 * Parameters: v - The vector to be modified
 *             i - The index of the first item
 *             j - The index of the second item
 */
void vector_swap(vector *v, unsigned int i, unsigned int j);

#endif
