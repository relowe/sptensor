#ifndef SPTENSOR_H
#define SPTENSOR_H

#define SPTENSOR_DEFAULT_CAPACITY 128
#define SPI(tns, i) (tns->idx + i * tns->nmodes)

typedef unsigned int sp_index_t;

typedef struct sptensor
{
    double *ar;      /* the tensor values */
    sp_index_t *idx; /* the index list (maintained in sorted order) */
    sp_index_t *dim; /* The dimension of each tensor mode */
    int nmodes;      /* The number of tensor modes */
    int nnz;         /* The number of non-negative values */
    int capacity;    /* Total capacity of the tensor */
} sptensor;


/*
 * Allocate a sparse tensor.  Sparse tensors are allocated using
 * malloc, and have a SPTENSOR_DEFAULT_CAPACITY capacity.
 * 
 * Parameters: nmodes - The number of modes
 *             dim    - The dimension of the tensor
 *
 * Return: A pointer to the newly created tensor.
 */ 
sptensor* sptensor_alloc(int nmodes, const sp_index_t *dim);


/*
 * Free the memory allocated for a sparse tensor.
 */
void sptensor_free(sptensor *tns);


/*
 * Get a value from a sparse tensor.
 *
 * Parameters: tns - The sparse tensor to retrieve from
 *             idx - The index of the item to retrieve
 */
double sptensor_get(sptensor *tns, const sp_index_t *idx);


/* 
 * Set a value in the sparse tensor.
 * 
 * Parameters: tns - The sparse tensor to write to
 *             idx - The index of the item to retrieve
 *             val - The value to write to the tensor
 */
void sptensor_set(sptensor *tns, const sp_index_t *idx, double val);


/* 
 * Compare two indexes for a given tensor.  Comparison is 
 * performed from left to right.  Pretty much exactly as 
 * strcmp, except with ints.
 * 
 * Parameter: tns - The sparse (used to determine nmodes)
 *            a   - The left hand index being compared
 *            b   - The right hand index being compared
 *  
 * Return:  < 0 if a < b
 *            0 if a == b
 *          > 0 if a > b
 */
int sptensor_indexcmp(sptensor *tns, const sp_index_t *a, const sp_index_t *b);



/*
 * Find the index into ar of the tensor struct.
 * 
 * Parameter: tns - The sparse tensor to search
 *            idx - The index to find
 *
 * Return: The index into ar of element idx.  If the index is
 *         not of a non-zero value, return -1.
 */
int sptensor_find_index(sptensor *tns, const sp_index_t *idx);

#endif
