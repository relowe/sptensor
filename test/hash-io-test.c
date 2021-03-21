#include <stdio.h>
#include <sptensor/sptensor.h>
#include <sptensor/hash.h>

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
    sptensor_hash_write(stdout, t);

    /* cleanup */
    sptensor_hash_free(t);
    fclose(file);

    return 0;
}