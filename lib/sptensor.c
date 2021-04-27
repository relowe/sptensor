#include <sptensor/sptensor.h>

/* wrappers for sptensor functions */
void sptensor_get(sptensor_t * t, sptensor_index_t *i, mpf_t v) { t->get(t, i, v); }
void sptensor_set(sptensor_t * t, sptensor_index_t *i, mpf_t v) { t->set(t, i, v); }
sptensor_iterator_t* sptensor_iterator(sptensor_t *t) { return t->iterator(t); }
sptensor_iterator_t* sptensor_nz_iterator(sptensor_t *t) { return t->nz_iterator(t); }
void sptensor_free(sptensor_t* t){ t->free(t); }

/* wrappers for sptensor iterator */
int sptensor_iterator_next(struct sptensor_iterator* itr) { return itr->next(itr); }
int sptensor_iterator_prev(struct sptensor_iterator* itr) { return itr->prev(itr); }
int sptensor_iterator_valid(struct sptensor_iterator* itr) { return itr->valid(itr); }
void sptensor_iterator_get(struct sptensor_iterator* itr, mpf_t v) { itr->get(itr, v); }
void sptensor_iterator_set(struct sptensor_iterator* itr, mpf_t v) { itr->set(itr, v); }
void sptensor_iterator_free(sptensor_iterator_t* t){ t->free(t); }


/* perform formatted printing of sptensors using the format string specifier for each value */
void sptensor_fprintf(FILE *file, const char *fmt, sptensor_t *t)
{
    sptensor_iterator_t *itr;
    int i;
    mpf_t v;

    mpf_init(v);
    for(itr = sptensor_iterator(t);
        sptensor_iterator_valid(itr);
        sptensor_iterator_next(itr)) {
            /* detect the end of a slice */
            if(t->modes > 2 && itr->index[1] == 1 && itr->index[0] == 1 ){
                /* display the index reference */
                fprintf(file, "\n( , ");
                for(i=2; i<t->modes; i++) {
                    fprintf(file, ", %u", itr->index[i]);
                }
                fprintf(file, ")\n");
            } 

            /* print the value */
            sptensor_get(t, itr->index, v);
            gmp_fprintf(file, fmt, v);

            /* end rows */
            if(t->modes < 2 || itr->index[1] == t->dim[1]) {
                fprintf(file, "\n");
            }
    }

    /* cleanup */
    mpf_clear(v);
    sptensor_iterator_free(itr);
}


/* print the sptensor with a default format string to stdout */
void sptensor_print(sptensor_t *t)
{
    mpz_t max, v2;
    mpf_t v;
    sptensor_iterator_t *itr;
    unsigned int digits;
    char fmt[20];

    /* initialize our values a bit */
    mpz_init_set_ui(max, 0);
    mpz_init(v2);
    mpf_init(v);

    /* compute the maximum number of digits */
    for(itr = sptensor_nz_iterator(t); 
        sptensor_iterator_valid(itr); 
        sptensor_iterator_next(itr)) 
    {
        sptensor_get(t, itr->index, v);
        mpz_set_f(v2, v);
        mpz_abs(v2, v2);
        if(mpz_cmp(v2, max) > 0) {
            mpz_set(max, v2);
        }
    }
    digits = mpz_sizeinbase(max, 10);

    /* clean up*/
    mpf_clear(v);
    mpz_clear(max);
    mpz_clear(v2);
    sptensor_iterator_free(itr);

    /* generate the string */
    sprintf(fmt, "%%%d.03Ff", digits+7);

    /* print the tensor to stdout */
    sptensor_fprintf(stdout, fmt, t);
}