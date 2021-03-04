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
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <gmp.h>
#include <sptensor/index_iterator.h>
#include <sptensor/hash.h>
#include <sptensor/inzt.h>

/*static sp_index_t OCCBIT = UINT_MAX ^ (UINT_MAX >> 1);

#define IS_OCCUPIED(h) ((h) & OCCBIT)
#define OCCUPIED(h) ( (h) | OCCBIT )
#define NOT_OCCUPIED(h) ( (h) & ~OCCBIT )*/

/* Each hash table item has a key and value) */
typedef struct hashtable_item 
{
    mpz_t key;
    mpf_t value;
	
} hash_item;

hash_item* create_hashitem() {
	hash_item* i = malloc(sizeof(hash_item));
	mpz_set(i->key,0);
	mpf_set(i->value,0);
	
	return i;
}

hash_item* set_key(hash_item* i, mpz_t k) {
	i->key = k;
	return i;
}

hash_item* set_value(hash_item* i, mpf_t v) {
	i->value = v;
	return i;
}

mpz_t get_key(hash_item* i) {
	return i->key;
}

mpf_t get_value(hash_item* i) {
	return i->value;
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
	result->nbuckets = NBUCKETS;
	result->curr_size = 0;
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


/* to insert an element in the hash table */
unsigned int sptensor_hash_insert(sptensor_hash_t *t, sptensor_index_t *i, mpf_t v) {
	
	unsigned int index; /*index to compare to*/
	mpf_t nv;
	
	/* find the index */
	index = sptensor_hash_search(t, i);

	if(index == -1) 
	{

		/* not there yet!  If this is zero, we are done */
		if(mpz_cmp_d(v, 0.0) == 0) {
			return;
		}

		/* ok, so we need to init and copy */
		mpf_init_set(nv, v);
			
		/* probing through the array until we reach an empty space */
		while (mpz_cmp_ui(get_key((hash_item*)VPTR(t->hashtable,index))) != 0) {

			if (mpz_cmp_ui(get_key((hash_item*)VPTR(t->hashtable,index)), index) == 0) {

				/* case where already existing key matches the given key */
				printf("\n Key already exists, return its index. \n");
				return index;
			}

			mpz_add(index,index,1);
			mpz_mod(index,index,(t->nbuckets));

			if (mpz_cmp_ui(i, index) == 0) {
				printf("\n Hashtable is full, cannot insert any more item. \n");
				return -1;
			}
		}
	} 

	/* add the key and value into the hash table*/
	set_key((hash_item*)VPTR(t->hashtable,i), i);
	set_value((hash_item*)VPTR(t->hashtable,i), nv);
	t->curr_size = t->curr_size+1;
	printf("\n Key has been inserted \n");

}

/* search the tensor for an index.  Return the element number, -1 on failure */
unsigned int sptensor_hash_search(sptensor_hash_t *t, sptensor_index_t *idx)
{
	unsigned int index;
	
	/* start with an empty morton code */
	mpz_init(idx);

	/* Compress idx using the morton encoding */
	sptensor_inzt_morton(index, idx, index);

	/* mod by number of buckets in hash */
	index = mpz_get_ui(index) % t->nbuckets;
	
	/* probing through the array until we reach an empty space */
	while (mpz_get_ui(get_key((hash_item*)VPTR(t->hashtable,index))) != 1) {

		if (mpz_get_ui(get_key((hash_item*)VPTR(t->hashtable,index))) == index) {

			/* case where already existing key matches the given key */
			printf("\n Key already exists, return its index. \n");
			return index;
		}

		mpz_add(index,index,1);
		mpz_mod(index,index,(t->nbuckets));
		
		if (index == *idx) {
			printf("\n Index not found. \n");
			return -1;
		}

	}
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