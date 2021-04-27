#include <sptensor/sptensor.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int nmodes;
    int i;
    mpf_t val;
    int done;
    sptensor_index_t *idx;
    FILE *file;

    /* get the file open */
    if(argc != 2) {
        printf("Usage: %s filename\n", argv[0]);
        return -1;
    }
    file = fopen(argv[1], "r");
    if(!file) {
        printf("Could not open %s\n", argv[1]);
        return -2;
    }

    /* a little bit of mpf allocation */
    mpf_init(val);


    /* get the dimensions */
    fscanf(file, "%u", &nmodes);
    idx = sptensor_index_alloc(nmodes);
    for(i=0; i<nmodes; i++) {
	    gmp_fscanf(file, "%u", idx + i);
    }

    i=0;
    done = 0;
    while(!done) {
	    /* read the indexes */
	    for(i=0; i<nmodes; i++) {
	        if(gmp_fscanf(file, "%u", idx + i) != 1) {
		        done = 1;
		        break;
	        }
	    }

	    if(done) continue;
	    if(gmp_fscanf(file, "%Ff", val) != 1) {
	        done = 1;
	        break;
	    }
    }

    /* cleanup and return! */
    fclose(file);
    free(idx);
    mpf_clear(val);
}