/*
    This is a collection of functions for the computation of MTTKRP.
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
#ifndef SPTENSOR_MTTKRP_H
#define SPTENSOR_MTTKRP_H
#include <sptensor/sptensor.h>
#include <sptensor/matrix.h>

/* Compute MTTKRP between tensor x and matrices b and c:  x * b o c */
sptensor_matrix_t* sptensor_mttkrp(sptensor_t *x, sptensor_matrix_t *b, sptensor_matrix_t *c);

#endif