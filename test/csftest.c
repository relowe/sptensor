#include <stdio.h>
#include <gmp.h>
#include <sptensor/sptensor.h>
#include <sptensor/csf.h>
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


void test1(){
sptensor_index_t modes[] = {3, 4, 5};
    sptensor_index_t idx[][3]  = { {1, 1, 1},
                                   {2, 3, 1},
                                   {2, 4, 4},
                                   {3, 2, 2}};
    double v [4] = {99.0, 44.0, 100.0, 1.0};
    sptensor_t *t;
    sptensor_t *t2;
    sptensor_iterator_t *itr;
    int i=0;
    int n=4;
    mpf_t mpfv;

    /* build the coo tensor */
    t = sptensor_coo_alloc(modes, 3);
    mpf_init(mpfv);
    for(i=0; i<4; i++) {
        mpf_set_d(mpfv, v[i]);
        sptensor_set(t, idx[i], mpfv);
    }

    /* build the csf from coo */
    t2 = (sptensor_t*)sptensor_csf_from_coo((sptensor_coo_t*)t);

    /* display structure of csf for debugging */
    sptensor_csf_print(t2);

    
    /* display the non-zero tensor */
    //printf("non-zero entries\n");
    //itr = sptensor_nz_iterator(t2);
    // itr_dump(itr);
    // sptensor_iterator_free(itr);
    // printf("\n");

    /* do the full iterator */
    // printf("Full Iterator\n");
    // itr=sptensor_iterator(t2);
    // itr_dump(itr);
    // sptensor_iterator_free(itr);
    

    /* deallocate the tensor */
    sptensor_free(t);
    sptensor_free(t2);
}

void test2(){
    sptensor_index_t modes[] = {3, 4, 5, 4};
    sptensor_index_t idx[][4]  = { {1, 1, 1, 2},
                                   {1, 1, 1, 3},
                                   {1, 2, 1, 1},
                                   {1, 2, 1, 3},
                                   {1, 2, 2, 1},
                                   {2, 2, 2, 1},
                                   {2, 2, 2, 2},
                                   {2, 2, 2, 3}};
    double v [8] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    sptensor_t *t;
    sptensor_t *t2;
    sptensor_iterator_t *itr;
    int i=0;
    int n=8;
    mpf_t mpfv;

    /* build the coo tensor */
    t = sptensor_coo_alloc(modes, 4);
    mpf_init(mpfv);
    for(i=0; i<8; i++) {
        mpf_set_d(mpfv, v[i]);
        sptensor_set(t, idx[i], mpfv);
    }

    /* build the csf from coo */
    t2 = (sptensor_t*)sptensor_csf_from_coo((sptensor_coo_t*)t);

    /* display structure of csf for debugging */
    sptensor_csf_print(t2);

    
    /* display the non-zero tensor */
    //printf("non-zero entries\n");
    //itr = sptensor_nz_iterator(t2);
    // itr_dump(itr);
    // sptensor_iterator_free(itr);
    // printf("\n");

    /* do the full iterator */
    // printf("Full Iterator\n");
    // itr=sptensor_iterator(t2);
    // itr_dump(itr);
    // sptensor_iterator_free(itr);
    

    /* deallocate the tensor */
    sptensor_free(t);
    sptensor_free(t2);
}

void main()
{
    /*test1();*/
    test2();
}
