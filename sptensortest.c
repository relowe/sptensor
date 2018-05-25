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
    tensor_view *v;
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

    /* benchmark */
    printf("%d random gets take: %g seconds\n", (int)RANDOM_TRIALS, randomGetTime(v, RANDOM_TRIALS));
    printf("%d random sets take: %g seconds\n", (int)RANDOM_TRIALS, randomSetTime(v, RANDOM_TRIALS));
    printf("%d random gets take: %g seconds\n", (int)RANDOM_TRIALS, randomGetTime(v, RANDOM_TRIALS));

    
    /* deallocate it all */
    TVFREE(v);
    sptensor_free(sp);
}
