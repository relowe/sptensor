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
	
	st = sptensor_skbt_alloc(modes, 3);

	sptensor_skbt_make_tree(st, t, 3, modes, 4);
	
    /* display the non-zero tensor */
    printf("non-zero entries\n");
    itr = sptensor_skbt_nz_iterator(t);
    itr_dump(itr);
    sptensor_iterator_free(itr);
    printf("\n");


    /* deallocate the tensor */
    sptensor_free(t);
}
