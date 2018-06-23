/*
  This program tests sptensor's arithmetic functions.

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
#include <view.h>
#include <tensor_math.h>

#define ARSIZE(a) (sizeof(a)/sizeof(a[0]))

/* set up a couple of 3x2 tensors */
sp_index_t adim[] = {3,2};
sp_index_t aidx_list[][2] = {{1,1},
			     {1,2},
			     {3,2}};
double a_values[] = {6, 7, 8};
#define ANELEM ARSIZE(a_values)
#define ANDIM ARSIZE(adim)
sp_index_t bdim[] = {3,2};
sp_index_t bidx_list[][2] = {{2,1},
			     {2,2},
			     {3,2}};
double b_values[] = {6, 7, 8};
#define BNELEM ARSIZE(b_values)
#define BNDIM ARSIZE(bdim)


int main()
{
    tensor_view *a, *b;  /* primary tensor view */
    tensor_view *c;      /* another one for results */
    int i;

    /* build a and b */
    a = sptensor_view_tensor_alloc(ANDIM, adim);
    for(i=0; i<ANELEM; i++) {
	TVSET(a, aidx_list[i], a_values[i]);
    }
    b = sptensor_view_tensor_alloc(BNDIM, bdim);
    for(i=0; i<BNELEM; i++) {
	TVSET(b, bidx_list[i], b_values[i]);
    }

    /* print a and b */
    printf("A\n");
    tensor_view_print(a, 0);
    printf("\n\n");
    printf("B\n");
    tensor_view_print(b, 0);
    printf("\n\n");

    /* test addition */
    printf("A+B\n");
    c = tensor_add(a, b);
    tensor_view_print(c, 0);
    printf("\n\n");
    TVFREE(c);

    /* test subtraction */
    printf("A-B\n");
    c = tensor_sub(a, b);
    tensor_view_print(c, 0);
    printf("\n\n");
    TVFREE(c);

    /* test scalar multiplication */
    printf("3 * A\n");
    c = scalar_mul(a, 3);
    tensor_view_print(c, 0);
    printf("\n\n");
    TVFREE(c);
    
    /* test scalar division */
    printf("A/3\n");
    c = scalar_div(a, 3);
    tensor_view_print(c, 0);
    printf("\n\n");
    TVFREE(c);

    /* test the norms */
    for(i=1; i<=3; i++) {
	printf("L-%d norm of A: %lf\n", i, tensor_lpnorm(a, i));
    }

}


