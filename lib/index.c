/*
    This is the implemenation of the sptensor index operations. 

    The sptensor library provides a set of functions and data type for
    dealing with sparse tensors.
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
#include <sptensor/index.h>


/* 
 * Compare two indexes for a given tensor.  Comparison is 
 * performed from left to right.  Pretty much exactly as 
 * strcmp, except with ints.
 * 
 * Parameter: nmodes - The number of modes
 *            a   - The left hand index being compared
 *            b   - The right hand index being compared
 *  
 * Return:  < 0 if a < b
 *            0 if a == b
 *          > 0 if a > b
 */
int
sptensor_index_cmp(unsigned int nmodes, const sptensor_index_t *a, const sptensor_index_t *b)
{
    int i;

    /* look for < or > comparisons */
    for(i=0; i < nmodes; i++) {
	if(a[i] < b[i]) {
	    return -1;
	}

	if(a[i] > b[i]) {
	    return 1;
	}
    }

    return 0; /* must be equal! */
}


/* 
 * Increment an index in a row major way 
 */
void
sptensor_index_inc(unsigned int nmodes, const sptensor_index_t *dim, sptensor_index_t *idx)
{
    unsigned int ui;

    /* start with columns! */
    if(nmodes >= 2) {
	    ui = 1;
    } else {
	    ui = 0;
    }

    do {
	    idx[ui]++;  /* increment */

	    /* accept valid increments */
	    if(idx[ui] <= dim[ui]) {
	        return;
	    }

    	/* go on to the next mode! */
	    idx[ui] = 1;
	    if(ui == 0) {
	        ui = 2;
	    } else if(ui == 1){
	        ui = 0;
	    } else {
	        ui++;
	    }
    } while(ui < nmodes); /* once we have done the last mode, God help us! */

    /* make it bigger */
    for(ui=0; ui<nmodes; ui++) {
	    idx[ui] = dim[ui]+1;
    }
}


/* 
 * Decrement an index in a row major way 
 */
void
sptensor_index_dec(unsigned int nmodes, const sptensor_index_t *dim, sptensor_index_t *idx)
{
    unsigned int ui;

    /* start with columns! */
    if(nmodes >= 2) {
	    ui = 1;
    } else {
	    ui = 0;
    }

    do {
	    idx[ui]--; /* decrement */

	    /* accept valid increments */
	    if(idx[ui] > 0) {
	        return;
	    }

    	/* go on to the next mode! */
	    idx[ui] = dim[ui];
	    if(ui == 0) {
	        ui = 2;
	    } else if(ui == 1){
	        ui = 0;
	    } else {
	        ui++;
	    }
    } while(ui < nmodes); /* once we have done the last mode, God help us! */

    /* make it smaller */
    for(ui=0; ui<nmodes; ui++) {
	    idx[ui] = 0;
    }
}