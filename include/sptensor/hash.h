/*
    This is an implementation of a hash search which operates on vectors.
    It will either find the location of the item or it will find where it
    should be.
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
#ifndef HASH_H
#define HASH_H
#include <sptensor/inzt.h>
#include <sptensor/sptensor.h>
#include <sptensor/vector.h>
#include <gmp.h>

#define NBUCKETS 128

/* Each hash table item has a key and value) */
typedef struct sptensor_hash_item {
	mpz_t morton; /*Morton encoding of item's index*/
    mpz_t key;
    mpf_t value;
	sptensor_index_t *idx;
	int flag;	/*1 for occupied, 0 for unnocupied */
	
} sptensor_hash_item_t;

/* hash storage type */
typedef struct sptensor_hash
{
    /* sptensor fields */
    int modes;
    sptensor_index_t *dim;
    sptensor_get_f get;
    sptensor_set_f set;
    sptensor_iterator_f iterator;
    sptensor_iterator_f nz_iterator;
    sptensor_free_f free;
	
	/*had to add this one until we have a monton-decoding method..*/
	sptensor_index_t *modes_arr;

    /* hash specific fields */
    sptensor_hash_item_t* hashtable;
	unsigned int nbuckets;
	unsigned int hash_curr_size; /*Count of how many items are in the hash table */
	mpz_t num_collisions;
	mpf_t probe_time; /* cumulative time spent probing */
} sptensor_hash_t;


/* sptensor hash allocation functions */
sptensor_t* sptensor_hash_alloc(sptensor_index_t *modes, int nmodes);
void sptensor_hash_free(sptensor_hash_t* t);


/* Function to insert an element in the hash table.
	i - index to insert 
	v - value to insert 
*/
void sptensor_hash_set(sptensor_hash_t *t, sptensor_index_t *i, mpf_t v);


/* Function to retrieve an element in the hash table. */
void sptensor_hash_get(sptensor_hash_t *t, sptensor_index_t *i, mpf_t v);


/* I/O Functions */
sptensor_hash_t * sptensor_hash_read(FILE *file);
void sptensor_hash_write(FILE *file, sptensor_hash_t *tns);

/* Iterator Functions */
sptensor_iterator_t* sptensor_hash_iterator(sptensor_t *t);
sptensor_iterator_t* sptensor_hash_nz_iterator(sptensor_t *t);

#endif
