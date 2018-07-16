/*
    This is the main header file for the sptensor library.  The sptensor 
    library provides a set of functions and data type for dealing with
    sparse tensors.
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
#include <sptensor/storage.h>
#include <sptensor/binsearch.h>
#include <sptensor/ccd.h>
#include <sptensor/multiply.h>
#include <sptensor/sptensorio.h>
#include <sptensor/tensor_math.h>
#include <sptensor/vector.h>
#include <sptensor/view.h>

/* Maximum memory to use for dtensors */
extern unsigned int sptensor_max_memory;

/* 1 for verbose output, 0 for no output */
extern unsigned int sptensor_verbose;

/* default norm level to use in tensor algorithms */
extern int stpensor_default_lp;
