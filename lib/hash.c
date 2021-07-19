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
#include <time.h>
#include <gmp.h>
#include <sptensor/index_iterator.h>
#include <sptensor/hash.h>
#include <sptensor/inzt.h>

/* Static Helper Functions */
static struct sptensor_hash_item* sptensor_hash_search(sptensor_hash_t *t, sptensor_index_t *idx);
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
	mpz_init(result->num_accesses);
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
	}
}


/* Function to insert an element in the hash table.
*/
void sptensor_hash_set(sptensor_hash_t *t, sptensor_index_t *i, mpf_t v) {

	sptensor_hash_item_t *item;

	/* get the hash item */
	item = sptensor_hash_search(t, i);
	
	/* either set or clear the item */
	if(mpf_cmp_ui(v, 0) != 0) {
		/* mark as present */
		item->flag = 1;

		/* copy the value */
		mpf_set(item->value, v);
		
		/* Increase hashtable count */
		t->hash_curr_size = t->hash_curr_size + 1;
	} else {
		
		/* check if item is present in the table */
		if(item->flag) { 	
		    /* remove it from the table */
			sptensor_hash_remove(t, item); 
		}
	}
	
	/* Check if we need to rehash */
	if((t->hash_curr_size/t->nbuckets) > 0.8) {
		sptensor_hash_rehash(t);
	}

	return;
}


/* Function to retrieve an element in the hash table. */
void sptensor_hash_get(sptensor_hash_t *t, sptensor_index_t *i, mpf_t v)
{
	sptensor_hash_item_t *item;

	/* get the hash item */
	item = sptensor_hash_search(t, i);

	/* set the value */
	if(item->flag) {
		mpf_set(v, item->value);
	} else {
		mpf_set_d(v, 0.0);
	}
}


/* Search the tensor for an index. Return pointer to the item if found, or a hash_item pointer where value=0 if not found.
*/
static struct sptensor_hash_item* sptensor_hash_search(sptensor_hash_t *t, sptensor_index_t *idx)
{
	
	/*To measure probe time*/
	clock_t start, end;
    double cpu_time_used;
	
	struct sptensor_hash_item *ptr;
	mpz_t index;
	unsigned int i;
	mpz_t morton;
	
	/* Init */
	mpz_inits(index, morton, NULL);
	
	/* Compress idx using the morton encoding */
	sptensor_inzt_morton(t->modes, idx, morton);

	/* mod by number of buckets in hash and get the index */
	mpz_mod_ui(index, morton, t->nbuckets);
	i = mpz_get_ui(index);

	/* count the accesses */
	mpz_add_ui(t->num_accesses, t->num_accesses, 1);

	start = clock();
	
	while (1) {

		/* set pointer to that index */
		ptr = t->hashtable + i;
		
		/* we have found the index in the table */
		if (mpz_cmp(ptr->morton,morton) == 0) {
			break;
		}

		/* this is an empty position */
		if (ptr->flag == 0) {
			mpz_set(ptr->morton,morton);
			mpz_set_ui(ptr->key, i);
			sptensor_index_cpy(t->modes, ptr->idx, idx);
			break;
		}

		/* do linear probing */
		mpz_add_ui(t->num_collisions, t->num_collisions, 1);
		i = (i+1) % t->nbuckets;
	}

	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	mpf_add_ui(t->probe_time, t->probe_time, cpu_time_used);
	
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
		}
	}
	
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
	j = i+1;

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

void sptensor_hash_clear(sptensor_hash_t *t) {
	
	struct sptensor_hash_item *ptr;
	int i;
	
	for(i=0;i<t->nbuckets;i++) {
		ptr = t->hashtable + i;
		ptr->flag = 0;
	}
	
}

unsigned int sptensor_hash_nnz(sptensor_hash_t *t) {
	return t->hash_curr_size;
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
	char buf[32];
	
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
	    if(count%1000000==0) {
	    printf("number of items inserted = %d\n", count); }
	    /*if(count == 3000) {break;}*/
    }

    /* cleanup and return! */
    free(idx);
    mpf_clear(val);

    return tns;
}

/*
 * Write a sparse tensor to a stream.  It is written in the format 
 * described in the sptensor_read function.
 * 
 * Parameter: file - The stream to write to.
 *            tns  - The tensor to write.
 */ 
void sptensor_hash_write(FILE *file, sptensor_hash_t *tns)
{
    int i;
    mpf_t val;
    sptensor_iterator_t *itr;

    /* allocate the value */
    mpf_init(val);
    
    /* print the preamble */
    gmp_fprintf(file, "%u", tns->modes);
    for(i = 0; i < tns->modes; i++) {
	    gmp_fprintf(file, "\t%u", tns->dim[i]);
    }
    gmp_fprintf(file, "\n");

    /* print the non-zero values */
    for(itr=sptensor_nz_iterator(tns); sptensor_iterator_valid(itr); sptensor_iterator_next(itr)) {
        /* print the index */
	    for(i = 0; i < tns->modes; i++) {
	        gmp_fprintf(file, "%u\t", itr->index[i]);
	    }

        /* print the value */
        sptensor_get(tns, itr->index, val);
	    gmp_fprintf(file, "%Ff\n", val);
    }

    /* cleanup */
    sptensor_iterator_free(itr);
    mpf_clear(val);
}

/* create the index iterator */
sptensor_iterator_t* sptensor_hash_iterator(sptensor_hash_t *t)
{
	return sptensor_index_iterator_alloc(t);
}


/* sptensor iterator (base type) */
struct sptensor_hash_nz_iterator
{
	/* iterator fields */
    sptensor_t *t;
    sptensor_index_t *index;
    sptensor_iterator_valid_f valid;
    sptensor_iterator_next_f next;
    sptensor_iterator_prev_f prev;
    sptensor_iterator_get_f get;
    sptensor_iterator_set_f set;
    sptensor_free_f free;

	/* my one special field */
	int i;
};


static void hash_nz_seek_forward(struct sptensor_hash_nz_iterator *itr)
{
	sptensor_hash_t *t = (sptensor_hash_t*) itr->t;

	/* find the filled slot, or let it blow up */
	while(itr->i < t->nbuckets && t->hashtable[itr->i].flag == 0) {
		itr->i++;
	}

	if(itr->i < t->nbuckets) {
		sptensor_index_cpy(t->modes, itr->index, t->hashtable[itr->i].idx);
	}
}


static void hash_nz_seek_backward(struct sptensor_hash_nz_iterator *itr)
{
	sptensor_hash_t *t = (sptensor_hash_t*) itr->t;

	/* find the filled slot, or let it blow up */
	while(itr->i > 0 && t->hashtable[itr->i].flag == 0) {
		itr->i--;
	}

	if(itr->i > 0) {
		sptensor_index_cpy(t->modes, itr->index, t->hashtable[itr->i].idx);
	}
}


static int hash_nz_next(struct sptensor_hash_nz_iterator* itr)
{
	/* increment and then find the next filled spot */
	itr->i++;
	hash_nz_seek_forward(itr);
	return sptensor_iterator_valid((sptensor_iterator_t*) itr);
}


static int hash_nz_prev(struct sptensor_hash_nz_iterator* itr)
{
	/* decrement and then find the next filled spot */
	itr->i--;
	hash_nz_seek_backward(itr);
	return sptensor_iterator_valid(itr);
}


static int hash_nz_valid(struct sptensor_hash_nz_iterator* itr)
{
	return itr->i >= 0 && itr->i < ((sptensor_hash_t*)itr->t)->nbuckets;
}


/* create a non-zero iterator */
sptensor_iterator_t* sptensor_hash_nz_iterator(sptensor_hash_t *t)
{
	struct sptensor_hash_nz_iterator *itr;

	/* allocate the iterator */
	itr = malloc(sizeof(struct sptensor_hash_nz_iterator));
	if(!itr) return NULL;

	/* allocate the index */
	itr->index = sptensor_index_alloc(t->modes);
	if(!itr->index) {
		sptensor_index_iterator_free((sptensor_iterator_t*) itr);
		return NULL;
	}

	/* populate the fields */
	itr->t = (sptensor_t *) t;
	itr->free = (sptensor_free_f) sptensor_index_iterator_free;
	itr->next = (sptensor_iterator_next_f) hash_nz_next;
	itr->prev = (sptensor_iterator_prev_f) hash_nz_prev;
	itr->valid = (sptensor_iterator_valid_f) hash_nz_valid;

    /* TODO: Make an effecient hash version of these */
    itr->get = sptensor_index_iterator_get;
    itr->set = sptensor_index_iterator_set;

	/* get to the first position */
	itr->i=0;
	hash_nz_seek_forward(itr);

	return (sptensor_iterator_t*) itr;
}
