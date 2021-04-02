#include <stdio.h>
#include <gmp.h>
#include <sptensor/sptensor.h>
#include <sptensor/hash.h>
#include <sptensor/index_iterator.h>
#include<time.h>

int main()
{
    sptensor_index_t modes[] = {40000, 100000, 2000000};
    /*sptensor_index_t idx[][3]  = { {1, 1, 1},
                                   {2, 3, 1},
                                   {2, 4, 4},
                                   {3, 2, 2}};
    double v [4] = {99.0, 44.0, 100.0, 1.0};*/
	
    sptensor_hash_t *t;
    sptensor_iterator_t *itr;
    mpf_t mpfv;
	/*int i=0;
    int n=4;*/
	
	mpf_init(mpfv);
	mpf_set_ui(mpfv, 2.0);

    /* build the tensor */
    t = sptensor_hash_alloc(modes, 3);
    itr = sptensor_index_iterator_alloc((sptensor_t*)t);
	
	 // Use current time as seed for random generator
    srand(time(0));
	int i;
	int j;
	
	sptensor_index_t idx[40000][3];
	for(i=0;i<40000;i++) {
		for(j=0;j<3;j++) {
			idx[i][j] = rand();
			/*printf("%d, ",idx[i][j]);*/
		}
		sptensor_hash_set(t, idx[i] , mpfv);
		/*printf("\n");*/
	}
	
	/*while(sptensor_iterator_valid(itr)) {
		sptensor_hash_set(t, itr->index , mpfv);
		sptensor_index_iterator_next(itr);
	}*/
	
	gmp_printf("num of collisions = %Zd\n", t->num_collisions);
	gmp_printf("cumulative time spent probing = %Ff\n", t->probe_time);
	
	
	/*sptensor_hash_set(t, idx[1],mpfv);

	sptensor_hash_remove(t, idx[1], mpfv);
	sptensor_hash_search(t->hashtable, idx[1],t->nbuckets, t->modes);
	/*
    /*mpf_init(mpfv);
    for(i=0; i<4; i++) {
        mpf_set_d(mpfv, v[i]);
        sptensor_hash_set(t, idx[i], mpfv);
    }

	sptensor_hash_search(t, idx[0]);
		
    /* deallocate the tensor */
    sptensor_hash_free(t);
	
}