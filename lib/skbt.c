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

#include <sptensor/skbt.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <gmp.h>

/* nonzero iterator */
/* sptensor iterator (base type) */
struct sptensor_skbt_nz_iterator{
	sptensor_t* t;
	sptensor_index* index;
	sptensor_iterator_valid_f valid;
	sptensor_iterator_next_f next;
	sptensor_iterator_prev_f prev;
	sptensor_free_f free;

	int ti;
}

/* 
 * Allocate memory for all required members in the sptensor_skbt_t struct
 * 
 * Parameter: modes - pointer to the modes field in the sptensor_skbt_t struct
 *            nmodes - number of modes(dimensions) as int
 *  
 * Return:  pointer to a sptensor_skbt_t struct with all initialized members
 */
sptensor_skbt_t* sptensor_skbt_alloc(sptensor_index_t *modes, int nmodes){
	sptensor_skbt_t* sptensor_skbt;
	sptensor_skbt = malloc(sizeof(sptensor_skbt_t));
	
	sptensor_skbt-> get = (sp_tensor_get_f) sptensor_skbt_get;
	sptensor_skbt-> set = null;
	
	sptensor_skbt->tree_values = (mpf_t*) malloc(pow(2, sptensor_skbt.modes*ceil(log10(10) / log10(2))) *  sizeof(mpf_t));
	sptensor_skbt->tree_bitmap = (mpz_t*) malloc(sizeof(mpz_t));
	sptensor_skbt->modes = nmodes;
	sptensor_skbt->d_sizes = (int*) calloc(nmodes* 2 * sizeof(int));
		
	return sptensor_skbt;
}


/* 
 * Deallocate all the memory that was allocated in the sptensor_skbt_alloc method
 * 
 * Parameter: modes - pointer to the modes field in the sptensor_skbt_t struct
 *            nmodes - number of modes(dimensions) as int
 *  
 * Return:  none
 */
void sptensor_skbt_free(sptensor_skbt_t* t){
	free(t->tree_values);
	free(t->tree_bitmap);
	free(t->d_sizes);
	free(t);
}

/* 
 * Get the value from the tree at desired index if exists otherwise get 0
 * 
 * Parameter: t - pointer 
 *            nmodes - number of modes(dimensions) as int
 *  
 * Return:  none
 */
void sptensor_skbt_get(sptensor_skbt_t* t, sptensor_index_t *i, mpf_t v){
	if(mpz_tstbit(*(t->tree_bitmap), *i) == 0){
		mpf_set_ui(v, 0);
	}else{
		mpf_set(v, t->tree_values[*i]);
	}
}

void sptensor_skbt_set(sptensor_skbt_t* t, sptensor_index_t *i, mpf_t v){
	
}


/*
	static function prototypes
*/
static bool sptensor_skbt_should_expand();
static void sptensor_skbt_add_mid();


/* 
 * Print the succint k-binary tree (including midpoints and values)
 * in a pretty format
 * 
 * Parameter: t - pointer to the sptensor_skbt struct that contains all required members
 *  
 * Return:  none
 */
void sptensor_skbt_print_tree(sptensor_skbt_t* t){
	printf("The tree: \n");
    int k = 0;
    for(int i = 0; i < t->modes * ceil(log10(10) / log10(2)) ; i++){
        for(int j =0; j < pow(2,i); j++){
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
 * Parameter: t - pointer to the sptensor_skbt struct that contains all required members
 *  
 * Return:  None
 */
void sptensor_skbt_print_bitmap(sptensor_skbt_t* t){
	printf("The bitmap is: ");
	mpz_out_str(stdout, 2, *(t->tree_bitmap));
	printf("\n");
}

/* 
 * Save the succint k-binary tree bitmap to a file in raw format
 *
 * Parameter: t - pointer to the sptensor_skbt struct that contains all required members
 *            file_name - the name of the file to write to
 *  
 * Return:  None
 */
void sptensor_skbt_save_raw_bitmap_to_file(sptensor_skbt_t* t, char file_name[]){
	FILE* file = fopen(file_name, "w");
	mpz_out_raw(file, *(t->tree_bitmap));
	fclose(file);
}


/* 
 * Helper: Decide whether to expand a subtree or not
 * 
 * Parameter: t - pointer to the sptensor_skbt struct that contains all required members
 *            tensor_coo   - the tensor in coo format
 *            bounds   - the bounds (highs and lows for each dimension at this point)
 *            direction   - "l" or "r" which direction to expand
 *            dim_no    - the current dimension that the tree wants to split
 *  
 * Return:  true if should expand (sub tree is not empty)
 *          false if shouldn't expand (sub tree is empty)
 */
static bool sptensor_skbt_should_expand(sptensor_skbt_t* t, int* tensor_coo, int* bounds, char direction, int dim_no){
    if(direction != 'l' && direction !='r'){
        printf("direction should be either 'l' or 'r'");
        return false;
    }
    /* for(int i = 0; i < num_non_zeros; ){
    	for(int d  = 0; d < t->modes; d++){

        }
     }*/
	return true;  
}


/* 
 * Add a midpoint to a desired index in the kdtree (split dimension)
 * based on bounds given
 * 
 * Parameter: t - pointer to the sptensor_skbt struct that contains all required members
              tensor_coo - the tensor in coo format
 *            dim_no   - the current dimension that the tree will split
 *            index   - the current index in the values array at which the midpoint/non-zero will be added
 *            bounds   - the bounds(for each dimension) to use to get the midpoint
 *  
 * Return:  None
 */
static void sptensor_skbt_add_mid(sptensor_skbt_t* t, sptensor_coo_t* tensor_coo, unsigned int dim_no, unsigned int index, int* bounds, unsigned int bit_index){
    /*Calculate high and low and if the next child would be a leaf*/
    int high = bounds[(dim_no*2)+1];
    int low = bounds[(dim_no*2)];
    if(high == low){
        return;
    }

    /*Add midpoint to current index*/
	mpf_set_ui(t->tree_values[index], (bounds[(dim_no*2)] + bounds[(dim_no*2)+1]) / 2);
	// mpz_setbit(t->*(tree_bitmap), index);

    /*Expand tree left if it should*/
    if(sptensor_skbt_should_expand(tensor_coo, bounds, 'l', dim_no == t->modes - 1 ? 0 : dim_no + 1)){
        int* boundsl = (int*)calloc(t->modes*2, sizeof(int));
        memcpy(boundsl, bounds, t->modes*2*sizeof(int));
        boundsl[(dim_no*2)+1] = (boundsl[dim_no*2] + boundsl[(dim_no*2)+1]) / 2;
        sptensor_skbt_add_mid (t, tensor_coo, dim_no == t->modes - 1 ? 0 : dim_no + 1, 2 * (index) + 1, boundsl, 2*((2*bit_index)+1));
        free(boundsl);
		mpz_setbit(t->*(tree_bitmap), bit_index);
    }

    /*Expand tree right if it should*/
    if(sptensor_skbt_should_expand(tensor_coo, bounds, 'r', dim_no == t->modes - 1 ? 0 : dim_no + 1)){
        int* boundsr = (int*)calloc(2*t->modes, sizeof(int));
        memcpy(boundsr, bounds, t->*2*sizeof(int));
        boundsr[(dim_no*2)] = ((boundsr[(dim_no*2)] + boundsr[(dim_no*2)+1]) / 2) + 1;
        sptensor_skbt_add_mid(t, tensor_coo, dim_no == sptensor_skbt.modes - 1 ? 0 : dim_no + 1, 2 * (index) + 1 + 1, boundsr, 2*((2*bit_index)+1));
     	free(boundsr);
		mpz_setbit(t->*(tree_bitmap), bit_index+1);
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
void sptensor_skbt_maketree(sptensor_skbt_t* t, sptensor_coo_t* tensor_coo, int* d_sizes, int num_non_zeros){
	
	t->d_sizes = d_sizes;
	t->num_non_zeros = num_non_zeros;
	
    /*Get longest dimension*/
    int nMax = 0;
    for(int i = 0; i < t->modes; i++){
        if(t->d_sizes[i] > nMax){
            nMax = t->d_sizes[i];
        }
    }

    /* Initialize all nonzeros to -1*/
    for(int i = 0; i < pow(2, t->modes * ceil(log10(10) / log10(2)) ); i++){
		mpf_init(t->tree_values[i]);
		mpf_set_ui(t->tree_values[i], -1);
    }
	
	mpz_init(*(t->tree_bitmap));
	mpz_set_ui(*(t->tree_bitmap), 0);

    /* fill bounds with highs and lows */
    int* bounds = (int*)calloc(t->modes*2, sizeof(int));
    for(int i = 0; i < t->modes; i++){
        bounds[(i*2)+1] = t->d_sizes[i] - 1;
    }
	
    sptensor_skbt_add_mid(t, tensor_coo, 0, 0, bounds, 0);
	free(bounds);
}

/* Iterator functions */
sptensor_iterator_t* sptensor_skbt_iterator(sptensor_skbt_t* t){
	return sptensor_index_iterator_alloc((sptensor_t* t) t);
}

/* Non zero iterator functions */
static int sptensor_skbt_nz_valid(struct sptensor_skbt_nz_iterator* itr){
	struct sptensor_skbt* t = (struct sptensor_skbt* t) itr->t;


	if(itr->ti >=0 && mpz_tstbit(*(itr->t->tree_bitmap), itr->ti) == 0){
		itr->ti++;
		if(itr->mpz_tstbit(*(itr-t->tree_bitmap), itr->ti) == 0){
			itr->ti--;
			return 1;
		}
	}else{
		return 0;
	}
}

static void sptensor_skbt_nz_load_index(struct sptensor_skbt_nz_iterator* itr){
	struct sptensor_skbt* t = (struct sptensor_skbt);
	if(sptensor_coo_nz_valid(itr)) {
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
		unsigned int* constraints = malloc(itr->t->modes * sizeof(unsigned int));
		
		int dim_no = 0;
		unsigned int current_index = 0;
		for(int i = levels-1; i>=0; i++){
			int temp = mpf_get_d(itr->t->tree_values[current_index]));
			constraints[dim_no] = temp;
			if(path_bottom_to_top[i] == 0){
				current_index = 2*(current_index) + 1;
			}else if(path_bottom_to_top[i] == 1){
				current_index = 2*(current_index) + 2;
			}
			if(dim_no == itr->t->modes - 1){
				dim_no = 0;
			}else{
				dim_no++;
			}
		}		
		/*load the dimension index into the itr->index*/
		for(int i = 0; i < itr->modes; i++){
			itr->index[i] = constraints[i];
		}
    }
}

static void sptensor_skbt_nz_free(struct sptensor_skbt_nz_iterator* itr){
	sptensor_index_free(itr->index);
	free(itr);
}

static void sptensor_skbt_nz_next(struct sptensor_skbt_nz_iterator* itr){
	if(sptensor_skbt_nz_valid(itr)){
		itr->ti++;
	}

	sptensor_skbt_nz_load_index(itr);

	return sptensor_skbt_nz_valid(itr);
}

static void sptensor_skbt_nz_prev(struct sptensor_skbt_nz_iterator* itr){
	if(sptensor_skbt_nz_valid(itr)){
		itr->ti--;
	}

	sptensor_skbt_nz_load_index(itr);

	return sptensor_skbt_nz_valid(itr);
}

sptensor_iterator_t*  sptensor_skbt_nz_iterator(sptensor_skbt_t* t){
	struct sptensor_iterator* itr;

	itr = malloc(sizeof(struct sptensor_skbt_nz_iterator));
	if(!itr){
		return NULL;
	}

	itr->index = sptensor_index_alloc(t->modes);
	if(!itr->index){
		sptensor_skbt_nz_free(itr);
		return NULL;
	}

	itr->ti = 0;

	itr->valid = (sptensor_iterator_valid_f) sptensor_skbt_nz_valid;
	itr->next = (sptensor_iterator_next_f) sptensor_skbt_nz_next;
	itr->prev = (sptensor_iterator_prev_f) sptensor_skbt_nz_prev;
	itr->free = (sptensor_free_f) sptensor_skbt_nz_free;
	itr->t = (sptensor_t *) t;
	sptensor_skbt_nz_load_index(itr);

	return (sptensor_iterator_t* ) itr;
}