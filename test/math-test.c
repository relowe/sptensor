#include <stdio.h>
#include <gmp.h>
#include <sptensor/sptensor.h>
#include <sptensor/coo.h>
#include <sptensor/math.h>

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
    sptensor_index_t idx1[][3]  = { {1, 1, 1},
                                    {2, 3, 1},
                                    {2, 4, 4},
                                    {3, 2, 2}};
    sptensor_index_t idx2[][3]  = { {1, 1, 1},
                                    {2, 3, 3},
                                    {2, 2, 4},
                                    {3, 2, 2}};
    double v1 [4] = {1, 2, 3, 4};
    double v2 [4] = {5, 6, 7, 8};

    sptensor_t *r, *t1, *t2;
    sptensor_iterator_t *itr;
    int i=0;
    int n=4;
    mpf_t mpfv;

    /* build the tensor */
    t1 = sptensor_coo_alloc(modes, 3);
    t2 = sptensor_coo_alloc(modes, 3);
    mpf_init(mpfv);
    for(i=0; i<4; i++) {
        mpf_set_d(mpfv, v1[i]);
        sptensor_set(t1, idx1[i], mpfv);
        mpf_set_d(mpfv, v2[i]);
        sptensor_set(t2, idx2[i], mpfv);
    }

    /* test addition */
    printf("Addition Result\n");
    printf("===============\n");
    r = sptensor_coo_alloc(modes, 3);
    sptensor_add(r, t1, t2);
    itr = sptensor_nz_iterator(r);
    itr_dump(itr);
    sptensor_iterator_free(itr);
    sptensor_free(r);
    printf("\n\n");

    /* test subtraction */
    printf("Subtraction Result\n");
    printf("==================\n");
    r = sptensor_coo_alloc(modes, 3);
    sptensor_sub(r, t1, t2);
    itr = sptensor_nz_iterator(r);
    itr_dump(itr);
    sptensor_iterator_free(itr);
    sptensor_free(r);
    printf("\n\n");

    /* test scalar multiplication */
    printf("Multiply by 5 Result\n");
    printf("====================\n");
    mpf_set_d(mpfv, 5.0);
    r = sptensor_coo_alloc(modes, 3);
    sptensor_smul(r, t1, mpfv);
    itr = sptensor_nz_iterator(r);
    itr_dump(itr);
    sptensor_iterator_free(itr);
    sptensor_free(r);
    printf("\n\n");

    /* test scalar multiplication */
    printf("Divide by 5 Result\n");
    printf("==================\n");
    mpf_set_d(mpfv, 5.0);
    r = sptensor_coo_alloc(modes, 3);
    sptensor_sdiv(r, t1, mpfv);
    itr = sptensor_nz_iterator(r);
    itr_dump(itr);
    sptensor_iterator_free(itr);
    sptensor_free(r);
    printf("\n\n");
    

    /* deallocate the tensor */
    sptensor_free(t1);
    sptensor_free(t2);
}
