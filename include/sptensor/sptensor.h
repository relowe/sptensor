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


/* General tensor framework */
#ifndef SPTENSOR_H
#define SPTENSOR_H

/* basic typedefs and struct prototypes */
typedef unsigned int sptensor_index_t;
struct sptensor;
struct sptensor_iterator;

/* function prototype types */
typedef double (*sptensor_get_f)(struct sptensor*, sptensor_index_t*);
typedef double (*sptensor_set_f)(struct sptensor*, sptensor_index_t*, double);
typedef struct sptensor_iterator* (*sptensor_iterator_f)(struct sptensor*);
typedef void (*sptensor_free_f)(void*);
typedef int (*sptensor_iterator_next_f)(struct sptensor_iterator*);
typedef int (*sptensor_iterator_prev_f)(struct sptensor_iterator*);
typedef int (*sptensor_iterator_valid_f)(struct sptensor_iterator*);

/* sptensor type (base type) */
typedef struct sptensor
{
    int modes;
    sptensor_index_t *dim;
    sptensor_get_f get;
    sptensor_set_f set;
    sptensor_iterator_f iterator;
    sptensor_iterator_f nz_iterator;
    sptensor_free_f free;
} sptensor_t;


/* sptensor iterator (base type) */
typedef struct sptensor_iterator
{
    sptensor_t *t;
    sptensor_index_t *index;
    sptensor_iterator_valid_f valid;
    sptensor_iterator_next_f next;
    sptensor_iterator_prev_f prev;
    sptensor_free_f free;
} sptensor_iterator_t;


/* inline wrappers for sptensor functions */
inline double sptensor_get(sptensor_t * t, sptensor_index_t *i) { return t->get(t, i); }
inline double sptensor_set(sptensor_t * t, sptensor_index_t *i, double d) { return t->set(t, i, d); }
inline sptensor_iterator_t* sptensor_iterator(sptensor_t *t) { return t->iterator(t); }
inline sptensor_iterator_t* sptensor_nz_iterator(sptensor_t *t) { return t->nz_iterator(t); }
inline void sptensor_free(sptensor_t* t){ t->free(t); }

/* inline wrappers for sptensor iterator */
inline int sptensor_iterator_next(struct sptensor_iterator* itr) { return itr->next(itr); }
inline int sptensor_iterator_prev(struct sptensor_iterator* itr) { return itr->prev(itr); }
inline int sptensor_iterator_valid(struct sptensor_iterator* itr) { return itr->valid(itr); }
inline void sptensor_free(sptensor_t* t){ t->free(t); }
#endif