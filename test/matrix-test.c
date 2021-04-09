#include <sptensor/sptensor.h>
#include <sptensor/matrix.h>

int main(int argc, int argv)
{
    sptensor_t *t;
    sptensor_index_t dim[2] = {3, 5};
    sptensor_index_t idx[2];
    int i, j;
    mpf_t v;

    /* create the tensor */
    t = sptensor_matrix_alloc(dim);

    mpf_init(v);
    for(i=0; i<dim[0]; i++) {
        idx[0] = i;
        for(j=0; j<dim[1]; j++) {
            idx[1] = j;
            mpf_add_ui(v, v, 1);
            sptensor_set(t, idx, v);
        }
    }
    mpf_clear(v);

    sptensor_print(t);

    sptensor_free(t);
}