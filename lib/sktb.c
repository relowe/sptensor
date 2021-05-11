/*
  This is a collection of functions dealing with the succint k-binary tree.
  
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <gmp.h>
#include <sptensor/sktb.h>
#include <sptensor/coo.h>
#include <sptensor/index_iterator.h>
#include <sptensor/inzt.h>

/* nonzero iterator */
/* sptensor iterator (base type) */
struct sptensor_sktb_nz_iterator{
	sptensor_t* t;
	sptensor_index_t* index;
	sptensor_iterator_valid_f valid;
	sptensor_iterator_next_f next;
	sptensor_iterator_prev_f prev;
    sptensor_iterator_get_f get;
    sptensor_iterator_set_f set;
	sptensor_free_f free;

	int ti;
};

/* 
 * Allocate memory for all required members in the sptensor_sktb_t struct
 * 
 * Parameter: modes - the size of each dimension 
 *            nmodes - number of modes(dimensions) as int
 *  
 * Return:  pointer to a sptensor_sktb_t struct with all initialized members
 */
sptensor_t* sptensor_sktb_alloc(sptensor_index_t *modes, int nmodes){
	sptensor_sktb_t* sptensor_sktb;
	sptensor_sktb = malloc(sizeof(sptensor_sktb_t));
	
	sptensor_sktb-> get = (sptensor_get_f) sptensor_sktb_get;
	sptensor_sktb-> set = NULL;
	sptensor_sktb-> iterator = (sptensor_iterator_f) sptensor_sktb_iterator;
	sptensor_sktb-> nz_iterator = (sptensor_iterator_f) sptensor_sktb_nz_iterator; 	
	
	sptensor_sktb->tree_values = (mpf_t*) malloc(pow(2, nmodes*ceil(log10(10) / log10(2))) *  sizeof(mpf_t));
	sptensor_sktb->tree_leaf_values = (mpf_t*) malloc(pow(2, nmodes*ceil(log10(10) / log10(2))) *  sizeof(mpf_t));
	/* Add a loop that inits each value of tra_leaf_values */
	int i;
	for(i = 0; i < nmodes*ceil(log10(10) / log10(2)); i++){
		mpf_init(sptensor_sktb->tree_values[i]);
		mpf_init(sptensor_sktb->tree_leaf_values[i]);
	}
	sptensor_sktb->tree_bitmap = (mpz_t*) malloc(sizeof(mpz_t));
	sptensor_sktb->modes = nmodes;
	sptensor_sktb->d_sizes = (int*) calloc(nmodes* 2, sizeof(int));
		
	return (sptensor_t*) sptensor_sktb;
}


/* 
 * Deallocate all the memory that was allocated in the sptensor_sktb_alloc method
 * 
 * Parameter: modes - the size of each dimension
 *            nmodes - number of modes(dimensions) as int
 *  
 * Return:  none
 */
void sptensor_sktb_free(sptensor_sktb_t* t){
	free(t->tree_values);
	free(t->tree_leaf_values);
	free(t->tree_bitmap);
	free(t->d_sizes);
	free(t);
	int i;
	for(i = 0; i < t->modes*ceil(log10(10) / log10(2)); i++){
		mpf_clear(t->tree_values[i]);
		mpf_clear(t->tree_leaf_values[i]);
	}
}

/* 
 * Get the value from the tree at desired index if exists otherwise get 0
 * 
 * Parameter: t - pointer to the sptensor_sktb_t tensor from which the value needs to be gotten
 *            i - n d array of indices for each dimension where n is the number of dimensions
 *            v - this is the mpf_t which will receive the value from the tree ath the specified index
 *  
 * Return:  none
 */
void sptensor_sktb_get(sptensor_sktb_t* t, sptensor_index_t *i, mpf_t v){
	unsigned int current_index = 0;
	unsigned int dim_no = 0;
	bool found = 0;
	while(!found){
		if( (mpz_tstbit(*(t->tree_bitmap), current_index * 2 == 1)) && (mpz_tstbit(*(t->tree_bitmap), current_index * 2 + 1 == 1))){
			mpf_set(v, t->tree_values[current_index]);
			found = true;
		}else{
			unsigned int split_midpoint = (unsigned int) mpf_get_d(t->tree_values[current_index]);
			if(split_midpoint <= i[dim_no]){
				current_index = current_index * 2 + 1;
			}else{
				current_index = current_index * 2 + 2;
			}
			if(dim_no == t->modes){
				dim_no = 0;
			}else{
				dim_no++;
			}
		}
	}
}

void sptensor_sktb_set(sptensor_sktb_t* t, sptensor_index_t *i, mpf_t v){
	
}


/* 
 * Print the succint k-binary tree (including midpoints and values)
 * in a pretty format
 * 
 * Parameter: t - pointer to the sptensor_sktb struct that contains all required members
 *  
 * Return:  none
 */
void sptensor_sktb_print_tree(sptensor_sktb_t* t){
	printf("The tree: \n");
    int k = 0;
	int i;
    for(i = 0; i < t->modes * ceil(log10(10) / log10(2)) ; i++){
		int j;
        for(j =0; j < pow(2,i); j++){
            if (mpf_cmp_ui(t->tree_values[k], -1) == 0){
                printf("    \t");
            }else{
				gmp_printf("%.*Ff", 2, t->tree_values[k]);
				printf("\t");
            }
		k++;
        }
        printf("\n");
    }
}

/* 
 * Print the succint k-binary tree bitmap to the console
 * 
 * Parameter: t - pointer to the sptensor_sktb struct that contains all required members
 *  
 * Return:  None
 */
void sptensor_sktb_print_bitmap(sptensor_sktb_t* t){
	printf("The bitmap is: ");
	mpz_out_str(stdout, 2, *(t->tree_bitmap));
	printf("\n");
}

/* 
 * Save the succint k-binary tree bitmap to a file in raw format
 *
 * Parameter: t - pointer to the sptensor_sktb struct that contains all required members
 *            file_name - the name of the file to write to
 *  
 * Return:  None
 */
void sptensor_sktb_save_raw_bitmap_to_file(sptensor_sktb_t* t, char file_name[]){
	FILE* file = fopen(file_name, "w");
	mpz_out_raw(file, *(t->tree_bitmap));
	fclose(file);
}


/* 
 * Helper: Decide whether to expand a subtree or not
 * 
 * Parameter: t - pointer to the sptensor_sktb struct that contains all required members
 *            tensor_coo   - the tensor in coo format
 *            bounds   - the bounds (highs and lows for each dimension at this point)
 *            direction   - "l" or "r" which direction to expand
 *            dim_no    - the current dimension that the tree wants to split
 *  
 * Return:  true if should expand (sub tree is not empty)
 *          false if shouldn't expand (sub tree is empty)
 */
static bool sptensor_sktb_should_expand(sptensor_t* a, sptensor_t* b, int* bounds, char direction, int dim_no){
    if(direction != 'l' && direction !='r'){
        printf("direction should be either 'l' or 'r'");
        return false;
    }

	sptensor_iterator_t* itr = sptensor_index_iterator_alloc(b);
	sptensor_inzt_t* inzt = sptensor_inzt_create(itr);

	sptensor_index_t* min = sptensor_index_alloc(b->modes);
	sptensor_index_t* max = sptensor_index_alloc(b->modes);
	int i;
	for(i = 0; i < a->modes; i++){
		min[i] = bounds[i*2];
	}
	for(i = 0; i< a->modes; i++){
		max[i] = bounds[i*2+1];
	}

	if(sptensor_inzt(inzt, min, max) == 1){
		return true;
	}else{
		return false;
	}

	sptensor_iterator_free(itr);
	sptensor_inzt_free(inzt);
	sptensor_index_free(min);
	sptensor_index_free(max);
}

/* 
 * After splitting midpoints, add the values of the coo to the bottom of sktb tree 
 * 
 * Parameter: t - pointer to the sptensor_sktb struct that contains all required members
 *            a - the sktb tensor to add values to 
 *            b - the coo tensor to add values from
 *  
 * Return:  None
 */
static void sptensor_sktb_add_values(sptensor_t* a, sptensor_t* b){
	sptensor_sktb_t* t = (sptensor_sktb_t*) a;
	sptensor_coo_t* tensor_coo = (sptensor_coo_t*) b;
	sptensor_iterator_t* itr = sptensor_nz_iterator(b);
	mpf_t v;
	mpf_init(v);
	int i = 0;
	while(sptensor_iterator_valid(itr)){
		sptensor_get(itr->t, itr->index, v);
		mpf_set(t->tree_leaf_values[i], v);
		i++;
		sptensor_iterator_next(itr);
	}
	mpf_clear(v);
	sptensor_iterator_free(itr);
}

/* 
 * Add a midpoint to a desired index in the kdtree (split dimension)
 * based on bounds given
 * 
 * Parameter: t - pointer to the sptensor_sktb struct that contains all required members
              tensor_coo - the tensor in coo format
 *            dim_no   - the current dimension that the tree will split
 *            index   - the current index in the values array at which the midpoint/non-zero will be added
 *            bounds   - the bounds(for each dimension) to use to get the midpoint
 *  
 * Return:  None
 */
static void sptensor_sktb_add_mid(sptensor_t* a, sptensor_t* b, unsigned int dim_no, unsigned int index, int* bounds, unsigned int bit_index){
	sptensor_sktb_t* t = (sptensor_sktb_t*) a;
	sptensor_coo_t* tensor_coo = (sptensor_coo_t*) b;
    /*Calculate high and low and if the next child would be a leaf*/
    int high = bounds[(dim_no*2)+1];
    int low = bounds[(dim_no*2)];
	int i;
	bool no_more_range = true;
	for(i = 0; i < t->modes; i++){
		if(bounds[2*i] != bounds[1+(2*i)]){
			no_more_range = false;
		} 
	}
	if(no_more_range == true){
		sptensor_sktb_add_values(a, b);
		return;
	}

    /*Add midpoint to current index*/
	mpf_set_ui(t->tree_values[index], (bounds[(dim_no*2)] + bounds[(dim_no*2)+1]) / 2);

    /*Expand tree left if it should*/
    if(sptensor_sktb_should_expand(a, b, bounds, 'l', dim_no == t->modes - 1 ? 0 : dim_no + 1)){
        int* boundsl = (int*)calloc(t->modes*2, sizeof(int));
        memcpy(boundsl, bounds, t->modes*2*sizeof(int));
        boundsl[(dim_no*2)+1] = (boundsl[dim_no*2] + boundsl[(dim_no*2)+1]) / 2;
        sptensor_sktb_add_mid ((sptensor_t*) t, (sptensor_t*)tensor_coo, dim_no == t->modes - 1 ? 0 : dim_no + 1, 2 * (index) + 1, boundsl, 2*((2*bit_index)+1));
        free(boundsl);
		mpz_setbit(*(t->tree_bitmap), bit_index);
    }

    /*Expand tree right if it should*/
    if(sptensor_sktb_should_expand(a, b, bounds, 'r', dim_no == t->modes - 1 ? 0 : dim_no + 1)){
        int* boundsr = (int*)calloc(2*t->modes, sizeof(int));
        memcpy(boundsr, bounds, t->modes*2*sizeof(int));
        boundsr[(dim_no*2)] = ((boundsr[(dim_no*2)] + boundsr[(dim_no*2)+1]) / 2) + 1;
        sptensor_sktb_add_mid((sptensor_t*) t, (sptensor_t*)tensor_coo, dim_no == t->modes - 1 ? 0 : dim_no + 1, 2 * (index) + 1 + 1, boundsr, 2*((2*bit_index)+1));
     	free(boundsr);
		mpz_setbit(*(t->tree_bitmap), bit_index+1);
    }
 }

/* 
 * Make the succint k-binary tree tree for the tensor 
 * 
 * Parameter: t - 
 *            a   - The left hand index being compared
 *            b   - The right hand index being compared
 *  
 * Return:  None
 */
void sptensor_sktb_maketree(sptensor_t* a, sptensor_t* b, int num_dimensions, int* d_sizes, int num_non_zeros){

	sptensor_sktb_t* t = (sptensor_sktb_t*) a;
	sptensor_coo_t* tensor_coo = (sptensor_coo_t*) b; 

	t->d_sizes = d_sizes;
	t->num_non_zeros = num_non_zeros;
	
    /*Get longest dimension*/
    int nMax = 0;
	int i;
    for(i = 0; i < t->modes; i++){
        if(t->d_sizes[i] > nMax){
            nMax = t->d_sizes[i];
        }
    }

    /* Initialize all nonzeros to -1*/
    for(i = 0; i < pow(2, t->modes * ceil(log10(10) / log10(2)) ); i++){
		mpf_init(t->tree_values[i]);
		mpf_set_ui(t->tree_values[i], -1);
    }
	
	mpz_init(*(t->tree_bitmap));
	mpz_set_ui(*(t->tree_bitmap), 0);

    /* fill bounds with highs and lows */
    int* bounds = (int*)calloc(t->modes*2, sizeof(int));
    for(i = 0; i < t->modes; i++){
        bounds[(i*2)+1] = t->d_sizes[i] - 1;
    }
	
    sptensor_sktb_add_mid((sptensor_t*)t, (sptensor_t*)tensor_coo, 0, 0, bounds, 0);
	free(bounds);
}

/* Iterator functions */
sptensor_iterator_t* sptensor_sktb_iterator(sptensor_sktb_t* t){
	return sptensor_index_iterator_alloc((sptensor_t*) t);
}

/* Non zero iterator functions */
static int sptensor_sktb_nz_valid(struct sptensor_sktb_nz_iterator* itr){
	struct sptensor_sktb* t = (struct sptensor_sktb*) itr->t;


	if(itr->ti >=0 && mpz_tstbit(*(t->tree_bitmap), itr->ti) == 0){
		itr->ti++;
		if(mpz_tstbit(*(t->tree_bitmap), itr->ti) == 0){
			itr->ti--;
			return 1;
		}
	}else{
		return 0;
	}
}

static void sptensor_sktb_nz_load_index(struct sptensor_sktb_nz_iterator* itr){
	struct sptensor_sktb* t = (struct sptensor_sktb*) itr->t;
	if(sptensor_sktb_nz_valid(itr)) {
		/*find how many levels to reach parent*/
		unsigned int n_levels = 0;
		int index = itr->ti;
		while(index > 0){
			if((index / 2 - 1) % 2 == 0){
				index = index / 2 - 1;
				n_levels++;
			}else{
				index = index / 2 - 2;
				n_levels++;
			}
		}
		/*trace path back to the parent, store it in array (left ,right)*/
		bool* path_bottom_to_top = malloc(n_levels * sizeof(bool));
		index = itr->ti;
		int i = 0;
		while(index > 0){
			if((index / 2 - 1) % 2 == 0){
				index = index / 2 - 1;
				path_bottom_to_top[i] = 0;
				i++;
			}else{
				index = index / 2 - 2;
				path_bottom_to_top[i] = 1;
				i++;
			}
		}
		/*trace path from parent to the value in tree_values, keep track of dimensions with each one, */
		unsigned int* constraints = malloc(t->modes * sizeof(unsigned int));
		
		int dim_no = 0;
		unsigned int current_index = 0;
		for(i = n_levels-1; i>=0; i++){
			int temp = (int) mpf_get_d(t->tree_values[current_index]);
			constraints[dim_no] = temp;
			if(path_bottom_to_top[i] == 0){
				current_index = 2*(current_index) + 1;
			}else if(path_bottom_to_top[i] == 1){
				current_index = 2*(current_index) + 2;
			}
			if(dim_no == t->modes - 1){
				dim_no = 0;
			}else{
				dim_no++;
			}
		}
		/*load the dimension index into the itr->index*/		
		sptensor_index_cpy(t->modes, itr->index, constraints);
    }
}

static void sptensor_sktb_nz_free(struct sptensor_sktb_nz_iterator* itr){
	sptensor_index_free(itr->index);
	free(itr);
}

static int sptensor_sktb_nz_next(struct sptensor_sktb_nz_iterator* itr){
	if(sptensor_sktb_nz_valid(itr)){
		itr->ti++;
	}

	sptensor_sktb_nz_load_index(itr);

	return sptensor_sktb_nz_valid(itr);
}

static int sptensor_sktb_nz_prev(struct sptensor_sktb_nz_iterator* itr){
	if(sptensor_sktb_nz_valid(itr)){
		itr->ti--;
	}

	sptensor_sktb_nz_load_index(itr);

	return sptensor_sktb_nz_valid(itr);
}

sptensor_iterator_t*  sptensor_sktb_nz_iterator(sptensor_sktb_t* t){
	struct sptensor_sktb_nz_iterator* itr;

	itr = malloc(sizeof(struct sptensor_sktb_nz_iterator));
	if(!itr){
		return NULL;
	}

	itr->index = sptensor_index_alloc(t->modes);
	if(!itr->index){
		sptensor_sktb_nz_free(itr);
		return NULL;
	}

	itr->ti = 0;

	itr->valid = (sptensor_iterator_valid_f) sptensor_sktb_nz_valid;
	itr->next = (sptensor_iterator_next_f) sptensor_sktb_nz_next;
	itr->prev = (sptensor_iterator_prev_f) sptensor_sktb_nz_prev;
	itr->free = (sptensor_free_f) sptensor_sktb_nz_free;
	itr->t = (sptensor_t *) t;

    /* TODO: Make an effecient SKTB version of these */
    itr->get = sptensor_index_iterator_get;
    itr->set = sptensor_index_iterator_set;

	sptensor_sktb_nz_load_index(itr);

	return (sptensor_iterator_t* ) itr;
}
