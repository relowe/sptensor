
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
#include <sptensor/sptensor.h>
#include <sptensor/mttkrp.h>
#include <sptensor/transform.h>
#include <sptensor/product.h>
#include <sptensor/math.h>
#include <sptensor/hash.h>
#include <string.h>

/* Use Shaden Smith's SPLATT algorithm to compute an MTTKRP row */
static void splatt_row(sptensor_matrix_t *m, sptensor_t *x, sptensor_matrix_t *b, sptensor_matrix_t *c, int i);

/* simple integer set for uniqueness checks */
struct intset {
    int *ar;
    int size;
    int entries;
};

/* create an integer set */
static struct intset set_alloc();

/* destroy an integer set */
static void set_free(struct intset s);

/* insert into the integer set */
static void set_add(struct intset *s, int i);

/* rehash a set */
static void set_rehash(struct intset *s);


/* Compute MTTKRP between tensor x and matrices b and c:  x * b o c */
sptensor_matrix_t* sptensor_mttkrp(sptensor_t *x, sptensor_matrix_t *b, sptensor_matrix_t *c)
{

}


/* Use Shaden Smith's SPLATT algorithm to compute an MTTKRP row */
static void splatt_row(sptensor_matrix_t *m, sptensor_t *x, sptensor_matrix_t *b, sptensor_matrix_t *c, int i)
{
    struct intset kset;
    sptensor_shape_t *shape;
    sptensor_t *slice;
    sptensor_t *f;
    sptensor_index_t idx[3];
    sptensor_index_t spec[3];
    sptensor_iterator_t *itr;
    mpf_t v;
    sptensor_index_t fdim[2];
    int i;

    /* get the slice for the row */
    spec[0] = (sptensor_index_t) i;
    spec[1] = 0;
    spec[2] = 0;
    shape = sptensor_slice_shape(x, spec);
    slice = sptensor_hash_alloc(shape->dim, shape->modes);
    sptensor_slice(slice, x, spec);
    sptensor_shape_free(shape);

    /* get the unique ks */
    kset = set_alloc();
    itr=sptensor_nz_iterator(slice);
    while(sptensor_iterator_valid(itr)) {
        set_add(&kset, itr->index[slice->modes-1]);
        sptensor_nz_next(itr);
    }
    sptensor_iterator_free(itr);
    sptensor_free(slice);


    /* go through all the unique k's */
    for(i=0; i<kset.size; i++) {
        /* skip empty entries */
        if(!kset.ar[i]) continue;
    }
}


/* create an integer set */
static struct intset set_alloc()
{
    struct intset s;
    int i;

    s.entries = 0;
    s.size = 128;
    s.ar = malloc(sizeof(int) * s.size);

    /* 0 it all out */
    memset(s.ar, 0, sizeof(int) * s.size);
}


/* destroy an integer set */
static void set_free(struct intset p)
{
    free(p.ar);
}


/* insert into the integer set */
static void set_add(struct intset *s, int i)
{
    int index = i%s->entries;

    /* probe for a spot */
    while(s->ar[index]) index = (index + 1) % s->entries;

    s->ar[index] = i;
    s->entries++;

    if(s->entries > (80 * s->size)/100) {
        set_rehash(s);
    }
}


/* rehash a set */
static void set_rehash(struct intset *s)
{
    int *oldar;
    int oldsize;
    int i;

    oldar = s->ar;
    oldsize = s->size;

    /* create the new entries */
    s->size *= 2;
    s->ar = malloc(sizeof(int) * s->size);
    memset(s->ar, 0, sizeof(int) * s->size);

    /* do the rehahshing */
    for(i=0; i<oldsize; i++) {
        if(oldar[i]) {
            set_add(s, oldar[i]);
        }
    }

    /* free the old array */
    free(oldar);
}