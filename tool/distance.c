/*
    This program computes the distance matrix for the given list of tensors.
    The lpnorm is used for the distance measure.
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
#include <math.h>
#include <sptensor/sptensor.h>
#include "commands.h"


void
cmd_distance(cmdargs *args)
{
    sptensor **t;
    tensor_view **u;
    tensor_view *diff;
    sptensor *dist;
    tensor_view *dist_view;
    int ntns;
    FILE *file;
    int i,j;
    int idx[2];
    int argc = args->args->size;
    char **argv = (char**)(args->args->ar);
    double x;
    double sum=0;
    double sumsq=0;
    double mean;
    double sp, ss;
    int count=0;
    
    /* ensure proper usage */
    if(argc < 3) {
	fprintf(stderr,
		"\nUsage: sptensor distance tensor1 [tensor2 ... ]\n\n");
	return;
    }

    /* process the easy parameters */
    ntns = argc - 1;

    /* read in the tensors */
    t = malloc(sizeof(sptensor*)*ntns);
    u = malloc(sizeof(tensor_view*)*ntns);
    for(i=0; i<ntns; i++) {
	file = fopen(argv[1+i], "r");
	t[i] = sptensor_read(file);
	u[i] = sptensor_view_alloc(t[i]);
	fclose(file);
    }

    /* allocate the distances */
    idx[0] = idx[1] = ntns;
    dist = sptensor_alloc(2, idx);

    /* compute the distances */
    for(j=0; j<ntns; j++) {
	idx[0] = j+1;
	for(i=0; i<ntns; i++) {
	    idx[1] = i+1;
	    diff = tensor_sub(u[j], u[i]);
	    x=tensor_lpnorm(diff, args->lp);
	    sptensor_set(dist, idx, x);
	    TVFREE(diff);

	    /* gather statistiscs */
	    if(i != j) {
		count++;
		sum += x;
		sumsq += x*x;
	    }
	}
    }

    /* write the output */
    dist_view = sptensor_view_alloc(dist);
    cmd_write_tensor(args, "distance", -1, dist_view);

    /* cleanup */
    sptensor_free(dist);
    for(i=0; i<ntns; i++) {
	TVFREE(u[i]);
	sptensor_free(t[i]);
    }
    TVFREE(dist_view);

    /* compute statitstics and print to stderr */
    mean=sum/count;
    ss=sp=sumsq - 2.0 * mean * sum + mean * mean * count;
    ss = sqrt(ss/(count-1));
    sp = sqrt(sp/count);
    fprintf(stderr, "Distribution\nMean: %lf\n Standard Deviation: %lf\nSample Standard Deviation: %lf\n", mean, sp, ss);
}
