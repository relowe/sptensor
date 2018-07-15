/*
  This program tests sptensor's ccd functions.

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
#include <sptensor/sptensor.h>

#define ARSIZE(a) (sizeof(a)/sizeof(a[0]))

/* set up a 3x2x2 tensor */
sp_index_t adim[] = {2, 2, 2};
sp_index_t aidx_list[][3] = {{1,1,1},
			     {2,1,1},
			     {1,2,1},
			     {1,1,2},
			     {2,1,2}};
double a_values[]={1,2,3,4,5};
#define ANELEM ARSIZE(a_values)
#define ANDIM ARSIZE(adim)


int main()
{
    tensor_view *a;
    tensor_view *b;
    ccd_result *result;
    int i;
    double lambda[] = {0.33, 0.33, 0.33};

    /* build tensor a */
    a = tensor_alloc(ANDIM, adim);
    for(i = 0; i < ANELEM; i++) {
	TVSET(a, aidx_list[i], a_values[i]);
    }
    printf("Tensor A\n");
    tensor_print(a, 0);
    printf("\n\n");

    /* run ccd */
    result = ccd_identity(a, 3, lambda, 1000, 0.0);

    /* display the results */
    b = ccd_construct(result);
    printf("Constructed Tensor\n");
    tensor_print(b, 2);
    printf("\n\n");
    printf("Core Tensor\n");
    tensor_print(result->core, 2);
    printf("\n\n");
    for(i=0; i<result->n; i++) {
	printf("U_%d\n", i);
	tensor_print(result->u[i], 2);
	printf("\n\n");
    }
    printf("Iterations: %d\n", result->iter);
    printf("Final Error: %lf\n", result->final_error);
    printf("Fit: %lf\n", result->fit);

    /* cleanup */
    TVFREE(a);
    TVFREE(b);
    ccd_free(result);
}
