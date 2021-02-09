/*
    This is a collection of functions for performing basic tensor arithmetic.
    Copyright (C) 2020  Robert Lowe <pngwen@acm.org>

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
#include <sptensor/math.h>


/* Tensor additon rop = op1 + op2 */
void sptensor_add(sptensor_t *rop, sptensor_t *op1, sptensor_t *op2)
{
    mpf_t v1, v2; /* values */
    sptensor_iterator_t *itr;

    /* initialize values */
    mpf_inits(v1, v2, NULL);
    
    /* copy the non-zero values */
    for(itr = sptensor_nz_iterator(op1); 
        sptensor_iterator_valid(itr); 
        sptensor_iterator_next(itr)) 
    {
        sptensor_get(op1, itr->index, v1);
        sptensor_set(rop, itr->index, v1);
    }
    sptensor_iterator_free(itr);
   
    /* add the op2 values to the result tensor */
    for(itr = sptensor_nz_iterator(op2); 
        sptensor_iterator_valid(itr); 
        sptensor_iterator_next(itr)) 
    {
        sptensor_get(rop, itr->index, v1);
        sptensor_get(op2, itr->index, v2);
        mpf_add(v1, v1, v2);
        sptensor_set(rop, itr->index, v1);
    }
    sptensor_iterator_free(itr);

    /* clear the values */
    mpf_clears(v1, v2, NULL);
}


/* Tensor subtraction rop = op1 + op2 */
void sptensor_sub(sptensor_t *rop, sptensor_t *op1, sptensor_t *op2)
{
    mpf_t v1, v2; /* values */
    sptensor_iterator_t *itr;

    /* initialize values */
    mpf_inits(v1, v2, NULL);
    
    /* copy the non-zero values */
    for(itr = sptensor_nz_iterator(op1); 
        sptensor_iterator_valid(itr); 
        sptensor_iterator_next(itr)) 
    {
        sptensor_get(op1, itr->index, v1);
        sptensor_set(rop, itr->index, v1);
    }
    sptensor_iterator_free(itr);
   
    /* add the op2 values to the result tensor */
    for(itr = sptensor_nz_iterator(op2); 
        sptensor_iterator_valid(itr); 
        sptensor_iterator_next(itr)) 
    {
        sptensor_get(rop, itr->index, v1);
        sptensor_get(op2, itr->index, v2);
        mpf_sub(v1, v1, v2);
        sptensor_set(rop, itr->index, v1);
    }
    sptensor_iterator_free(itr);

    /* clear the values */
    mpf_clears(v1, v2, NULL);
}


/* Tensor scalar multiplication rop = op1 * op2 */
void sptensor_smul(sptensor_t *rop, sptensor_t *op1, mpf_t op2)
{
    mpf_t v; /* value */
    sptensor_iterator_t *itr;

    /* intilaize value */
    mpf_init(v);

    /* perform the multiplication */
    for(itr = sptensor_nz_iterator(op1); 
        sptensor_iterator_valid(itr);
        sptensor_iterator_next(itr))
    {
        sptensor_get(op1, itr->index, v);
        mpf_mul(v, v, op2);
        sptensor_set(rop, itr->index, v);
    }

    /* cleanup */
    sptensor_iterator_free(itr);
    mpf_clear(v);
}


/* Tensor scalar division rop = op1 / op2 */
void sptensor_sdiv(sptensor_t *rop, sptensor_t *op1, mpf_t op2)
{
    mpf_t v; /* value */
    sptensor_iterator_t *itr;

    /* intilaize value */
    mpf_init(v);

    /* perform the multiplication */
    for(itr = sptensor_nz_iterator(op1); 
        sptensor_iterator_valid(itr);
        sptensor_iterator_next(itr))
    {
        sptensor_get(op1, itr->index, v);
        mpf_div(v, v, op2);
        sptensor_set(rop, itr->index, v);
    }

    /* cleanup */
    sptensor_iterator_free(itr);
    mpf_clear(v);
}
