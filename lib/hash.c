/*
    This is a collection of functions for hashing sparse tensor
    indexes.
    Copyright (C) 2018  Robert Lowe <pngwen@acm.org>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */
#include<limits.h>
#include<stdio.h>
#include <string.h>
#include <gmp.h>
#include <sptensor/index_iterator.h>
#include <sptensor/hash.h>
#include <sptensor/inzt.h>

/*static sp_index_t OCCBIT = UINT_MAX ^ (UINT_MAX >> 1);

#define IS_OCCUPIED(h) ((h) & OCCBIT)
#define OCCUPIED(h) ( (h) | OCCBIT )
#define NOT_OCCUPIED(h) ( (h) & ~OCCBIT )*/

/* nonzero iterator */
/* sptensor iterator (base type) */
/*struct sptensor_hash_nz_iterator
{
    sptensor_t *t;
    sptensor_index_t *index;
    sptensor_iterator_valid_f valid;
    sptensor_iterator_next_f next;
    sptensor_iterator_prev_f prev;
    sptensor_free_f free;

    int ci;  /* The indx within the COO list 
};*/

/* each hash table item has a flag (status) and data (consisting of key and value) */
typedef struct hashtable_item 
{
 
    int flag;
    /*
     * flag = 0 : data does not exist
     * flag = 1 : data exists
     * flag = 2 : data existed at least once
    */
    int key;
    int value;
 
} hash_item;

hash_item* create_hashitem() {
	hash_item* i = malloc(sizeof(hash_item));
	i->key = NULL;
	i->value = NULL;
	i->flag = 0;
	return i;
}
/*
hash_item* set_key(hash_item* i, int k) {
	i->key = k;
	return i;
}

hash_item* set_value(hash_item* i, int v) {
	i->value = v;
	return i;
}

hash_item* set_flag(hash_item* i, int f) {
	i->flag = f;
	return i;
}*/

int get_flag(hash_item* i) {
	return i->flag;
}

/* hash allocation functions */
sptensor_t* sptensor_hash_alloc(sptensor_index_t *modes, int nmodes)
{
    sptensor_hash_t *result;

    /* allocate result */
    result = malloc(sizeof(sptensor_hash_t));
    if(!result) { return NULL; }
    result->dim = NULL;

    /* allocate the modes */
    result->dim = malloc(sizeof(sptensor_index_t) * nmodes);
    if(!result->dim) {
        sptensor_hash_free(result);
        return NULL;
    }
	
    /* copy the dimensions */
    memcpy(result->dim, modes, nmodes * sizeof(sptensor_index_t));

    /* populate the fields */
    result->modes = nmodes;
	result->nbuckets = NBUCKETS;	/*this may need to be changed*/
	result->curr_size = 0;	/*this will grow as items are added to the hash table */
    result->get = (sptensor_get_f) sptensor_hash_get;
    result->set = (sptensor_set_f) sptensor_hash_set;
    result->free = (sptensor_free_f) sptensor_hash_free;
    /*result->iterator = (sptensor_iterator_f) sptensor_hash_iterator;
    result->nz_iterator = (sptensor_iterator_f) sptensor_hash_nz_iterator;*/
	
	/* Allocate the vectors */
    result->hashtable = sptensor_vector_alloc(sizeof(hash_item)*result->nbuckets,
											  SPTENSOR_VECTOR_DEFAULT_CAPACITY);
	
	/* initializing hash table array */	
	int i;
	
    for (i = 0; i < result->nbuckets; i++) {
		sptensor_vector_push_back(result->hashtable, create_hashitem());
	}

    return (sptensor_t*) result;
}


/* free the memory associated with the */
void sptensor_hash_free(sptensor_hash_t* t)
{
    /* free the non-zero poitners */
    if(t->dim) free(t->dim);
    if(t->hashtable) free(t->hashtable);
}


/* search the tensor for an index.  Return the element number, -1 on failure */
int sptensor_hash_search(sptensor_hash_t *t, sptensor_index_t *idx)
{
	mpz_t morton;
	int n = t->modes;
	unsigned int index;
    unsigned int i = index;
	
	/* start with an empty morton code */
	mpz_init(morton);

	/* Compress idx using the morton encoding */
	sptensor_inzt_morton(n, idx, morton);

	/* mod by number of buckets in hash */
	index = mpz_get_ui(morton) % t->nbuckets;
	
	struct hash_item *test = (hash_item*)VPTR(t->hashtable,0);
	int flag = get_flag(test);
	printf("%d\n", flag);
	
	/* probing through the array until we reach an empty space */
	/*while (t->hashtable[i].flag == 1) {

		if (t->hashtable[i].data->key == *idx) {*/

			/* case where already existing key matches the given key */
			/*printf("\n Key already exists, return its index. \n");
			return i;
		}

		i = (i + 1) % t->nbuckets;
		
		if (i == index) {
			printf("\n Index not found. \n");
			return -1;
		}

	}*/
}

/* Retrieve an alement from the hash tensor */
void sptensor_hash_get(sptensor_hash_t * t, sptensor_index_t *i, mpf_t v)
{
    /*int index;

    /* find the index 
    index = sptensor_hash_search(t, i);

    /* return the value 
    if(index == -1) {
        mpf_set_d(v, 0.0);
    } else {
        mpf_set(v, VVAL(mpf_t, t->data, index));
    }*/
}

/* set the element with the coo tensor  */
void sptensor_hash_set(sptensor_hash_t * t, sptensor_index_t *i, mpf_t v)
{
    /*int index;
    
    /* find the index 
    index = sptensor_hash_search(t, i);

    if(index == -1) 
    {
        mpf_t nv;

        /* not there yet!  If this is zero, we are done 
        if(mpf_cmp_d(v, 0.0) == 0) {
            return;
        }

        /* ok, so we need to init and copy 
        mpf_init_set(nv, v);

        /* and then we add it to the vector 
        sptensor_vector_push_back(t->index, i);
        sptensor_vector_push_back(t->data, nv);
    } else {
        /* if we are changing an existing element 
        if(mpf_cmp_d(v, 0.0) == 0) {
            /* remove the zero 
            sptensor_vector_remove(t->index , index);
            sptensor_vector_remove(t->data, index);
        } else {
            /* set the element 
            mpf_set(VVAL(mpf_t, t->data, index), v);
        }
    }*/
}

/*Iterators*/
/* Iterator Functions */
sptensor_iterator_t* sptensor_hash_iterator(sptensor_hash_t *t) 
{
    return sptensor_index_iterator_alloc((sptensor_t*) t);
}