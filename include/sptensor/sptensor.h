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
#include <gmp.h>
#include <stdio.h>
#include <sptensor/index.h>

/* General tensor framework */
#ifndef SPTENSOR_H
#define SPTENSOR_H

/* basic typedefs and struct prototypes */
struct sptensor;
struct sptensor_iterator;
typedef struct sptensor sptensor_t;
typedef struct sptensor_iterator sptensor_iterator_t;

/* function prototype types */
typedef void (*sptensor_get_f)(sptensor_t*, sptensor_index_t*, mpf_t );
typedef void (*sptensor_set_f)(sptensor_t*, sptensor_index_t*, mpf_t);
typedef struct sptensor_iterator* (*sptensor_iterator_f)(sptensor_t*);
typedef void (*sptensor_free_f)(void*);
typedef int (*sptensor_iterator_next_f)(struct sptensor_iterator*);
typedef int (*sptensor_iterator_prev_f)(struct sptensor_iterator*);
typedef int (*sptensor_iterator_valid_f)(struct sptensor_iterator*);
typedef void (*sptensor_iterator_get_f)(struct sptensor_iterator*, mpf_t);
typedef void (*sptensor_iterator_set_f)(struct sptensor_iterator*, mpf_t);

/* sptensor type (base type) */
struct sptensor
{
    int modes;
    sptensor_index_t *dim;
    sptensor_get_f get;
    sptensor_set_f set;
    sptensor_iterator_f iterator;
    sptensor_iterator_f nz_iterator;
    sptensor_free_f free;
};


/* sptensor iterator (base type) */
struct sptensor_iterator
{
    sptensor_t *t;
    sptensor_index_t *index;
    sptensor_iterator_valid_f valid;
    sptensor_iterator_next_f next;
    sptensor_iterator_prev_f prev;
    sptensor_iterator_get_f get;
    sptensor_iterator_set_f set;
    sptensor_free_f free;
};


/* wrappers for sptensor functions */
void sptensor_get(sptensor_t * t, sptensor_index_t *i, mpf_t v);
void sptensor_set(sptensor_t * t, sptensor_index_t *i, mpf_t v);
sptensor_iterator_t* sptensor_iterator(sptensor_t *t);
sptensor_iterator_t* sptensor_nz_iterator(sptensor_t *t);
void sptensor_free(sptensor_t* t);

/* wrappers for sptensor iterator */
int sptensor_iterator_next(struct sptensor_iterator* itr);
int sptensor_iterator_prev(struct sptensor_iterator* itr);
int sptensor_iterator_valid(struct sptensor_iterator* itr);
void sptensor_iterator_get(struct sptensor_iterator* itr, mpf_t v);
void sptensor_iterator_set(struct sptensor_iterator* itr, mpf_t v);
void sptensor_iterator_free(sptensor_iterator_t* t);

/* perform formatted printing of sptensors using the format string specifier for each value */
void sptensor_fprintf(FILE *file, const char *fmt, sptensor_t *t);

/* print the sptensor with a default format string to stdout */
void sptensor_print(sptensor_t *t);
#endif
