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

#include <stdio.h>
#include <stdlib.h>
#include <sptensor/sptensor.h>
#include "cmdargs.h"
#include "commands.h"

static void single_usage_check(cmdargs *args)
{
    if(args->args->size < 2) {
	fprintf(stderr, "\nUsage: sptensor %s tensor\n\n", VVAL(char*, args->args, 0));
	exit(-1);
    }
}


void
cmd_lpnorm(cmdargs *args)
{
    tensor_view *t;

    single_usage_check(args);
    t = cmd_read_tensor(VVAL(char*, args->args, 1));
    printf("%lf\n", tensor_lpnorm(t, args->lp));
    TVFREE(t);
}


void
cmd_slice(cmdargs *args)
{
    tensor_view *t;

    
}


void
cmd_transpose(cmdargs *args)
{
    int i,j;
    tensor_view *t, *tt;

    if(args->args->size != 4) {
	fprintf(stderr, "\nUsage: sptensor transpose tensor mode1 mode2\n\n");
	exit(-1);
    }

    t = cmd_read_tensor(VVAL(char*, args->args, 1));
    i = atoi(VVAL(char*, args->args, 2));
    j = atoi(VVAL(char*, args->args, 3));
    tt = tensor_transpose(t, i, j);
    cmd_write_tensor(args, NULL, -1, tt);

    TVFREE(t);
    TVFREE(tt);
}


void
cmd_unfold(cmdargs *args)
{
    int n;
    tensor_view *t, *tu;

    if(args->args->size != 3) {
	fprintf(stderr, "\nUsage: sptensor transpose tensor mode1 mode2\n\n");
	exit(-1);
    }

    t = cmd_read_tensor(VVAL(char*, args->args, 1));
    n = atoi(VVAL(char*, args->args, 2));
    tu = unfold_tensor(t, n-1);
    cmd_write_tensor(args, NULL, -1, tu);

    TVFREE(t);
    TVFREE(tu);
}


void
cmd_print(cmdargs *args)
{
    tensor_view *t;

    single_usage_check(args);
    t = cmd_read_tensor(VVAL(char*, args->args, 1));
    cmd_write_tensor(args, NULL, -1, t);
    TVFREE(t);
}


void
cmd_ccd_construct(cmdargs *args)
{
    ccd_result ccdr;
    int i;
    char **fname;
    tensor_view *t;

    if(args->args->size<3) {
	fprintf(stderr, "\nUsage: sptensor ccd-construct core u1 [u2...un]\n\n");
	exit(-1);
    }

    /* set up the ccd result */
    ccdr.n = args->args->size - 2;
    ccdr.u = malloc(sizeof(tensor_view*)*ccdr.n);

    /* get the first tensor */
    fname = (char**)VPTR(args->args, 1);
    ccdr.core = cmd_read_tensor(*fname);
    fname++;

    /* read all the u matrices in */
    for(i=0; i<ccdr.n; i++) {
	ccdr.u[i] = cmd_read_tensor(*fname);
	fname++;
    }

    /* build the tensor */
    t = ccd_construct(&ccdr);

    /* write and cleanup */
    cmd_write_tensor(args, NULL, -1, t);
    TVFREE(t);
}


void
cmd_tensor_scale(cmdargs *args)
{
    tensor_view *t;
    double s;
    int i;

    if(args->args->size < 3) {
	fprintf(stderr, "\nUsage: sptensor tensor-scale tensor scalar\n\n");
	exit(-1);
    }

    t = cmd_read_tensor(VVAL(char*, args->args, 1));
    s = atof(VVAL(char *, args->args, 2));

    tensor_scale(t, s);
    
    /* write and cleanup */
    cmd_write_tensor(args, NULL, -1, t);
    TVFREE(t);
}
