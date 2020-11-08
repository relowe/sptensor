#include <stdio.h>
#include <gmp.h>
#include <sptensor/sptensor.h>
#include <sptensor/coo.h>

void itr_dump(sptensor_iterator_t *itr) 
{
    int i;
    mpf_t v;

    mpf_init(v);
    while(sptensor_iterator_valid(itr)) {
        for(i=0; i<itr->t->modes; i++) {
            printf("%d%s", 
                    itr->index[i], 
                    i<itr->t->modes-1 ? ", " : " : ");
        }
        sptensor_get(itr->t, itr->index, v);
        gmp_printf("%Ff\n", v);
        sptensor_iterator_next(itr);
    }
    mpf_clear(v);
}



int main()
{
    sptensor_index_t modes[] = {3, 4, 5};
    sptensor_index_t idx[][3]  = { {1, 1, 1},
                                   {2, 3, 1},
                                   {2, 4, 4},
                                   {3, 2, 2}};
    double v [4] = {99.0, 44.0, 100.0, 1.0};
    sptensor_t *t;
    sptensor_iterator_t *itr;
    int i=0;
    int n=4;
    mpf_t mpfv;

    /* build the tensor */
    t = sptensor_coo_alloc(modes, 3);
    mpf_init(mpfv);
    for(i=0; i<4; i++) {
        mpf_set_d(mpfv, v[i]);
        sptensor_set(t, idx[i], mpfv);
    }

    /* display the non-zero tensor */
    printf("non-zero entries\n");
    itr = sptensor_nz_iterator(t);
    itr_dump(itr);
    sptensor_iterator_free(itr);
    printf("\n");

    /* replace an item with non-zero */
    printf("Replacing some elements\n");
    mpf_set_d(mpfv, 42);
    sptensor_set(t, idx[1], mpfv);
    mpf_set_d(mpfv, 0.0);
    sptensor_set(t, idx[2], mpfv);

    /* display the non-zero tensor iterator again */
    printf("New non-zero entries\n");
    itr=sptensor_nz_iterator(t);
    itr_dump(itr);
    sptensor_iterator_free(itr);
    printf("\n");

    /* do the full iterator */
    printf("Full Iterator\n");
    itr=sptensor_iterator(t);
    itr_dump(itr);
    sptensor_iterator_free(itr);

    /* deallocate the tensor */
    sptensor_free(t);
}
