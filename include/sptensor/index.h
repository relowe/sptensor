/*
    This is the header file for sptensor index operations. 

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
#ifndef SPTENSOR_INDEX_H
#define SPTENSOR_INDEX_H

/* data type for index values */
typedef unsigned int sptensor_index_t;

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
int sptensor_index_cmp(unsigned int nmodes, const sptensor_index_t *a, const sptensor_index_t *b);


/* 
 * Increment an index in a row major way 
 */
void sptensor_index_inc(unsigned int nmodes, const sptensor_index_t *dim, sptensor_index_t *idx);

#endif