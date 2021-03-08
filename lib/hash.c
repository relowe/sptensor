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
	/*gmp_printf("%Zd\n",i->key);
	gmp_printf("%Ff\n",i->value);*/
	
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

	if(mpz_get_si(r) == -1) {
		printf("Could not find index in table. Proceeding to insert...\n");

		/* not there yet!  If this is zero, we are done */
		if(mpf_cmp_d(v, 0.0) == 0) {
			return;
		}

		/* ok, so we need to init and copy */
		mpf_init_set(nv, v);

		/* Compress idx using the morton encoding */
		mpz_init(index);
		sptensor_inzt_morton(t->modes, i, index);
		printf("Finished morton encoding...\n");
		gmp_printf("index: %Zd\n", index);

		ptr = malloc(sizeof(struct hash_item));
		ptr = (struct hash_item*)VPTR(t->hashtable,mpz_get_ui(index));
		
		gmp_printf("index =  %Zd\n", index);
		gmp_printf("ptr->key =  %Zd\n", ptr->key);

		printf("probing for empty space...\n");
		
		/* probing through the array until we reach an empty space */
		while (mpz_cmp_ui(ptr->key, 1) != 0) {
	        gmp_printf("ptr->key =  %Zd\n", ptr->key);
			
			/*WORKING HERE*/
			if (mpz_cmp_ui(ptr->key, mpz_get_ui(r)) == 0) {

				/* case where already existing key matches the given key */
				printf("\n Key already exists, return its index. \n");
				gmp_printf("\nReturning %Zd, this should be -1.\n", r);
				return;
			}

			mpz_add_ui(r,r,1);
			mpz_mod_ui(r,r,t->nbuckets);

			if (mpz_cmp(r, index) == 0) {
				printf("\n Hashtable is full, cannot insert any more item. \n");
				mpz_set_si(r, -1);
				/* Will need to resize & rehash table*/
				return;
			}

			ptr = (struct hash_item*)VPTR(t->hashtable,mpz_get_ui(r));
		}

	}
	
	printf("We are good to insert.\n");
	
	/* mod by number of buckets in hash */
	mpz_mod_ui(index, index, t->nbuckets);
	printf("Finished modding...\n");
	gmp_printf("modded index: %Zd\n", index);

	
	/* add the key and value into the hash table*/
	printf("\n Inserting item... \n");
	gmp_printf("inserting at key: %Zd\n", index);

	mpz_set(ptr->key, index);
	gmp_printf("ptr->key =  %Zd\n", ptr->key);
	mpf_set(ptr->value, nv);
	t->curr_size = t->curr_size + 1;
	printf("\n Key has been inserted \n");
}

/* Search the tensor for an index. Return the element number, -1 on failure 
	r - to hold index value upon return

*/
void sptensor_hash_search(sptensor_hash_t *t, sptensor_index_t *idx, mpz_t r)
{
	struct hash_item *ptr;
    ptr = malloc(sizeof(struct hash_item));
	mpz_t morton;
	mpz_t index;
	
	/* start with an empty morton code */
	mpz_init(morton);
	mpz_init(index);

	/* Compress idx using the morton encoding */
	sptensor_inzt_morton(t->modes, idx, morton);

	printf("Finished morton encoding...\n");
	
	/* mod by number of buckets in hash */
	mpz_mod_ui(index, morton,t->nbuckets);

	printf("done compressing morton code...\n");
	
	ptr = (struct hash_item*)VPTR(t->hashtable,mpz_get_ui(index));

	/*initialize counter for loop */
	mpz_t i;
	mpz_init(i);
	mpz_set(i,index);
	
	/* probing through the array until we reach an empty space */
	while (mpz_cmp_ui(ptr->key, 1) != 0) {
		/*gmp_printf("i =  %Zd\n", i);
		gmp_printf("index =  %Zd\n", index);
		printf("mpz_cmp(ptr->key,i) = %d\n",mpz_cmp(ptr->key,index));*/
		
		if (mpz_cmp(ptr->key,index) == 0) {
			/* case where already existing key matches the given key */
			printf("\n Key already exists, return its index. \n");
			mpz_set(r, i);
			gmp_printf("\nReturning %Zd\n", r);
			return;
		}

		mpz_add_ui(i,i,1);
		mpz_mod_ui(i,i,t->nbuckets);
		
		/*gmp_printf("new i =  %Zd\n", i);*/
		
		if (mpz_cmp(i, index) == 0) {
			printf("\n Index not found. \n");
			mpz_set_si(r, -1);
			gmp_printf("\nReturning %Zd, this should be -1.\n", r);
			return;
		}
		ptr = (struct hash_item*)VPTR(t->hashtable,mpz_get_ui(i));
	}
}