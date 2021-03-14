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
	i->idx = NULL;
	
	return i;
}

/* hash allocation functions */
sptensor_hash_t* sptensor_hash_alloc(sptensor_index_t *modes, int nmodes, int nbuckets)
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
	result->modes_arr = modes;
    result->modes = nmodes;
	result->nbuckets = nbuckets;
	result->hash_curr_size = 0;
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

void set_hashitem(struct hash_item* ptr, mpz_t morton, mpz_t key, mpf_t v, sptensor_index_t *idx) {
	
	mpz_set(ptr->morton, morton);
	mpz_set(ptr->key, key);
	mpf_set(ptr->value, v);
	ptr->idx = idx;

	ptr->flag = 1;
	gmp_printf("Key %Zd has been inserted with value %Ff. \n", ptr->key, ptr->value);
}

/* Function to insert an element in the hash table.
	v- value to insert
*/
void sptensor_hash_set(sptensor_hash_t *t, sptensor_index_t *i, mpf_t v) {

	struct hash_item *ptr;
	mpz_t index; /*modded morton encoding of the original index to insert*/
	mpz_t morton;
	mpz_t key;
	

	/* If this is zero, we are done */
	if(mpf_cmp_d(v, 0.0) == 0) {return;}

	/* Initialize pointer */
	ptr = malloc(sizeof(struct hash_item));
	
	/* Try to find if item already exists in the table.
	Returns a hash item pointer- 
	If found, it returns a pointer to the existing item
	If not found, it returns a pointer to a hash_item, with morton and index fields filled but value = 0.
	*/
	ptr = sptensor_hash_search(t, i);
	
	
	if(mpf_cmp_ui(ptr->value,0) != 0) { return; }

	else {
		/*If we get back an empty pointer, extract values and point to the correct place in table */
		/* mod by number of buckets in hash */
		mpz_init_set(morton, ptr->morton);
		mpz_init_set(index, ptr->key);
		
		/*point to proper slopt in table */
		ptr = (struct hash_item*)VPTR(t->hashtable,mpz_get_ui(index));
		
		/*probe for empty bucket */

		while (ptr->flag == 1) {
			/*printf("probing for empty space in table...\n");*/

			mpz_add_ui(index,index,1);
			mpz_mod_ui(index,index,t->nbuckets);

			ptr = (struct hash_item*)VPTR(t->hashtable,mpz_get_ui(index));
		}	
	}

	/* If we get here, we found an empty bucket */

	gmp_printf("inserting at key: %Zd\n", index);

	/* add the key and value into the hash table */
	set_hashitem(ptr, morton, index, v, i);
	t->hash_curr_size = t->hash_curr_size + 1;
	gmp_printf("new item key: %Zd\n", ptr->key);
	gmp_printf("new item morton value: %Zd\n", ptr->morton);
	gmp_printf("new item value: %Ff\n", ptr->value);
	
	
	/*Before leaving, check if we have reached 80% capacity */
	/*if((t->hash_curr_size/t->nbuckets) > 0.8 ) {
		sptensor_hash_rehash(t);
	}*/
	
	return;

}


/* Search the tensor for an index. Return pointer to the item if found, or a hash_item pointer where value=0 if not found.
*/
struct hash_item* sptensor_hash_search(sptensor_hash_t *t, sptensor_index_t *idx)
{
	struct hash_item *ptr;
	mpz_t index;
	mpz_t i;
	mpz_t morton;
	
	mpz_init(index);
	
	/* start with an empty morton code */
	mpz_init(morton);
	
	/* Compress idx using the morton encoding */
	sptensor_inzt_morton(t->modes, idx, morton);

	/* mod by number of buckets in hash */
	mpz_mod_ui(index, morton, t->nbuckets);
	
	/* Initialize pointer to that index */
	ptr = malloc(sizeof(struct hash_item));
	ptr = (struct hash_item*)VPTR(t->hashtable,mpz_get_ui(index));

	/*initialize counter for loop */
	mpz_init_set(i,index);
		
	while (1) {

		if (mpz_cmp(ptr->key,index) == 0) {
			/* case where already existing key matches the given key */
			/*Do I need to check if values are the same?*/
			gmp_printf("\n Key %Zd already exists. \n", ptr->key);

			return ptr;
		}

		mpz_add_ui(i,i,1);
		mpz_mod_ui(i,i,t->nbuckets);
		
		if (mpz_cmp(i, index) == 0) {
			printf("\n Index not found. \n");
			struct hash_item *empty_hash = create_hashitem();
			mpz_set(empty_hash->morton,morton);
			mpz_set(empty_hash->key,index);
			return empty_hash;
		}
		ptr = (struct hash_item*)VPTR(t->hashtable,mpz_get_ui(i));
	}
}

/*For now we're just doing it the cheaty way */
sptensor_hash_t* sptensor_hash_rehash(sptensor_hash_t *t) {
	
	struct hash_item *ptr;
	sptensor_hash_t *nt;
	int new_hash_size;
	
	/* Double the size of buckets, plus make it a power of 2*/
	new_hash_size = t->nbuckets * 2;

	/* Allocate new tensor */
	nt = sptensor_hash_alloc(t->modes_arr, t->modes, new_hash_size);
	
	/* Allocate memory for the pointer*/
	ptr = malloc(sizeof(struct hash_item));
	
	/* Rehash all existing items in t's hashtable to the new table */
	int i;
	for (i=0; i < t->nbuckets; i++) {
		ptr = (struct hash_item*)VPTR(t->hashtable,i);
		
		/*If occupied, we need to copy it to the other table! */
		if(ptr->flag == 1) {
			sptensor_hash_set(nt, ptr->idx, ptr->value);
		}
	}

	/* Free the old tensor!*/
	sptensor_hash_free(t);
	
	printf("\n Done rehashing.\n");
	return nt;
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
	mpz_init(ptr->morton);
	mpf_init(ptr->value);
	ptr->idx = NULL;
	ptr->flag = 0;
	t->hash_curr_size = t->hash_curr_size - 1;
	printf("Key has been removed. \n");
}


