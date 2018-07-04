/*
    This program performs non-negative tensor factor decomposition using the CCD
    algorithm.
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

/* find the root name of the file without extension or directory */
char *root_name(char *str)
{
    int start, end;  /* bounds of the core name in the string */
    int len;
    int i;
    char *name;

    /* find the last / if there is one */
    for(i=strlen(str)-1; i>=0; i--) {
	if(str[i] == '/') {
	    break;
	}
    }
    start = i+1;

    /* find the last . if there is one */
    for(i=strlen(str)-1; i>=0; i--) {
	if(str[i] == '.') {
	    break;
	}
    }
    end = i;
    if(end == 0) {
	end = strlen(str);
    }

    /* compute length of the new string and allocate it */
    len = end - start;
    name = malloc(sizeof(len+1));

    /* produce and return the result */
    strncpy(name, str + start, end);
    name[len] = '\0';
    return name;
}


int main(int argc, char **argv)
{
    FILE *file;
    sptensor *sptns;
    tensor_view *t;
    int n;
    int max_iter;
    int i;
    ccd_result *result;
    double *lambda;
    double tol;
    int max_name_len;
    char *rname;
    char *fname;

    /* display usage message (if needed) */
    if(argc<5) {
	fprintf(stderr,
		"\nUsage: %s tns-file nfactors max_iter tol [lambda_1 lambda_2 ... ]\n\n",
		argv[0]);
	return -1;
    }
    
    /* open the file up and build the tensor view*/
    file = fopen(argv[1], "r");
    sptns = sptensor_read(file);
    fclose(file);
    t = sptensor_view_alloc(sptns);

    /* get the model parameters */
    n = atoi(argv[2]);
    max_iter = atoi(argv[3]);
    tol = atof(argv[4]);

    /* get the lambda values */
    lambda = malloc(sizeof(double) * t->nmodes);
    for(i=0; i<t->nmodes; i++) {
	if(i+5 >= argc) {
	    lambda[i] = 0.0;
	} else {
	    lambda[i] =  atof(argv[i+5]);
	}
    }

    /* do the decomposition */
    result = ccd_identity(t, n, lambda, max_iter, tol);

    /* print stats to std err */
    fprintf(stderr, "Iterations: %d\n", result->iter);
    fprintf(stderr, "Final Error: %lf\n", result->final_error);
    fprintf(stderr, "Fit: %lf\n", result->fit);


    /* make sapce for the file names.  This is probably a tad too long,
       but what's a couple of bytes between friends? */
    rname = root_name(argv[1]);
    max_name_len = (int)ceil(log10(result->n)) + strlen(rname) + 10; 
    fname = malloc(max_name_len);

    /* write the core tensor */
    sprintf(fname, "%s-core.tns", rname);
    file = fopen(fname, "w");
    tensor_view_write(file, result->core);
    fclose(file);
    fprintf(stderr, "%s Written\n", fname);

    /* write the factor matrices */
    for(i=0; i<result->n; i++) {
	sprintf(fname, "%s-u%d.tns", rname, i+1);
	file = fopen(fname, "w");
	tensor_view_write(file, result->u[i]);
	fclose(file);
	fprintf(stderr, "%s Written\n", fname);
    }

    /* cleanup */
    TVFREE(t);
    sptensor_free(sptns);
    ccd_free(result);
    free(lambda);
    free(rname);
    free(fname);
    
    return 0;
}
