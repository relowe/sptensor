#include <stdio.h>
#include <sptensor/sptensor.h>
#include <sptensor/coo.h>
#include <sptensor/product.h>

static void fill_tensor(sptensor_t *t, sptensor_index_t *data, int n) 
{
    mpf_t value;
    int i;

    /* allocate the value */
    mpf_init(value);

    /* copy the data into the tensor */
    for(i=0; i<n; i++) {
        /* set up the value */
        mpf_set_ui(value, data[t->modes]);

        /* set the value */
        sptensor_set(t, data, value);

        /* next index! */
        data += t->modes + 1;
    }


    /* cleanup */
    mpf_clear(value);
}


static void matrix_test()
{
    sptensor_index_t m1[] = {1, 1, 3,
                             1, 2, 1,
                             1, 3, 4,
                             2, 2, 2,
                             2, 3, 5};
    sptensor_index_t m2[] = {1, 1, 2,
                             1, 3, 1,
                             2, 2, 2,
                             3, 3, 2};
    sptensor_index_t dim[][2] = {{2, 3}, {3, 3}};
    sptensor_t *c, *a, *b;
    sptensor_shape_t *shape;

    /* create the operands */
    a = sptensor_coo_alloc(dim[0], 2);
    fill_tensor(a, m1, 5);
    b = sptensor_coo_alloc(dim[1], 2);
    fill_tensor(b, m2, 4);

    /* create the result */
    shape = sptensor_matrix_product_shape(a, b);
    c = sptensor_coo_alloc(shape->dim, shape->modes);
    sptensor_shape_free(shape);

    /* do the multiplication */
    sptensor_matrix_product(c, a, b);

    /* print the results */
    printf("Matrix Multiplication Test\n");
    printf("==========================================\n");
    printf("A=\n");
    sptensor_print(a);
    printf("B=\n");
    sptensor_print(b);
    printf("A*B=\n");
    sptensor_print(c);

    /* cleanup */
    sptensor_free(a);
    sptensor_free(b);
    sptensor_free(c);
}


static void nmode_test()
{
    sptensor_index_t ydat[] = { 1, 1, 1, 1,
                                1, 2, 1, 4,
                                1, 3, 1, 7,
                                1, 4, 1, 10,
                                2, 1, 1, 2,
                                2, 2, 1, 5,
                                2, 3, 1, 8,
                                2, 4, 1, 11,
                                3, 1, 1, 3,
                                3, 2, 1, 6,
                                3, 3, 1, 9,
                                3, 4, 1, 12,
                                1, 1, 2, 13,
                                1, 2, 2, 16,
                                1, 3, 2, 19,
                                1, 4, 2, 22,
                                2, 1, 2, 14,
                                2, 2, 2, 17,
                                2, 3, 2, 20,
                                2, 4, 2, 23,
                                3, 1, 2, 15,
                                3, 2, 2, 18,
                                3, 3, 2, 21,
                                3, 4, 2, 24};
    sptensor_index_t udat[] = { 1, 1, 1, 
                                1, 2, 3,
                                1, 3, 5,
                                2, 1, 2,
                                2, 2, 4,
                                2, 3, 6 };
    sptensor_index_t ydim[] = {3, 4, 2};
    sptensor_index_t udim[] = {2, 3};
    sptensor_t *c, *a, *b;
    sptensor_shape_t *shape;

    /* create the operands */
    a = sptensor_coo_alloc(ydim, 3);
    b = sptensor_coo_alloc(udim, 2);
    fill_tensor(a, ydat, 24);
    fill_tensor(b, udat, 6);

    /* create the result */
    shape = sptensor_nmode_product_shape(0, a, b);
    c = sptensor_coo_alloc(shape->dim, shape->modes);
    sptensor_shape_free(shape);

    /* perform the multiplication */
    sptensor_nmode_product(0, c, a, b);

    /* print the results */
    printf("nmode Product Test\n");
    printf("==========================================\n");
    printf("A=\n");
    sptensor_print(a);
    printf("B=\n");
    sptensor_print(b);
    printf("A *_0 B=\n");
    sptensor_print(c);

    /* cleanup */
    sptensor_free(a);
    sptensor_free(b);
    sptensor_free(c);
}


static void outer_test()
{
    sptensor_index_t adat[] = { 1, 1, 1,
                                1, 2, 4,
                                1, 3, 7,
                                1, 4, 10,
                                2, 1, 2,
                                2, 2, 5,
                                2, 3, 8,
                                2, 4, 11,
                                3, 1, 3,
                                3, 2, 6,
                                3, 3, 9,
                                3, 4, 12};
    sptensor_index_t bdat[] = { 1, 1,
                                2, 2,
                                3, 3};
    sptensor_index_t adim[] = {3, 4};
    sptensor_index_t bdim[] = {3};
    sptensor_shape_t *shape;
    sptensor_t *a, *b, *c;

    /* create the operands */
    a = sptensor_coo_alloc(adim, 2);
    b = sptensor_coo_alloc(bdim, 1);
    fill_tensor(a, adat, 12);
    fill_tensor(b, bdat, 3);

    /* allocate the result */
    shape = sptensor_outer_product_shape(a, b);
    c = sptensor_coo_alloc(shape->dim, shape->modes);
    sptensor_shape_free(shape);

    /* perform the multiplication */
    sptensor_outer_product(c, a, b);


    /* print the results */
    printf("Outer Product Test\n");
    printf("==========================================\n");
    printf("A=\n");
    sptensor_print(a);
    printf("B=\n");
    sptensor_print(b);
    printf("A*B=\n");
    sptensor_print(c);

    /* cleanup */
    sptensor_free(a);
    sptensor_free(b);
    sptensor_free(c);
}

int main()
{
    matrix_test();
    nmode_test();
    outer_test();
}