/*
  This program tests dense tensor storage.
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
#include <sptensor/sptensor.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    sp_index_t *idx;
    sp_index_t *dim;
    tensor_view *tns;
    int nmodes;
    int i;
    double val;

    printf("Modes: ");
    scanf("%d", &nmodes);

    idx = malloc(sizeof(sp_index_t) * nmodes);
    dim = malloc(sizeof(sp_index_t) * nmodes);

    printf("Dims: ");
    for(i=0; i<nmodes; i++) {
	scanf("%d", dim+i);
	idx[i] = 1;
    }

    tns = dense_tensor_alloc(nmodes, dim);
    
    i=0;
    for(idx; sptensor_indexcmp(nmodes, idx, dim) <=0;
	sptensor_index_inc(nmodes, dim, idx)) {

	if(rand()%6 < 3) {
	    val = 0.0;
	} else {
	    val = i+1;
	}

	TVSET(tns, idx, val);
	i++;
    }

    printf("Pretty Print\n");
    tensor_print(tns, 2);
    printf("\n\n");

    printf("Coordinate List\n");
    tensor_clprint(tns);
    printf("\n\n");

    TVFREE(tns);
}
