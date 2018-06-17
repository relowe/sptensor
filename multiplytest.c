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

int main()
{
    tensor_view *a;
    tensor_view *u;
    tensor_view *b;
    int i;

    /* build tensor a */
    a = sptensor_view_alloc(sptensor_alloc(ANDIM, adim));
    for(i = 0; i < ANELEM; i++) {
	TVSET(a, aidx_list[i], a_values[i]);
    }
    printf("Tensor A\n");
    tensor_view_print(a, 0);
    printf("\n\n");

    /* build tensor u */
    u = sptensor_view_alloc(sptensor_alloc(UNDIM, udim));
    for(i = 0; i < UNELEM; i++) {
	TVSET(u, uidx_list[i], u_values[i]);
    }
    printf("Tensor U\n");
    tensor_view_print(u, 0);
    printf("\n\n");

    /* try out each product */
    for(i=0; i<ANDIM; i++) {
	printf("A x_%d U\n", i);
	b = nmode_product(i, a, u);
	tensor_view_print(b, 0);
	printf("\n\n");
    }
    
}
