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


#include <stdlib.h>
#include <string.h>
#include <sptensor/vector.h>

/*
 * Allocate a sptensor_vector.
 * 
 * Parameters: element_size - The size of each sptensor_vector element
 *             capacity     - The initial capacity of the sptensor_vector
 * 
 * Returns: The newly allocated sptensor_vector
 */
sptensor_vector *
sptensor_vector_alloc(size_t element_size, int capacity) 
{
    sptensor_vector *result;

    result = malloc(sizeof(sptensor_vector));
    result->size=0;
    result->capacity=capacity;
    result->element_size = element_size;
    result->ar = malloc(result->element_size * result->capacity);

    return result;
}


/*
 * Copy a sptensor_vector 
 *
 * Parameters: v - The sptensor_vector to copy
 *
 * Returns a copy of the sptensor_vector, perfectly sized to hold 
 * its contents.
 */
sptensor_vector *sptensor_vector_copy(sptensor_vector *v)
{
    sptensor_vector *result;

    /* allocate the new sptensor_vector */
    result = sptensor_vector_alloc(v->element_size, v->size);

    /* copy the sptensor_vector contents */
    result->size = v->size;
    memcpy(result->ar, v->ar, v->element_size * v->size);

    return result;
}


/*
 * Free all the memory associated with a sptensor_vector.
 * 
 * Parameters: v - The sptensor_vector to free
 */
void
sptensor_vector_free(sptensor_vector *v)
{
    free(v->ar);
    free(v);
}


/* 
 * Grow the sptensor_vector's capacity by a factor of two.
 * 
 * Parameters: v - The sptensor_vector to grow
 */
void
sptensor_vector_grow(sptensor_vector *v)
{
    void *ar;

    /* allocate a new sptensor_vector */
    v->capacity *= 2;
    ar = malloc(v->capacity * v->element_size);
  
    /* copy the original sptensor_vector's contents */
    memcpy(ar, v->ar, v->size * v->element_size);

    /* replace the original array */
    free(v->ar);
    v->ar = ar;
}


/*
 * Append an item to the back of the sptensor_vector, growing if needed.
 * 
 * Paremters: v    - The sptensor_vector to append to
 *            item - The item to copy into the sptensor_vector
 */
void
sptensor_vector_push_back(sptensor_vector *v, const void *item)
{
    /* grow if needed */
    if(v->size == v->capacity) {
	sptensor_vector_grow(v);
    }

    /* insert the item */
    memcpy(v->ar + v->size * v->element_size, item, v->element_size);
    v->size++;
}


/*
 * Insert an item into a sptensor_vector at index i.  Everythine 
 * after this position is pushed one slot further down.
 *
 * Paraemters: v    - The sptensor_vector to insert into
 *             i    - The index of the position to be inserted
 *             item - The item to copy into the sptensor_vector
 */
void
sptensor_vector_insert(sptensor_vector *v, unsigned int i, const void *item)
{
    /* grow if needed */
    if(v->size == v->capacity) {
	sptensor_vector_grow(v);
    }


    /* shift everything down */
    memmove(VPTR(v, i+1), VPTR(v, i), (v->size - i) * v->element_size);

    /* add the element at the specified position */
    memcpy(VPTR(v, i), item, v->element_size);
    v->size++;
}


/*
 * Remove the element at index i from the sptensor_vector, shifting everything
 * After i back one position.
 * 
 * Parameters: v - The sptensor_vector to be modified
 *             i - The index of the item to be removed
 */
void
sptensor_vector_remove(sptensor_vector *v, unsigned int i)
{
    /* shift everything back one position */
    memcpy(VPTR(v, i), VPTR(v, i+1), (v->size - i - 1) * v->element_size);
    v->size--;
}



/*
 * Swap two elements in a sptensor_vector.
 * Parameters: v - The sptensor_vector to be modified
 *             i - The index of the first item
 *             j - The index of the second item
 */
void
sptensor_vector_swap(sptensor_vector *v, unsigned int i, unsigned int j)
{
    unsigned char *p1, *p2;  /* the two values to swap */
    int count;

    /* get the values to swap */
    p1 = VPTR(v, i);
    p2 = VPTR(v, j);

    /* do iterative xor swap */
    /* We are told this is ugly, but it's ok, we're pros! */
    /*   Ok, it's hideous but saves a malloc! */
    for(count=0; count < v->element_size; count++, p1++, p2++) {
	*p1 = *p1 ^ *p2;
	*p2 = *p1 ^ *p2; 
	*p1 = *p1 ^ *p2;
    }
}
