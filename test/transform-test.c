#include <stdio.h>
#include <sptensor/sptensor.h>
#include <sptensor/coo.h>
#include <sptensor/transform.h>

void printout(const char *label, sptensor_t *tensor)
{
    /* print the label and then the correct number of ='s with some leader space*/
    printf("\n\n%s\n", label);
    printf("==========================================\n");

    /* print the tensor */
    sptensor_print(tensor);
}


void transpose_test(sptensor_t *t, unsigned int i, unsigned int j) 
{
    sptensor_shape_t *shape;
    sptensor_t *result;
    char buf[100];

    /* create the result tensor */
    shape = sptensor_transpose_shape(t, i, j);
    result = sptensor_coo_alloc(shape->dim, shape->modes);
    sptensor_shape_free(shape);

    /* perform the transposition */
    sptensor_transpose(result, t, i, j);

    /* show the result */
    sprintf(buf, "Transpose %u, %u", i, j);
    printout(buf, result);

    /* cleanup */
    sptensor_free(result);
}


void unfold_test(sptensor_t *t, unsigned int n)
{
    sptensor_shape_t *shape;
    sptensor_t *result;
    char buf[100];

    /* create the result tensor */
    shape = sptensor_unfold_shape(t, n);
    result = sptensor_coo_alloc(shape->dim, shape->modes);
    sptensor_shape_free(shape);

    /* perform the transposition */
    sptensor_unfold(result, t, n);

    /* show the result */
    sprintf(buf, "Unfold %u", n);
    printout(buf, result);

    /* cleanup */
    sptensor_free(result);
}


void slice_test(sptensor_t *t, sptensor_index_t *spec)
{
    sptensor_shape_t *shape;
    sptensor_t *result;
    char buf[100];

    /* create the result tensor */
    shape = sptensor_slice_shape(t, spec);
    result = sptensor_coo_alloc(shape->dim, shape->modes);
    sptensor_shape_free(shape);

    /* perform the transposition */
    sptensor_slice(result, t, spec);

    /* show the result */
    sprintf(buf, "Slice %u, %u, %u", spec[0], spec[1], spec[2]);
    printout(buf, result);

    /* cleanup */
    sptensor_free(result);
}


int main()
{
    sptensor_t *base;  /* the base tensor */
    sptensor_index_t dim[] = {3, 4, 2};
    sptensor_index_t spec[][3] = {{0, 0, 1},
                                  {0, 0, 2},
                                  {2, 2, 0},
                                  {0, 1, 1},
                                  {3, 0, 2}
                                 };
    sptensor_iterator_t *itr;
    mpf_t v;
    int i;

    mpf_init(v);

    /* create the base tensor */
    base = sptensor_coo_alloc(dim, 3);
    i=1;
    itr = sptensor_iterator(base);
    do {
        mpf_set_ui(v, i);
        sptensor_set(base, itr->index, v);
        i++;
    } while(sptensor_iterator_next(itr));
    sptensor_iterator_free(itr);

    /* print the base tensor */
    printout("Base", base);

    /* perform the transpose tests */
    transpose_test(base, 0, 1); 
    transpose_test(base, 1, 2); 
    transpose_test(base, 0, 2);

    /* perform the unfold tests */
    unfold_test(base, 0);
    unfold_test(base, 1);
    unfold_test(base, 2);

    /* perform the slice tests */
    for(i = 0; i<sizeof(spec)/sizeof(spec[0]); i++) {
        slice_test(base, spec[i]);
    }

    /* cleanup */
    sptensor_free(base);
    mpf_clear(v);
}