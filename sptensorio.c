/*
    This is a collection of functions for dealing with sparse tensors.
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

#include <stdlib.h>
#include <sptensorio.h>

/* 
 * Read a sparse tensor from a file stream. The file stream is
 * expected to be in the form of:
 *   nmodes d0 d1 ... dk
 *   i0 i1 i2 ... ik v1
 *   i0 i1 i2 ... ik v2
 *   ...
 * Where nmodes is the number of modes, d0...dk are the dimensions,
 * and i0..ik are the indexes and v is the value at that index.
 * The file is read to the end, or until an unsucessful translation
 * occurs.
 *
 * Parameters: file - The file to read from
 *             
 * Return: The newly read and allocated tensor.
 */
sptensor *
sptensor_read(FILE *file)
{
    int nmodes;
    sp_index_t *idx;
    int i;
    double val;
    sptensor *tns;
    int done;

    /* get the dimensions */
    fscanf(file, "%u", &nmodes);
    idx = (sp_index_t*) malloc(sizeof(sp_index_t)*nmodes);
    for(i=0; i<nmodes; i++) {
	fscanf(file, "%u", idx + i);
    }

    /* allocate the tensor and free the dimension */
    tns = sptensor_alloc(nmodes, idx);

    i=0;
    done = 0;
    while(!done) {
	/* read the indexes */
	for(i=0; i<nmodes; i++) {
	    if(fscanf(file, "%u", idx + i) != 1) {
		done = 1;
		break;
	    }
	}

	if(done) continue;
	if(fscanf(file, "%lf", &val) != 1) {
	    done = 1;
	    break;
	}

	/* insert into the tensor */
	sptensor_set(tns, idx, val);
    }

    /* cleanup and return! */
    free(idx);
    return tns;
}


/*
 * Write a sparse tensor to a stream.  It is written in the format 
 * described in the sptensor_read function.
 * 
 * Parameter: file - The stream to write to.
 *            tns  - The tensor to write.
 */ 
void
sptensor_write(FILE *file, sptensor *tns)
{
    int i, j;
    
    /* print the preamble */
    fprintf(file, "%u", tns->nmodes);
    for(i = 0; i < tns->nmodes; i++) {
	fprintf(file, " %u", tns->dim[i]);
    }
    fprintf(file, "\n");

    /* print the non-zero values */
    for(i = 0; i < tns->ar->size; i++) {
	for(j = 0; j < tns->nmodes; j++) {
	    fprintf(file, "%u\t", *((unsigned int*)(VPTR(tns->idx, i))+j));
	}
	fprintf(file, "%g\n", VVAL(double, tns->ar,i));
    }
}
