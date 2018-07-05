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
#include "commands.h"


void
cmd_ntfd(cmdargs *args)
{
    FILE *file;
    sptensor *sptns;
    tensor_view *t;
    int i;
    ccd_result *result;
    double *lambda;
    char **argv = (char**)(args->args->ar);
    int argc = args->args->size;

    /* display usage message (if needed) */
    if(argc<2) {
	fprintf(stderr,
		"\nUsage: sptensor ntfd tns-file [lambda_1 lambda_2 ... ]\n\n");
	return;
    }
    
    /* open the file up and build the tensor view*/
    file = fopen(argv[1], "r");
    sptns = sptensor_read(file);
    fclose(file);
    t = sptensor_view_alloc(sptns);

    /* get the lambda values */
    lambda = malloc(sizeof(double) * t->nmodes);
    for(i=0; i<t->nmodes; i++) {
	if(i+2 >= argc) {
	    lambda[i] = 0.0;
	} else {
	    lambda[i] =  atof(argv[i+2]);
	}
    }

    /* do the decomposition */
    result = ccd_identity(t, args->nfactors, lambda, args->iter, args->tol);

    /* print stats to std err */
    fprintf(stderr, "Iterations: %d\n", result->iter);
    fprintf(stderr, "Final Error: %lf\n", result->final_error);
    fprintf(stderr, "Fit: %lf\n", result->fit);


    /* write the core tensor */
    cmd_write_tensor(args, "core", -1, result->core);

    /* write the factor matrices */
    for(i=0; i<result->n; i++) {
	cmd_write_tensor(args, "u", i+1, result->u[i]);
    }

    /* cleanup */
    TVFREE(t);
    sptensor_free(sptns);
    ccd_free(result);
    free(lambda);
    
    return;
}
