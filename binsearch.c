/*
    This is an implementation of binary search which operates on vectors.
    It will either find the location of the item or it will find where it
    should be.
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
#include <binsearch.h>

/*
 * Performs a binary search O(lg n) on the vector.  If the item is 
 * found, its index is returned.  If the item is not found, -x is returned
 * where |x+1| is the index where the item should be inserted.
 *   v    - The vector to search
 *   item - Pointer to the item to find 
 *   cmp  - The comparison function to use on the vector
 */
int
vector_binsearch(vector *v, void *item, binsearch_cmp_func cmp_f)
{
    int left = 0;
    int right = v->size - 1;
    int mid;
    int cmp;

    /* do a binary search on the list of indexes */
    while(left <= right) {
	mid = (right+left) / 2;
	cmp = (*cmp_f)(v->element_size, item, VPTR(v, mid));
	if(cmp == 0) {
	    return mid;
	} else if(cmp < 0) {
	    right = mid-1;
	} else {
	    left = mid+1;
	}
    }

    /* return the new position */
    return -left-1;
}


/*
 * Insert an item into a vector, maintaining it in sorted order by using
 * binary search to find its position.  If the item is already in the 
 * vector, it is not inserted. The position of the item within the 
 * vector is returned.
 *   v    - The vector to insert into
 *   item - Pointer to the item to insert
 *   cmp  - The comparison function to use on the vector
 */
int
vector_set_insert(vector *v, void *item, binsearch_cmp_func cmp)
{
    int i;

    /* find the position we are meant to be at */
    i = vector_binsearch(v, item, cmp);

    /* if it is already there, we are done! */
    if(i >= 0) {
	return i;
    }

    /* calculate the index, insert, and then return */
    i = -(i + 1);
    vector_insert(v, i, item);
    return i;
}


/*
 * Insert an item into a vector, maintaining it in sorted order by using
 * binary search to find its position. The position the item is inserted 
 * at is returned.
 *   v    - The vector to insert into
 *   item - Pointer to the item to insert
 *   cmp  - The comparison function to use on the vector
 */
int
vector_sorted_insert(vector *v, void *item, binsearch_cmp_func cmp)
{
    int i;

    /* find the position we are meant to be at */
    i = vector_binsearch(v, item, cmp);

    /* convert the index for a new insertion */
    if(i < 0) {
	i = -(i + 1);
    }

    /* calculate the index, insert, and then return */
    vector_insert(v, i, item);
    return i;
}
