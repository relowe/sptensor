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
#include<limits.h>
#include<stdio.h>
#include <sptensor/storage.h>

static sp_index_t OCCBIT = UINT_MAX ^ (UINT_MAX >> 1);

#define IS_OCCUPIED(h) ((h) & OCCBIT)
#define OCCUPIED(h) ( (h) | OCCBIT )
#define NOT_OCCUPIED(h) ( (h) & ~OCCBIT )
