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
#include <sptensor.h>
#include <sptensorio.h>
#include <view.h>
#include <tensor_math.h>


int
main(int argc, char **argv)
{
    sptensor **t;
    tensor_view **u;
    tensor_view *diff;
    sptensor *dist;
    char *fname;
    int ntns;
    double lp;
    FILE *file;
    int i,j;
    int idx[2];

    /* ensure proper usage */
    if(argc < 4) {
	fprintf(stderr,
		"\nUsage: %s lpnorm outfile tensor1 [tensor2 ...]\n\n", argv[0]);
	exit(-1);
    }

    /* process the easy parameters */
    lp = atof(argv[1]);
    fname = argv[2];
    ntns = argc - 3;

    /* read in the tensors */
    t = malloc(sizeof(sptensor*)*ntns);
    u = malloc(sizeof(tensor_view*)*ntns);
    for(i=0; i<ntns; i++) {
	file = fopen(argv[3+i], "r");
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
	    sptensor_set(dist, idx, tensor_lpnorm(diff, lp));
	    TVFREE(diff);
	}
    }

    /* write the output */
    file = fopen(fname, "w");
    sptensor_write(file, dist);
    fclose(file);

    /* cleanup */
    sptensor_free(dist);
    for(i=0; i<ntns; i++) {
	TVFREE(u[i]);
	sptensor_free(t[i]);
    }
}
