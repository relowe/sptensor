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

struct hash_item* create_hashitem() {
	struct hash_item *i = malloc(sizeof(hash_item));
	mpz_init(i->key);
	mpf_init(i->value);
	i->flag = 0;
	/*gmp_printf("%Zd\n",i->key);
	gmp_printf("%Ff\n",i->value);
	printf("%d\n",i->flag);*/
	
	return i;
}

/* hash allocation functions */
sptensor_hash_t* sptensor_hash_alloc(sptensor_index_t *modes, int nmodes)
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

    return (sptensor_hash_t*) result;
}


/* free the memory associated with the */
void sptensor_hash_free(sptensor_hash_t* t)
{
    /* free the non-zero poitners */
    if(t->dim) free(t->dim);
    if(t->hashtable) free(t->hashtable);
}


/* Function to insert an element in the hash table.
	r - to hold index value
	v - value to insert 
*/
void sptensor_hash_set(sptensor_hash_t *t, sptensor_index_t *i, mpz_t r, mpf_t v) {

	struct hash_item *ptr;
	mpz_t index; /*compressed version of the original index to insert*/
	mpf_t nv;
	
	
	/* Try to find the index, returns r with index value, -1 if it didn't fid it */
	sptensor_hash_search(t, i, r);
	
	/* start with an empty morton code */
	mpz_init(index);

	/* Compress idx using the morton encoding */
	sptensor_inzt_morton(t->modes, i, index);

	/* mod by number of buckets in hash */
	mpz_mod_ui(index,index,t->nbuckets);
	
	/* Initialize pointer to index */
	ptr = malloc(sizeof(struct hash_item));
	ptr = (struct hash_item*)VPTR(t->hashtable,mpz_get_ui(index));

	if(mpz_get_si(r) == -1) {

		/* not there yet!  If this is zero, we are done */
		if(mpf_cmp_d(v, 0.0) == 0) {
			return;
		}

		/*go ahead and insert */
		/* Init and copy the value */
		mpf_init_set(nv, v);
		
		gmp_printf("inserting at key: %Zd\n", index);

		/* add the key and value into the hash table */
		mpz_set(ptr->key, index);
		mpf_set(ptr->value, nv);
		ptr->flag = 1;
		t->curr_size = t->curr_size + 1;
		gmp_printf("Key %Zd has been inserted. \n", ptr->key);
		return;

	} else {
		/*probe for empty bucket */
		/*gmp_printf("index =  %Zd\n", index);
		gmp_printf("ptr->key =  %Zd\n", ptr->key);*/

		while (ptr->flag == 1) {
			printf("probing for empty space in table...\n");
			/*gmp_printf("ptr->key =  %Zd\n", ptr->key);
			gmp_printf("index =  %Zd\n", index);*/

			mpz_add_ui(r,r,1);
			mpz_mod_ui(r,r,t->nbuckets);

			if (mpz_cmp(r, index) == 0) {
				printf("\nHashtable is full, cannot insert any more item. \n");
				mpz_set_si(r, -1);
				/* Will need to resize & rehash table*/
				return;
			}

			ptr = (struct hash_item*)VPTR(t->hashtable,mpz_get_ui(r));
		}	
		/*if we get here, we found an empty bucket */
		
		mpf_init_set(nv, v);
		gmp_printf("inserting at key: %Zd\n", r);

		/* add the key and value into the hash table */
		mpz_set(ptr->key, r);
		mpf_set(ptr->value, nv);
		ptr->flag = 1;
		t->curr_size = t->curr_size + 1;
		gmp_printf("Key %Zd has been inserted. \n", ptr->key);
		return;
	}
}

/* Search the tensor for an index. Return the element index if found, -1 if not found.
	r - to hold index value upon return

*/
void sptensor_hash_search(sptensor_hash_t *t, sptensor_index_t *idx, mpz_t r)
{
	struct hash_item *ptr;
	mpz_t index;
	
	/* start with an empty morton code */
	mpz_init(index);
	
	/* Compress idx using the morton encoding */
	sptensor_inzt_morton(t->modes, idx, index);

	/* mod by number of buckets in hash */
	mpz_mod_ui(index, index, t->nbuckets);
	
	/* Initialize pointer to that index */
	ptr = malloc(sizeof(struct hash_item));
	ptr = (struct hash_item*)VPTR(t->hashtable,mpz_get_ui(index));

	/*initialize counter for loop */
	mpz_t i;
	mpz_init_set(i,index);
		
	while (1) {
		/*gmp_printf("i =  %Zd\n", i);
		gmp_printf("index =  %Zd\n", index);
		printf("mpz_cmp(ptr->key,i) = %d\n",mpz_cmp(ptr->key,index));*/
		
		if (mpz_cmp(ptr->key,index) == 0) {
			/* case where already existing key matches the given key */
			/*Do I need to check if values are the same?*/
			gmp_printf("\n Key %Zd already exists. \n", ptr->key);
			mpz_set(r, i);
			/*gmp_printf("\nReturning %Zd\n", r);*/
			return;
		}

		mpz_add_ui(i,i,1);
		mpz_mod_ui(i,i,t->nbuckets);
		
		/*gmp_printf("new i =  %Zd\n", i);*/
		
		if (mpz_cmp(i, index) == 0) {
			printf("\n Index not found. \n");
			mpz_set_si(r, -1);
			/*gmp_printf("\n Returning %Zd, this should be -1.\n", r);*/
			return;
		}
		ptr = (struct hash_item*)VPTR(t->hashtable,mpz_get_ui(i));
	}
}

/* Remove element from t's hash table at index i */
void sptensor_hash_remove(sptensor_hash_t *t, sptensor_index_t *i) 
{
	struct hash_item *ptr;
	mpz_t index; /*compressed version of the original index to insert*/	
	
	/* start with an empty morton code */
	mpz_init(index);

	/* Compress i using the morton encoding */
	sptensor_inzt_morton(t->modes, i, index);

	/* mod by number of buckets in hash */
	mpz_mod_ui(index,index,t->nbuckets);
	
	/* Initialize pointer to index */
	ptr = malloc(sizeof(struct hash_item));
	ptr = (struct hash_item*)VPTR(t->hashtable,mpz_get_ui(index));

	gmp_printf("Removing key %Zd... \n", ptr->key);
	mpz_init(ptr->key);
	mpf_init(ptr->value);
	ptr->flag = 0;
	t->curr_size = t->curr_size - 1;
	printf("Key has been removed. \n");
}


