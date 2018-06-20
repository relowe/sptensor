/*
  This program tests sptensor's multiply functions.

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
#include <multiply.h>

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

/* set up a 3x2 */
sp_index_t udim[] = {3,2};
sp_index_t uidx_list[][2] = {{1,1},
			     {2,1},
			     {3,1},
			     {1,2},
			     {2,2},
			     {3,2}};
double u_values[] = {6, 7, 8, 9, 10, 11};
#define UNELEM ARSIZE(u_values)
#define UNDIM ARSIZE(udim)


/* set up 2 1-mode tensors */
sp_index_t v1dim[] = {5};
sp_index_t v1idx_list[][1] = {1, 3, 4, 5};
double v1_values[] = {1.0, 2.0, 3.0, 5.0};
#define V1NELEM ARSIZE(v1_values)
#define V1NDIM ARSIZE(v1dim)
sp_index_t v2dim[] = {10};
sp_index_t v2idx_list[][1] = {5, 6, 7};
double v2_values[] = {7.0, 11.0, 13.0};
#define V2NELEM ARSIZE(v2_values)
#define V2NDIM ARSIZE(v2dim)


int main()
{
    tensor_view *a;
    tensor_view *u;
    tensor_view *b;
    tensor_view *m1, *m2;
    tensor_view *v1, *v2;
    tensor_view *c;
    tensor_slice_spec *slice;
    int i;

    /* build tensor a */
    a = sptensor_view_tensor_alloc(ANDIM, adim);
    for(i = 0; i < ANELEM; i++) {
	TVSET(a, aidx_list[i], a_values[i]);
    }
    printf("Tensor A\n");
    tensor_view_print(a, 0);
    printf("\n\n");

    /* build tensor u */
    u = sptensor_view_tensor_alloc(UNDIM, udim);
    for(i = 0; i < UNELEM; i++) {
	TVSET(u, uidx_list[i], u_values[i]);
    }
    printf("Tensor U\n");
    tensor_view_print(u, 0);
    printf("\n\n");

    /* build v1 and v2 */
    v1 = sptensor_view_tensor_alloc(V1NDIM, v1dim);
    for(i = 0; i < V1NELEM; i++) {
	TVSET(v1, v1idx_list[i], v1_values[i]);
    }
    v2 = sptensor_view_tensor_alloc(V2NDIM, v2dim);
    for(i = 0; i < V2NELEM; i++) {
	TVSET(v2, v2idx_list[i], v2_values[i]);
    }


    /* try out each mode as a product */
    for(i=0; i<ANDIM; i++) {
	printf("A x_%d U\n", i);
	b = nmode_product(i, a, u);
	tensor_view_print(b, 0);
	printf("\n\n");
	TVFREE(b);
    }

    /* test out matrix multiplication */
    slice = tensor_slice_spec_alloc(a);
    slice->fixed[2] = 1;
    m1 = tensor_slice(a, slice);
    slice->fixed[2] = 2;
    m2 = tensor_slice(a, slice);
    printf("m1\n");
    tensor_view_print(m1, 0);
    printf("\n\n");
    printf("m2\n");
    tensor_view_print(m2, 0);
    printf("\n\n");
    printf("m1 x m2\n");
    b = matrix_product(m1, m2);
    tensor_view_print(b, 0);
    printf("\n\n");

    /* tensor product */
    printf("V1\n");
    tensor_view_print(v1, 0);
    printf("V2\n");
    tensor_view_print(v2, 0);
    printf("V1 xt V2\n");
    c = tensor_product(v1, v2);
    tensor_view_print(c, 0);
    printf("\n\n");

    /* cleanup! */
    tensor_slice_spec_free(slice);
    TVFREE(m1);
    TVFREE(m2);
    TVFREE(a);
    TVFREE(u);
    TVFREE(v1);
    TVFREE(v2);
    TVFREE(c);
}
