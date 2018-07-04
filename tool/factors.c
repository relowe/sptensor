/*
    This program converts factor matrix (non-normalized Kruskal format) into
    tensor factors.  The tensors are normalized so they represent proportional
    profiles.  The output of the program is a set of tensor files, plus one
    file containing the normalization factors.

    Copyright (C) 2018 Robert Lowe <pngwen@acm.org>

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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sptensor/sptensor.h>

/* prototypes */
static int factorcmp(int size, void *a, void *b);
static void insert_factor(vector *factor, vector *lambda, tensor_view *f, double norm);


int
main(int argc, char **argv)
{
    sptensor **t;
    tensor_view **u;
    tensor_view *f;
    tensor_view *col;
    tensor_view *prev;
    tensor_slice_spec *slice;
    char *prefix;
    char *fname;
    int ntns;
    vector *factor;
    vector *lambda;
    double lp;
    double norm;
    FILE *file;
    int i,j;

    /* ensure proper usage */
    if(argc < 4) {
	fprintf(stderr,
		"\nUsage: %s lpnorm output-prefix tensor1 [tensor2 ...]\n\n", argv[0]);
	exit(-1);
    }

    /* process the easy parameters */
    lp = atof(argv[1]);
    prefix = argv[2];
    ntns = argc - 3;
    fname = malloc(strlen(prefix) + ceil(log10(ntns)) + 15);
    factor = vector_alloc(sizeof(tensor_view*), 16);
    lambda = vector_alloc(sizeof(double), 16);

    /* read in the tensors */
    t = malloc(sizeof(sptensor*)*ntns);
    u = malloc(sizeof(tensor_view*)*ntns);
    for(i=0; i<ntns; i++) {
	file = fopen(argv[3+i], "r");
	t[i] = sptensor_read(file);
	u[i] = sptensor_view_alloc(t[i]);
	fclose(file);
    }

    /* assemble and insert the factors */
    for(j=1; j <= u[0]->dim[1]; j++) {
	for(i=0; i<ntns; i++) {
	    slice = tensor_slice_spec_alloc(u[i]);
	    slice->fixed[1] = j;
	    col = tensor_slice(u[0], slice);
	    if(i==0) {
		f = tensor_view_deep_copy(col);
	    } else {
		prev = f;
		f = tensor_product(f, col);
		TVFREE(prev);
	    }
	    TVFREE(col);
	    tensor_slice_spec_free(slice);
	}
	
	norm = tensor_lpnorm(f, lp);
	tensor_scale(f, 1.0/norm);
	insert_factor(factor, lambda, f, norm);
    }

    /* write the factor files */
    for(i=0; i<factor->size; i++) {
	sprintf(fname, "%s%d.tns", prefix, i+1);
	file = fopen(fname, "w");
	tensor_view_write(file, VVAL(tensor_view*, factor, i));
	fclose(file);
    }

    /* write the lambdas */
    sprintf(fname, "%s-lambda.tns", prefix);
    file = fopen(fname, "w");
    fprintf(file, "1 %d\n", lambda->size);
    for(i=0; i<lambda->size; i++) {
	fprintf(file, "%d %lf\n", i+1, VVAL(double, lambda, i));
    }
    fclose(file);

    /* cleanup */
    for(i=0; i<ntns; i++) {
	sptensor_free(t[i]);
	TVFREE(u[i]);
    }
    for(i=0; i<factor->size; i++) {
	TVFREE(VVAL(tensor_view*, factor, i));
    }
}


static int
factorcmp(int size, void *a, void *b)
{
    double *x, *y;

    x = (double*)a;
    y = (double*)b;

    if(*x < *y) return 1;
    if(*x == *y) return 0;
    if(*x > *y) return -1;
}


static void
insert_factor(vector *factor, vector *lambda, tensor_view *f, double norm)
{
    int i;
    
    i=vector_sorted_insert(lambda, &norm, factorcmp);
    vector_insert(factor, i, &f);
}
