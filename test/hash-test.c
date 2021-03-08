#include <stdio.h>
#include <gmp.h>
#include <sptensor/sptensor.h>
#include <sptensor/hash.h>


int main()
{
    sptensor_index_t modes[] = {3, 4, 5};
    sptensor_index_t idx[][3]  = { {1, 1, 1},
                                   {2, 3, 1},
                                   {2, 4, 4},
                                   {3, 2, 2}};
    double v [4] = {99.0, 44.0, 100.0, 1.0};
    sptensor_hash_t *t;
    sptensor_iterator_t *itr;
    int i=0;
    int n=4;
    mpf_t mpfv;
	mpz_t r;
	mpz_t r2;
	
	mpz_init(r);
	mpz_init(r2);
	mpf_init(mpfv);
	mpf_set_ui(mpfv, 2.0);

	printf("building tensor...\n");
    /* build the tensor */
    t = sptensor_hash_alloc(modes, 3);
	printf("finished building tensor.\n");
	/*sptensor_hash_search(t, idx[0],r);*/
	sptensor_hash_set(t, idx[0],r,mpfv);
    sptensor_hash_set(t, idx[0],r2,mpfv);
	sptensor_hash_set(t, idx[1],r,mpfv);
	
    /*mpf_init(mpfv);
    for(i=0; i<4; i++) {
        mpf_set_d(mpfv, v[i]);
        sptensor_hash_set(t, idx[i], mpfv);
    }

	sptensor_hash_search(t, idx[0]);
		
    /* deallocate the tensor */
    sptensor_hash_free(t);
}
