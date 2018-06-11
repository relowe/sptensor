/*
  sptensortest: This program tests the sptensor functions.
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

#include <stdio.h>
#include <sptensor.h>
#include <sptensorio.h>
#include <view.h>
#include <stdlib.h>
#include <time.h>
#define RANDOM_TRIALS 1e6

/* returns number of seconds n random lookups take */
double
randomGetTime(tensor_view *v, int n)
{
    clock_t t;
    sp_index_t *idx = TVIDX_ALLOC(v);
    int i,j; /* indexes */
    double value;

    t = clock();
    for(i=0; i<n; i++) {
        /* generate the index */
        for(j=0; j<v->nmodes; j++) {
            idx[j] = rand() % v->dim[j] + 1;
        }

        /* access */
        value = TVGET(v, idx);
    }
    t = clock()-t; /* get clock duration */
    return ((double)t)/CLOCKS_PER_SEC;
}


/* returns number of second n random sets take */
double
randomSetTime(tensor_view *v, int n)
{
    clock_t t;
    sp_index_t *idx = TVIDX_ALLOC(v);
    int i,j; /* indexes */
    double value;

    t = clock();
    for(i=0; i<n; i++) {
        /* generate the index and value */
        for(j=0; j<v->nmodes; j++) {
            idx[j] = rand() % v->dim[j] + 1;
        }
        value=(double)(rand()%1000)/(rand()%10+1);

        /* write */
        TVSET(v, idx, value);
    }
    t = clock()-t; /* get clock duration */
    return ((double)t)/CLOCKS_PER_SEC;
}


int
main(int argc, char **argv)
{
    sptensor *sp;
    tensor_view *v, *vi, *vuf;
    tensor_view *vslice;
    tensor_slice_spec *slice;
    
    sp_index_t *idx;
    int i;
    FILE *file;

    /* read the tensor */
    file = fopen(argv[1], "r");
    sp = sptensor_read(file);
    fclose(file);

    /* print the tensor */
    printf("sptensor_write\n");
    sptensor_write(stdout, sp);
    printf("\n\n");

    /* create an sptensor view */
    printf("sptensor view\n");
    v = sptensor_view_alloc(sp);
    tensor_view_clprint(v);
    printf("\n\n");
    printf("Pretty Print\n");
    tensor_view_print(v, 2);
    printf("\n\n");

    /* do some unfolding */
    for(i=0; i<v->nmodes; i++) {
        printf("Unfold along mode %d\n", i);
        vuf = unfold_tensor(v, i);
        tensor_view_print(vuf, 2);
        TVFREE(vuf);
        printf("\n\n");
    }
    
    /* identity tensor */
    vi = identity_tensor(v->nmodes, v->dim);
    printf("identity tensor\n");
    tensor_view_print(vi, 0);
    printf("\n\n");

    /* tensor slice */
    slice = tensor_slice_spec_alloc(vi);
    printf("column slice\n");
    for(i=1; i<vi->nmodes; i++) {
        slice->fixed[i]=1;
    }
    vslice = tensor_slice(vi, slice);
    tensor_view_print(vslice, 0);
    tensor_view_clprint(vslice);
    free(slice);
    TVFREE(vslice);
    printf("\n\n");
    printf("front upper slice\n");
    slice = tensor_slice_spec_alloc(vi);
    slice->begin[0] = 1;
    slice->end[0] = 2;
    slice->begin[1] = 1;
    slice->end[1] = 2;
    for(i=2; i<vi->nmodes; i++) {
        slice->fixed[i]=1;
    }
    vslice=tensor_slice(vi, slice);
    tensor_view_print(vslice, 0);
    tensor_view_clprint(vslice);
    free(slice);
    TVFREE(vslice);
    printf("\n\n");
    printf("Center fiber\n");
    slice = tensor_slice_spec_alloc(vi);
    slice->begin[vi->nmodes-1] = 1;
    slice->end[vi->nmodes-1] = vi->dim[vi->nmodes-1];
    for(i=0; i<vi->nmodes-1; i++) {
        slice->fixed[i]=vi->dim[i]/2;
    }
    vslice = tensor_slice(vi, slice);
    tensor_view_print(vslice, 0);
    tensor_view_clprint(vslice);
    free(slice);
    TVFREE(vslice);
    
    /* benchmark */
    printf("%d random gets take: %g seconds\n", (int)RANDOM_TRIALS, randomGetTime(v, RANDOM_TRIALS));
    printf("%d random sets take: %g seconds\n", (int)RANDOM_TRIALS, randomSetTime(v, RANDOM_TRIALS));
    printf("%d random gets take: %g seconds\n", (int)RANDOM_TRIALS, randomGetTime(v, RANDOM_TRIALS));

    
    /* deallocate it all */
    TVFREE(v);
    TVFREE(vi);
    sptensor_free(sp);
}
