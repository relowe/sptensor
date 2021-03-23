#include <stdio.h>
#include <sptensor/sptensor.h>
#include <sptensor/constant.h>
#include <sptensor/identity.h>
#include <sptensor/coo.h>


int main()
{
    sptensor_t *c, *i;
    sptensor_index_t dim[] = {3, 3, 3};
    mpf_t v;

    /* create constant tensor */
    printf("CONSTANT TENSOR\n");
    printf("===============\n");
    mpf_init_set_d(v, 42.0);
    c = sptensor_constant_alloc(dim, 3, v);
    printf("COO\n");
    sptensor_coo_write(stdout, c);
    printf("Pretty Print\n");
    sptensor_print(c);
    mpf_clear(v);
    sptensor_free(c);

    printf("\n\n");

    /* create the identity tensor */
    printf("3 MODE IDENTITY\n");
    printf("===============\n");
    i = sptensor_identity_alloc(dim, 3);
    printf("COO\n");
    sptensor_coo_write(stdout, i);
    printf("Pretty Print\n");
    sptensor_print(i);
    printf("\n\n");
    printf("2 MODE IDENTITY\n");
    printf("===============\n");
    i = sptensor_identity_alloc(dim, 2);
    printf("COO\n");
    sptensor_coo_write(stdout, i);
    printf("Pretty Print\n");
    sptensor_print(i);
}