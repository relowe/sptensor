 /*
	This is a collection of functions dealing with the succint binary tree.

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

#include <sptensor/sptensor.h>

#ifndef SPTENSOR_SKBT_H
#define SPTENSOR_SKBT_H

/* COO storage type */
typedef struct sptensor_skbt
{
    /* sptensor fields */
    int modes;
    sptensor_index_t *dim;
    sptensor_get_f get;
    sptensor_set_f set;
    sptensor_iterator_f iterator;
    sptensor_iterator_f nz_iterator;
    sptensor_free_f free;

    /* skbt specific fields */
    mpf_t* tree_values;
    mpf_t* tree_leaf_values;
    mpz_t* tree_bitmap;
	unsigned int num_dimensions;
	unsigned int num_non_zeros;
	unsigned int* d_sizes;
} sptensor_skbt_t;

/* SKBT allocation functions */
sptensor_t* sptensor_skbt_alloc(sptensor_index_t *modes, int nmodes);
void sptensor_skbt_free(sptensor_skbt_t* t);

/* SKBT element access functions */
void sptensor_skbt_get(sptensor_skbt_t* t, sptensor_index_t *i, mpf_t v);
void sptensor_skbt_set(sptensor_skbt_t* t, sptensor_index_t *i, mpf_t v);

/* SKBT functions */
void sptensor_skbt_print_tree(sptensor_skbt_t* t);
void sptensor_skbt_print_bitmap(sptensor_skbt_t* t);
void sptensor_skbt_save_raw_bitmap_to_file(sptensor_skbt_t* t, char file_name[]);
void sptensor_skbt_make_tree(sptensor_t* a, sptensor_t* b, int num_dimensions, int* d_sizes, int num_non_zeros);

/* Iterator Functions */
sptensor_iterator_t* sptensor_skbt_iterator(sptensor_skbt_t *t);
sptensor_iterator_t* sptensor_skbt_nz_iterator(sptensor_skbt_t *t);

#endif  
