/*
    This progra performs non-negative tensor factorization using the CCD
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
#include <sptensor.h>
#include <view.h>
#include <ccd.h>
#include <sptensorio.h>

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

    /* write the ccd result */
    

    /* cleanup */
    TVFREE(t);
    sptensor_free(sptns);
    ccd_free(result);
    free(lambda);
    
    return 0;
}
