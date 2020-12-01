#include <stdio.h>
#include <sptensor/sptensor.h>
#include <sptensor/inzt.h>
#include <sptensor/coo.h>


int main()
{
    sptensor_t *t;  /* A test tensor */
    sptensor_iterator_t *itr; /* an iterator */
    sptensor_inzt_t *inzt; /* inzt object to test the morton coding */
    sptensor_inzt_index_t *inzt_idx;
    sptensor_index_t dim[2] = {4, 4};
    mpf_t one;
    int i;

    mpf_init(one);
    mpf_set_ui(one, 1);

    /* create a 4x4 2-tensor and fill it with ones */
    t = sptensor_coo_alloc(dim, 2);
    itr = sptensor_iterator(t);
    while(sptensor_iterator_valid(itr)) {
        sptensor_set(t, itr->index, one);
        sptensor_iterator_next(itr);
    }
    sptensor_iterator_free(itr);


    /* create the inzt object */
    itr = sptensor_nz_iterator(t);
    inzt = sptensor_inzt_create(itr);
    sptensor_iterator_free(itr);

    /* display the list of objects */
    inzt_idx = inzt->inzt_index->ar;
    printf("Row\tCol\tMorton\t\n");
    for(i=0; i<inzt->inzt_index->size; i++) {
        gmp_printf("%u\t%u\t%02ZX\n", 
                   inzt_idx[i].index[0], 
                   inzt_idx[i].index[1],
                   inzt_idx[i].morton);
    }

    /* free the remaining objects */
    sptensor_inzt_free(inzt);
    sptensor_free(t);
}
