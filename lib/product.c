/*
    This is a collection of functions for various tensor products.
    Copyright (C) 2021 Robert Lowe <rlowe@semo.edu>

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
#include <sptensor/product.h>


/* Get the shape of the matrix product */
sptensor_shape_t *sptensor_matrix_product_shape(sptensor_t *op1, sptensor_t *op2)
{
    sptensor_shape_t *result;

    /* create the shape */
    result = sptensor_shape_alloc(2);

    /* compute the shape */
    result->dim[0] = op1->dim[0];
    result->dim[1] = op2->dim[1];


    return result;
}

/* Compute the matrix product rop = op1 x op2 */
void sptensor_matrix_product(sptensor_t *rop, sptensor_t *op1, sptensor_t *op2)
{
    sptensor_iterator_t *itr1, *itr2;
    mpf_t rval, val1, val2;
    sptensor_index_t ridx[2];

    /* initilialize the mpf structures */
    mpf_inits(rval, val1, val2, NULL);

    /* create the iterator */
    itr1 = sptensor_nz_iterator(op1);
    while(sptensor_iterator_valid(itr1)) {

        /* iterate over the other non-zeroes */
        itr2 = sptensor_nz_iterator(op2);
        while(sptensor_iterator_valid(itr2)) {
            /* check to see if this is a multiplication we need to do */
            if(itr1->index[1] == itr2->index[0]) {
                /* get the original value */
                ridx[0] = itr1->index[0];
                ridx[1] = itr2->index[1];
                sptensor_get(rop, ridx, rval);

                /* retrieve the operations and multiply */
                sptensor_get(op1, itr1->index, val1);
                sptensor_get(op2, itr2->index, val2);
                mpf_mul(val1, val1, val2);

                /* add the result and store it */
                mpf_add(rval, rval, val1);
                sptensor_set(rop, ridx, rval);
            }
            /* next op2 non-zero */
            sptensor_iterator_next(itr2);
        }
        sptensor_iterator_free(itr2);

        /* go to the next nonzero */
        sptensor_iterator_next(itr1);
    }

    /* clean up */
    sptensor_iterator_free(itr1);
    mpf_clears(rval, val1, val2, NULL);
}


/* Get the shape of the nmode product */
sptensor_shape_t *sptensor_nmode_product_shape(sptensor_index_t n, sptensor_t *op1, sptensor_t *op2)
{
    sptensor_shape_t *result;

    /* allocate the result */
    result = sptensor_shape_alloc(op1->modes);

    /* calculate the shape */
    sptensor_index_cpy(result->modes, result->dim, op1->dim);
    result->dim[n] = op2->dim[0];

    return result;
}


/* Compute the nmode prouct of tensor op1 by matrix op2.
    rop = op1 x_n op2
 */
void sptensor_nmode_product(sptensor_index_t n, sptensor_t *rop, sptensor_t *op1, sptensor_t *op2)
{
    sptensor_iterator_t *itr1, *itr2;
    mpf_t rval, val1, val2;
    sptensor_index_t *ridx;

    /* initilialize the mpf and index structures */
    mpf_inits(rval, val1, val2, NULL);
    ridx=sptensor_index_alloc(rop->modes);

    /* create the iterator */
    itr1 = sptensor_nz_iterator(op1);
    while(sptensor_iterator_valid(itr1)) {

        /* iterate over the other non-zeroes */
        itr2 = sptensor_nz_iterator(op2);
        while(sptensor_iterator_valid(itr2)) {
            /* check to see if this is a multiplication we need to do */
            if(itr1->index[n] == itr2->index[1]) {
                /* get the original value */
                sptensor_index_cpy(rop->modes, ridx, itr1->index);
                ridx[n] = itr2->index[0];
                sptensor_get(rop, ridx, rval);

                /* retrieve the operations and multiply */
                sptensor_get(op1, itr1->index, val1);
                sptensor_get(op2, itr2->index, val2);
                mpf_mul(val1, val1, val2);

                /* add the result and store it */
                mpf_add(rval, rval, val1);
                sptensor_set(rop, ridx, rval);
            }
            /* next op2 non-zero */
            sptensor_iterator_next(itr2);
        }
        sptensor_iterator_free(itr2);

        /* go to the next nonzero */
        sptensor_iterator_next(itr1);
    }

    /* clean up */
    sptensor_iterator_free(itr1);
    mpf_clears(rval, val1, val2, NULL);
    sptensor_index_free(ridx);
}


/* Compute the shape of the outer tensor product */
sptensor_shape_t *sptensor_outer_product_shape(sptensor_t *op1, sptensor_t *op2)
{
   sptensor_shape_t *result;

   /* create the result */ 
   result = sptensor_shape_alloc(op1->modes + op2->modes);

   /* concatenate the modes */
   sptensor_index_cpy(op1->modes, result->dim, op1->dim);
   sptensor_index_cpy(op2->modes, result->dim + op1->modes, op2->dim);


   return result;
}


/* Compute the outer tensor product */
void sptensor_outer_product(sptensor_t *rop, sptensor_t *op1, sptensor_t *op2)
{
    sptensor_iterator_t *itr1, *itr2;
    sptensor_index_t *ridx;
    mpf_t rval, val1, val2;

    /* create the index and values*/
    ridx = sptensor_index_alloc(rop->modes);
    mpf_inits(rval, val1, val2, NULL);

    /* multiply each pairing */
    itr1 = sptensor_nz_iterator(op1);
    while(sptensor_iterator_valid(itr1)) {
        itr2 = sptensor_nz_iterator(op2);
        while(sptensor_iterator_valid(itr2)) {
            /* perform the multiplication */
            sptensor_get(op1, itr1->index, val1);
            sptensor_get(op2, itr2->index, val2);
            mpf_mul(rval, val1, val2);

            /* concatenate the indexes */
            sptensor_index_cpy(op1->modes, ridx, itr1->index);
            sptensor_index_cpy(op2->modes, ridx+op1->modes, itr2->index);

            /* store the result */
            sptensor_set(rop, ridx, rval);

            /* next! */
            sptensor_iterator_next(itr2);
        }
        sptensor_iterator_free(itr2);

        /* next! */
        sptensor_iterator_next(itr1);
    }

    /* cleanup */
    sptensor_index_free(ridx);
    sptensor_iterator_free(itr1);
    mpf_clears(rval, val1, val2, NULL);
}


/* Shape of the Hadamard product */
sptensor_shape_t *sptensor_hadamard_product_shape(sptensor_t *op1, sptensor_t *op2)
{
    sptensor_shape_t *result;

    /* create the result */
    result = sptensor_shape_alloc(op1->modes);
    sptensor_index_cpy(result->modes, result->dim, op1->dim);

    return result;
}


/* Compute the Hadamard product */
void sptensor_hadamard_product(sptensor_t *rop, sptensor_t *op1, sptensor_t *op2)
{
    sptensor_iterator_t *itr;
    mpf_t rval, val1, val2;

    /* initialize things */
    mpf_inits(rval, val1, val2, NULL);

    /* go through the non-zeros and multiply by their counter parts. */
    itr = sptensor_nz_iterator(op1);
    while(sptensor_iterator_valid(itr)) {
        /* retrieve the operands and multiply */
        sptensor_get(op1, itr->index, val1);
        sptensor_get(op2, itr->index, val2);
        mpf_mul(rval, val1, val2);

        /* store the result */
        sptensor_set(rop, itr->index, rval);

        /* go to the next item */
        sptensor_iterator_next(itr);
    }

    /* cleanup */
    sptensor_iterator_free(itr);
    mpf_clears(rval, val1, val2, NULL);
}