#include <stdio.h>
#include <gmp.h>
#include <sptensor/sptensor.h>
#include <sptensor/coo.h>
#include <sptensor/inzt.h>


int main()
{
    sptensor_index_t modes[] = {300, 400, 500};
    sptensor_index_t idx[][3]  = { {1, 1, 1},
                                   {2, 3, 1},
                                   {2, 4, 4},
                                   {3, 2, 2},
                                   {20, 21, 14},
                                   {23, 23, 17} 
                                 };
    sptensor_index_t tests[][2][3] = { 
            { {200, 200, 200}, {300, 300, 300} },
            { {2, 1, 1}, {2, 1, 4} }, 
            { {2, 2, 3}, {3, 4, 4} },
            { {2, 3, 1}, {300, 400, 500} },
            { {1, 1, 1}, {300, 400, 500} } 
        };
    double v [] = {99.0, 44.0, 100.0, 1.0, 17.0, 42.0};
    sptensor_t *t;
    sptensor_iterator_t *itr;
    sptensor_inzt_t *inzt;
    int i=0;
    int n=4;
    int nnz = sizeof(idx) / sizeof(idx[0]);
    int ntests = sizeof(tests) / sizeof(tests[0]);
    mpf_t mpfv;

    /* build the tensor */
    t = sptensor_coo_alloc(modes, 3);
    mpf_init(mpfv);
    for(i=0; i<nnz; i++) {
        mpf_set_d(mpfv, v[i]);
        sptensor_set(t, idx[i], mpfv);
    }

    /* build the inzt object */
    itr = sptensor_nz_iterator(t);
    inzt = sptensor_inzt_create(itr);
    sptensor_iterator_free(itr);

    /* run the tests */
    for(i=0; i<ntests; i++) {
        printf("min = (%u, %u, %u) max = (%u, %u, %u) inzt= %u\n",
                tests[i][0][0], tests[i][0][1], tests[i][0][2],
                tests[i][1][0], tests[i][1][1], tests[i][1][2],
                sptensor_inzt(inzt, tests[i][0], tests[i][1]));
    }

    /* deallocate the tensor and other objects */
    sptensor_inzt_free(inzt);
    sptensor_free(t);
}
