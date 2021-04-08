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
/*#include <stdlib.h> /*for system()*/
#include <string.h>
#include <time.h>
#include <gmp.h>
#include <sptensor/index_iterator.h>
#include <sptensor/hash.h>
#include <sptensor/inzt.h>

/* Static Helper Functions */
static struct sptensor_hash_item* sptensor_hash_search(sptensor_hash_item_t *hashtable, sptensor_index_t *idx, int nbuckets, int modes);
static void sptensor_hash_remove(sptensor_hash_t *t, sptensor_hash_item_t *ptr);
static void sptensor_hash_rehash(sptensor_hash_t *t);

static sptensor_hash_item_t* create_hashtable(unsigned int buckets, unsigned int nmodes) {
	sptensor_hash_item_t *table;
	int i;

	/* allocate the table */	
	table = malloc(sizeof(sptensor_hash_item_t) * buckets);

	for(i=0; i<buckets; i++) {
		mpz_init(table[i].key);
		mpz_init(table[i].morton);
		mpf_init(table[i].value);
		table[i].flag = 0;
		table[i].idx = sptensor_index_alloc(nmodes);
	}
	
	return table;
}


static void free_hashtable(sptensor_hash_item_t *table, unsigned int buckets) {
	int i;
	sptensor_hash_item_t *ptr;

	/* free up all the stuff in each bucket */
	ptr = table;
	for(i=0; i<buckets; i++) {
		mpz_clear(ptr->morton);
		mpz_clear(ptr->key);
		mpf_clear(ptr->value);
		sptensor_index_free(ptr->idx);
		ptr++;
	}

	free(table);
}


/* hash allocation functions */
sptensor_t* sptensor_hash_alloc(sptensor_index_t *modes, int nmodes) {
	
    sptensor_hash_t *result;

    /* allocate result */
    result = malloc(sizeof(sptensor_hash_t));
    if(!result) { return NULL; }
    result->dim = NULL;

    /* allocate the modes */
    result->dim = sptensor_index_alloc(nmodes);
    if(!result->dim) {
        sptensor_hash_free(result);
        return NULL;
    }
	
    /* copy the dimensions */
	sptensor_index_cpy(nmodes, result->dim, modes);

    /* populate the fields */
	result->modes_arr = modes;
    result->modes = nmodes;
	result->nbuckets = NBUCKETS;
	mpz_init(result->num_collisions);
	mpf_init(result->probe_time);
	result->set = (sptensor_set_f) sptensor_hash_set;
	result->get = (sptensor_get_f) sptensor_hash_get;
    result->free = (sptensor_free_f) sptensor_hash_free;
    result->iterator = (sptensor_iterator_f) sptensor_hash_iterator;
    result->nz_iterator = (sptensor_iterator_f) sptensor_hash_nz_iterator;
	
	/* Allocate the hash table */
    result->hashtable = create_hashtable(NBUCKETS, nmodes);
	
    return (sptensor_t*) result;
}


/* free the memory associated with the */
void sptensor_hash_free(sptensor_hash_t* t) {
    /* free the non-zero poitners */
    if(t->dim) free(t->dim);
    if(t->hashtable) {
		free_hashtable(t->hashtable, t->nbuckets);
		free(t->hashtable);
	}
}


/* Function to insert an element in the hash table.
*/
void sptensor_hash_set(sptensor_hash_t *t, sptensor_index_t *i, mpf_t v) {

	sptensor_hash_item_t *item;

	/* get the hash item */
	item = sptensor_hash_search(t->hashtable, i, t->nbuckets, t->modes);

	/* either set or clear the item */
	if(mpf_cmp_ui(v, 0) != 0) {
		/* mark as present */
		item->flag = 1;

		/* copy the value */
		mpf_set(item->value, v);
	} else {
		/* remove it from the table */
		sptensor_hash_remove(t, item);
	}

	/*printf("Did not insert index.\n");*/
	return;
}


/* Function to retrieve an element in the hash table. */
void sptensor_hash_get(sptensor_hash_t *t, sptensor_index_t *i, mpf_t v)
{
	sptensor_hash_item_t *item;

	/* get the hash item */
	item = sptensor_hash_search(t->hashtable, i, t->nbuckets, t->modes);

	/* set the value */
	if(item->flag) {
		mpf_set(v, item->value);
	} else {
		mpf_set_d(v, 0.0);
	}
}


/* Search the tensor for an index. Return pointer to the item if found, or a hash_item pointer where value=0 if not found.
*/
static struct sptensor_hash_item* sptensor_hash_search(sptensor_hash_item_t *hashtable, sptensor_index_t *idx,int nbuckets, int modes)
{
	struct sptensor_hash_item *ptr;
	mpz_t index;
	unsigned int i;
	mpz_t morton;
	
	/* Init */
	mpz_inits(index, morton, NULL);
	
	/* Compress idx using the morton encoding */
	sptensor_inzt_morton(modes, idx, morton);

	/* mod by number of buckets in hash and get the index */
	mpz_mod_ui(index, morton, nbuckets);
	i = mpz_get_ui(index);


	/*gmp_printf("\n Searching for Key %Zd.\n", index);*/
	while (1) {
		/* set pointer to that index */
		ptr = hashtable + i;
		
		/* we have found the index in the table */
		if (mpz_cmp(ptr->morton,morton) == 0) {
			/*Index exists, return pointer to it */
			/*printf("Index already exists.\n");*/
			break;
		}

		/* this is an empty position */
		if (ptr->flag == 0) {
			/*printf("Index not found. \n");*/
			mpz_set(ptr->morton,morton);
			mpz_set_ui(ptr->key, i);
			sptensor_index_cpy(modes, ptr->idx, idx);
			break;
		}

		/* do linear probing */
		i = (i+1) % nbuckets;
	}

	/* cleanup */
	mpz_clears(morton, index, NULL);

	return ptr;
}


/*Retry to rehash properly */
static void sptensor_hash_rehash(sptensor_hash_t *t) {
	
	struct sptensor_hash_item *ptr;
	sptensor_hash_item_t* new_hashtable;
	sptensor_hash_item_t* old_hashtable;
	int old_hash_size;
	int new_hash_size;
	int i;

	/* Double the number of buckets*/
	new_hash_size = t->nbuckets * 2;
	
	/* Allocate the new table */
    new_hashtable = create_hashtable(new_hash_size, t->modes);
	/* initialize new hash table array */	
	
	/*printf("copying old values to new tensor...\n");*/

	/* save the old hash table */
	old_hash_size = t->nbuckets;
	old_hashtable = t->hashtable;

	/* install the new one */
	t->nbuckets =  new_hash_size;
	t->hashtable = new_hashtable;

	/* Rehash all existing items in t's hashtable to the new table */
	for (i=0; i < old_hash_size; i++) {
		ptr = old_hashtable + i;
		
		/*If occupied, we need to copy it to the other table! */
		if(ptr->flag == 1) {
			sptensor_hash_set(t, ptr->idx, ptr->value);
			/*printf("inserted item into new hashtable.\n");*/
		}
	}

	/*printf("values successfuly copied over.\n");*/
	
	/* Free the old hash table!*/
	free_hashtable(old_hashtable, old_hash_size);
}

/* Remove element from t's hash table at index i with value v*/
static void sptensor_hash_remove(sptensor_hash_t *t, sptensor_hash_item_t *ptr)
{
	int i, j;
	int done;

	/* get the index */
	i = ptr - t->hashtable;

	/* slide back as needed */
	do {
		/* assume we are done */
		done=1;

		/* mark as not present */
		t->hashtable[i].flag = 0;

		/* go to the next probe slot */
		j = (i+1)%j;

		/* check to see if we need to slide back */
		if(t->hashtable[j].flag == 0) {
			continue;
		}

		/* check to see if this one should be pushed back */
		if(mpz_cmp(t->hashtable[i].key, t->hashtable[j].key) == 0) {
			done = 0;
			t->hashtable[i].flag = t->hashtable[j].flag;
			mpz_set(t->hashtable[i].morton, t->hashtable[j].morton);
			mpf_set(t->hashtable[i].value, t->hashtable[j].value);
			sptensor_index_cpy(t->modes, t->hashtable[i].idx, t->hashtable[j].idx);
		}

		/* go on for the next one */
		i=j;
	} while(!done);
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
	/*int count = 0;*/

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
		/*count = count + 1;
		printf("number of items inserted = %d\n", count);*/
    }

    /* cleanup and return! */
    free(idx);
    mpf_clear(val);

    return tns;
}

void sptensor_hash_write(FILE *file, sptensor_hash_t *tns) 
{
	
}

/* create the index iterator */
sptensor_iterator_t* sptensor_hash_iterator(sptensor_t *t)
{
	return sptensor_index_iterator_alloc(t);
}

/* create a non-zero iterator */
sptensor_iterator_t* sptensor_hash_nz_iterator(sptensor_t *t)
{
	return NULL;
}