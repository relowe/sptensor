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
void sptensor_iterator_free(sptensor_iterator_t* t){ t->free(t); }
