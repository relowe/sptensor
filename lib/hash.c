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
#include <stdlib.h> /*for system()*/
#include <string.h>
#include <gmp.h>
#include <sptensor/index_iterator.h>
#include <sptensor/hash.h>
#include <sptensor/inzt.h>

struct hash_item* create_hashitem() {
	struct hash_item *i = malloc(sizeof(hash_item));
	mpz_init(i->key);
	mpz_init(i->morton);
	mpf_init(i->value);
	i->flag = 0;
	i->idx = NULL;
	
	return i;
}

void free_hashitem(struct hash_item *ptr) {
	mpz_clear(ptr->morton);
	mpz_clear(ptr->key);
	mpf_clear(ptr->value);
	free(ptr->idx);
}

/* hash allocation functions */
sptensor_hash_t* sptensor_hash_alloc(sptensor_index_t *modes, int nmodes) {
	
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
	result->nbuckets = NBUCKETS;
	result->hash_curr_size = 0;
    result->free = (sptensor_free_f) sptensor_hash_free;
    /*result->iterator = (sptensor_iterator_f) sptensor_hash_iterator;
    result->nz_iterator = (sptensor_iterator_f) sptensor_hash_nz_iterator;*/
	
	/* Allocate the vectors */
    result->hashtable = sptensor_vector_alloc(sizeof(struct hash_item),result->nbuckets);
	
	/* initializing hash table array */	
	int i;
	
    for (i = 0; i < result->nbuckets; i++) {
		sptensor_vector_push_back(result->hashtable, create_hashitem());
	}

    return (sptensor_hash_t*) result;
}


/* free the memory associated with the */
void sptensor_hash_free(sptensor_hash_t* t) {
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
	/*gmp_printf("Key %Zd has been inserted with value %Ff. \n", ptr->key, ptr->value);*/
}

/* Function to insert an element in the hash table.
*/
void sptensor_hash_set(sptensor_hash_t *t, sptensor_index_t *i, mpf_t v) {

	int insert_success = 0;
	
	insert_success = sptensor_hash_set_helper(t->hashtable, i, v, t->nbuckets, t->modes);

	if(insert_success) {
		t->hash_curr_size = t->hash_curr_size + 1;


		/*printf("Current capacity at: %f\n", (double)t->hash_curr_size/t->nbuckets);*/
		/*Before leaving, check if we have reached 80% capacity */
		if((double)t->hash_curr_size/t->nbuckets > 0.8 ) {
			/*printf("Preparing to rehash...\n");*/
			sptensor_hash_rehash(t);
			/*printf("finished rehashing.\n");*/
		}
		
		return;
	}
	
	/*printf("Did not insert index.\n");*/
	return;

}

/*Helper function for sptensor_hash_set. So we can rehash!
	-returns 1 on success, 0 if item was not inserted.
*/
static int sptensor_hash_set_helper(sptensor_vector* hashtable, sptensor_index_t *i, mpf_t v, int nbuckets, int modes) 
{
	struct hash_item *ptr;
	mpz_t index; /*modded morton encoding of the original index to insert*/
	mpz_t morton;
	mpz_t key;
	
	/* If this is zero, we are done */
	if(mpf_cmp_d(v, 0.0) == 0) { return 0; }

	/* Initialize pointer */
	ptr = malloc(sizeof(struct hash_item));
	
	/* Try to find if item already exists in the table.
	Returns a hash item pointer- 
	If found, it returns a pointer to the existing item
	If not found, it returns a pointer to a hash_item, with morton and index fields filled but value = 0.
	*/
	ptr = sptensor_hash_search(hashtable, i, v, nbuckets, modes);
	
	if(mpf_cmp_ui(ptr->value,0) != 0) { 
		/*printf("item was found already in table.\n");*/
		return 0; 
	}

	else {
		/*If we get back an empty pointer, extract values and point to the correct place in table */
		/* mod by number of buckets in hash */
		mpz_init_set(morton, ptr->morton);
		mpz_init_set(index, ptr->key);

		/*point to proper slot in table */
		ptr = (struct hash_item*)VPTR(hashtable,mpz_get_ui(index));
		
		/*probe for empty bucket */

		while (ptr->flag == 1) {
			/*printf("probing for empty space in table...\n");*/

			mpz_add_ui(index,index,1);
			mpz_mod_ui(index,index,nbuckets);

			ptr = (struct hash_item*)VPTR(hashtable,mpz_get_ui(index));
		}
	}

	/* If we get here, we found an empty bucket */
	/*gmp_printf("inserting at key: %Zd\n", index);*/

	/* add the key and value into the hash table */
	set_hashitem(ptr, morton, index, v, i);

	return 1;
}

/* Search the tensor for an index. Return pointer to the item if found, or a hash_item pointer where value=0 if not found.
*/
struct hash_item* sptensor_hash_search(sptensor_vector* hashtable, sptensor_index_t *idx, mpf_t v, int nbuckets, int modes)
{
	struct hash_item *ptr;
	mpz_t index;
	mpz_t i;
	mpz_t morton;
	
	/* Init */
	mpz_init(index);
	mpz_init(morton);
	
	/* Compress idx using the morton encoding */
	sptensor_inzt_morton(modes, idx, morton);

	/* mod by number of buckets in hash */
	mpz_mod_ui(index, morton, nbuckets);

	/* Initialize pointer to that index */
	ptr = malloc(sizeof(struct hash_item));
	ptr = (struct hash_item*)VPTR(hashtable,mpz_get_ui(index));

	/*initialize counter for loop */
	mpz_init_set(i,index);
		
	/*gmp_printf("\n Searching for Key %Zd.\n", index);*/
	while (1) {
		
		if (mpz_cmp(ptr->key,index) == 0) {
			/*Overwirte existing value*/
			/*gmp_printf("Overwriting Key %Zd with value %Ff. \n", ptr->key, ptr->value);*/
			mpf_set(ptr->value,v);
			return ptr;
		}

		mpz_add_ui(i,i,1);
		mpz_mod_ui(i,i,nbuckets);
		
		if (mpz_cmp(i, index) == 0) {
			/*printf("Index + value pair not found. \n");*/
			struct hash_item *empty_item = create_hashitem();
			mpz_set(empty_item->morton,morton);
			mpz_set(empty_item->key,index);
			return empty_item;
		}
		ptr = (struct hash_item*)VPTR(hashtable,mpz_get_ui(i));
	}
}

/*Retry to rehash properly */
void sptensor_hash_rehash(sptensor_hash_t *t) {
	
	struct hash_item *ptr;
	sptensor_vector* new_hashtable;
	int new_hash_size;

	/* Double the number of buckets*/
	new_hash_size = t->nbuckets * 2;
	
	/* Allocate the vectors */
    new_hashtable = sptensor_vector_alloc(sizeof(struct hash_item), new_hash_size);
	/* initialize new hash table array */	
	int i;
	
    for (i = 0; i < new_hash_size; i++) { 
		sptensor_vector_push_back(new_hashtable, create_hashitem()); 
	}
	
	
	/* Allocate memory for the pointer*/
	ptr = malloc(sizeof(struct hash_item));
	
	/*printf("copying old values to new tensor...\n");*/
	
	/* Rehash all existing items in t's hashtable to the new table */
	for (i=0; i < t->nbuckets; i++) {
		ptr = (struct hash_item*)VPTR(t->hashtable,i);
		
		/*If occupied, we need to copy it to the other table! */
		if(ptr->flag == 1) {
			sptensor_hash_set_helper(new_hashtable, ptr->idx, ptr->value, new_hash_size, t->modes);
			/*printf("inserted item into new hashtable.\n");*/
		}
	}

	/*printf("values successfuly copied over.\n");*/
	
	/* Free the old tensor!*/
	free(t->hashtable);
	
	/*set it to the new one*/
	t->hashtable = new_hashtable;
	/*printf("new hashtable assigned to tensor.\n");*/
	
	/*Update the number of buckets*/
	t->nbuckets = new_hash_size;
}

/* Remove element from t's hash table at index i with value v*/
void sptensor_hash_remove(sptensor_hash_t *t, sptensor_index_t *i, mpf_t v) 
{
	struct hash_item *ptr;
	
	/* Allocate pointer*/
	ptr = malloc(sizeof(struct hash_item));
	
	ptr = sptensor_hash_search(t->hashtable, i, v, t->nbuckets,t->modes);
	
	if(ptr->flag == 1) {

		gmp_printf("Removing key %Zd... \n", ptr->key);
		mpz_init(ptr->key);
		mpz_init(ptr->morton);
		mpf_init(ptr->value);
		ptr->idx = NULL;
		ptr->flag = 0;
		t->hash_curr_size = t->hash_curr_size - 1;
		printf("Key has been removed. \n");
	} else {
		printf("Key is not in table, so it was not removed. \n");
	}
}

/* 
 * Read a HASH tensor from a text file stream. The file stream is
 * expected to be in the form of:
 *   nmodes d0 d1 ... dk
 *   i0 i1 i2 ... ik v1
 *   i0 i1 i2 ... ik v2
 *   ...
 * Where nmodes is the number of modes, d0...dk are the dimensions,
 * and i0..ik are the indexes and v is the value at that index.
 * The file is read to the end, or until an unsucessful translation
 * occurs.
 *
 * Parameters: file - The file to read from
 *             
 * Return: The newly read and allocated tensor.
 */
sptensor_hash_t * sptensor_hash_read(FILE *file) 
{
    int nmodes;
    sptensor_index_t *idx;
    int i;
    mpf_t val;
    sptensor_hash_t *tns;
    int done;
	
	/*Extra to help know our progress of reading in the tensor*/
	int count = 0;

    /* a little bit of mpf allocation */
    mpf_init(val);

    /* get the dimensions */
    fscanf(file, "%u", &nmodes);
    idx = sptensor_index_alloc(nmodes);
    for(i=0; i<nmodes; i++) {
	    gmp_fscanf(file, "%u", idx + i);
    }

    /* allocate the tensor and free the dimension */
    tns = sptensor_hash_alloc(idx, nmodes);

    i=0;
    done = 0;
    while(!done) {
	    /* read the indexes */
	    for(i=0; i<nmodes; i++) {
	        if(gmp_fscanf(file, "%u", idx + i) != 1) {
		        done = 1;
		        break;
	        }
	    }

	    if(done) continue;
	    if(gmp_fscanf(file, "%Ff", val) != 1) {
	        done = 1;
	        break;
	    }

	    /* insert into the tensor */
	    sptensor_hash_set(tns, idx, val);
		count = count + 1;
		printf("number of items inserted = %d\n", count);
		system("clear"); /*clear output screen*/
    }

    /* cleanup and return! */
    free(idx);
    mpf_clear(val);

    return tns;
}

void sptensor_hash_write(FILE *file, sptensor_hash_t *tns) 
{
	
}
