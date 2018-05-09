#ifndef VECTOR_H
#define VECTOR_H

typedef struct vector {
    void *ar;
    size_t element_size;
    unsigned int size;
    unsigned int capacity;
} vector;

#define VPTR(v, i) ((v)->ar + (i) * (v)->element_size)


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
void vector_push_back(vector *v, void *item);

/*
 * Insert an item into a vector at index i.  Everythine 
 * after this position is pushed one slot further down.
 *
 * Paraemters: v  -   The vector to insert into
 *             i    - The index of the position to be inserted
 *             item - The item to copy into the vector
 */
void vector_insert(vector *v, unsigned int i, void *item);


/*
 * Remove the element at index i from the vector, shifting everything
 * After i back one position.
 * 
 * Parameters: v - The vector to be modified
 *             i - The index of the item to be removed
 */
void vector_remove(vector *v, unsigned int i);


#endif
