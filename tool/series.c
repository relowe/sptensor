/*
    Command line processing for the sptensor tool.
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

#include <sptensor/sptensor.h>
#include "cmdargs.h"
#include "commands.h"

typedef tensor_view* (*tensor_binop)(tensor_view*, tensor_view*);

static tensor_view *
tensor_series_op(vector *tv, tensor_binop op)
{
    tensor_view *result;
    tensor_view *prev;
    int i;

    /* empty lists get no tensors */
    if(tv->size == 0) return NULL;

    /* carry out the chain */
    result = tensor_view_deep_copy(VVAL(tensor_view*, tv, 0));
    for(i=1; i<tv->size; i++) {
	prev = result;
	result = (*op)(result, VVAL(tensor_view*, tv, i));
	TVFREE(prev);
    }

    return result;
}


static vector*
read_tensor_series(cmdargs *args, int start)
{
    vector *v = vector_alloc(sizeof(tensor_view*), 8);
    tensor_view *t;
    int i;

    /* read all the tensors into a vector */
    for(i=start; i<args->args->size; i++) {
	t = cmd_read_tensor(VVAL(char*, args->args, i));
	vector_push_back(v, &t);
    }

    return v;
}


static void
free_tensor_series(vector *v)
{
    int i;

    for(i=0; i<v->size; i++) {
	TVFREE(VVAL(tensor_view*, v, i));
    }
    vector_free(v);
}


static void
series_usage_check(cmdargs *args)
{
    if(args->args->size > 2) {
	fprintf(stderr,
		"\nUsage: sptensor %s tensor1 [tensor2...]\n\n",
		VVAL(char*, args->args, 0));
	exit(-1);
    }
}


static void
series_cmd(cmdargs *args, tensor_binop op)
{
    vector *v;
    tensor_view *t;

    series_usage_check(args);
    v = read_tensor_series(args, 1);
    t = tensor_series_op(v, op);

    if(t) {
	cmd_write_tensor(args, NULL, -1, t);
	TVFREE(t);
    }

    free_tensor_series(v);
}


void
cmd_matrix_product(cmdargs *args)
{
    series_cmd(args, matrix_product);
}


void
cmd_nmode_product(cmdargs *args)
{
}


void
cmd_tensor_product(cmdargs *args)
{
    series_cmd(args, tensor_product);
}


void
cmd_tensor_add(cmdargs *args)
{
    series_cmd(args, tensor_add);
}


void
cmd_tensor_sub(cmdargs *args)
{
    series_cmd(args, tensor_sub);
}


