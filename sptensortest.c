#include <stdio.h>
#include <sptensor.h>
#include <sptensorio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
    sptensor *sp;
    FILE *file;

    /* read the tensor */
    file = fopen(argv[1], "r");
    sp = sptensor_read(file);
    fclose(file);

    /* print the tensor */
    sptensor_write(stdout, sp);
}
