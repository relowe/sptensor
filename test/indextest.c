/*
 * This is just a simple test of the sptensor index functions.
 */
#include <stdlib.h>
#include <stdio.h>
#include <sptensor/index.h>

void print_index(sptensor_index_t *idx, int nmodes);

int main()
{
    sptensor_index_t *dim;
    sptensor_index_t *idx;
    sptensor_index_t *lbound;
    int nmodes=4;

    /* Let's allocate a four mode dimension and initialize it */
    nmodes = 4;
    dim = (sptensor_index_t*) malloc(sizeof(sptensor_index_t) * nmodes);
    dim[0] = 2;
    dim[1] = 3;
    dim[2] = 2;
    dim[3] = 3;

    /* now we will do the index */
    idx = (sptensor_index_t*) malloc(sizeof(sptensor_index_t) * nmodes);

    /* and the lower bound */
    lbound = (sptensor_index_t*) malloc(sizeof(sptensor_index_t) * nmodes);

    /* test incrementing */
    printf("INCREMENT TEST\n");
    idx[0] = 1;
    idx[1] = 1;
    idx[2] = 1;
    idx[3] = 1;
    while(sptensor_index_cmp(nmodes, idx, dim) < 0) {
        print_index(idx, nmodes);
        sptensor_index_inc(nmodes, dim, idx);
    }
    print_index(idx, nmodes);
    printf("==========================================\n\n");

    /* test decrementing */
    printf("DECREMENT TEST\n");
    lbound[0] = lbound[1] = lbound[2] = lbound[3] = 1;
    while(sptensor_index_cmp(nmodes, idx, lbound) >= 0) {
        print_index(idx, nmodes);
        sptensor_index_dec(nmodes, dim, idx);
    }
    printf("==========================================\n\n");
}


void print_index(sptensor_index_t *idx, int nmodes)
{
    int i;

    for(i=0; i<nmodes; i++) {
        printf("%u\t", idx[i]);
    }
    printf("\n");
}