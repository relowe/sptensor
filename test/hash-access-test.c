#include <stdio.h>
#include <sptensor/sptensor.h>
#include <sptensor/hash.h>
#include <time.h>

void access_test(sptensor_hash_t *t, int num_elements) {
	int i;
	int j;
	
	clock_t start, end;
   	double cpu_time_used;
	
	mpf_t v;
	mpf_init_set_ui(v, 2.0);
	
	sptensor_index_t idx[3];
	
	start = clock();
	
	for(i=0; i < num_elements;i++) {
		for(j=0;j<3;j++) {
			idx[j] = rand();
		}
		sptensor_get_search(t, idx, v);
	}
	
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("time used to access %d elements = %f\n", num_elements, cpu_time_used);
}

int main(int argc, char **argv) 
{
    FILE *file;
    sptensor_hash_t *t;
	
    /* evaluate the command line */
    if(argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return -1;
    }

    /* attempt to open the file */
    file = fopen(argv[1], "r");
    if(!file){
        fprintf(stderr, "Could not open file: %s\n", argv[1]);
        return -2;
    }

    /* read the tensor, and then write it to stdout */
    t = sptensor_hash_read(file);
	gmp_printf("number of collisions = %Zd\n", t->num_collisions);
	gmp_printf("time spent probing = %Ff\n", t->probe_time);
  	
    srand(time(0));
	access_test(t, 100);
	access_test(t, 1000);
	access_test(t, 10000);
	access_test(t, 100000);
	access_test(t, 1000000);
	access_test(t, 1000000000);
    
    /* cleanup */
    sptensor_hash_free(t);
    fclose(file);

    return 0;
}
