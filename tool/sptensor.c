/*
    A sparse tensor processing tool for command line environments.
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sptensor/sptensor.h>
#include "cmdargs.h"
#include "commands.h"

/* display the usage message */
static void usage();

struct command {
    const char *name;
    const char *description;
    void (*f)(cmdargs*);
};

struct command cmdlist[]=
{
    {"ntfd",
     "Non-Negative tensor factorization",
     cmd_ntfd},
    {"ccd-construct",
     "Construct a tensor from its core and u values",
     cmd_ccd_construct},
    {"matrix-product",
     "Matrix multiply a list of tensors",
     cmd_matrix_product},
    {"nmode-product",
     "Carry out a series of nmode multiplications",
     cmd_nmode_product},
    {"tensor-product",
     "Carry out a series of tensor multiplications",
     cmd_tensor_product},
    {"tensor-add",
     "Add a list of tensors",
     cmd_tensor_add},
    {"tensor-sub",
     "Subtract a list of tensors",
     cmd_tensor_sub},
    {"tensor-scale",
     "Multiply a tensor by a scalar",
     cmd_tensor_scale},
    {"lpnorm",
     "Compute the lprnom of a tensor",
     cmd_lpnorm},
    {"slice",
     "Compute a tensor slice",
     cmd_slice},
    {"transpose",
     "Transpose two modes of a tensor",
     cmd_transpose},
    {"unfold",
     "Unfold a tensor along a dimension",
     cmd_unfold},
    {"print",
     "Pretty print a tensor",
     cmd_print},
    {"distance",
     "Compute a distance matrix for a list of tensors",
     cmd_distance},
    {"norm-factor",
     "Convert a modal matrix factorization into a series of normalized tensors",
     cmd_norm_factor}
};
static int ncmd = sizeof(cmdlist)/sizeof(cmdlist[0]);
    

int
main(int argc, char **argv)
{
    cmdargs *args;
    int valid_command;
    int i;

    /* process the command line arguments */
    args = cmdargs_parse(argc, argv);

    /* run the selected command */
    if(args->args->size == 0) usage();
    valid_command = 0;
    for(i=0; i<ncmd; i++) {
	if(!strcmp(VVAL(char*, args->args, 0), cmdlist[i].name)) {
	    (*cmdlist[i].f)(args);
	    valid_command = 1;
	    break;
	}
    }
    if(!valid_command) usage();

    /* cleanup */
    cmdargs_free(args);
}


static void
usage()
{
    int i;
    
    fprintf(stderr, "\nUsage: sptensor <options> command command-args\n\n");
    fprintf(stderr, "Options: \n");
    fprintf(stderr, "  -o filename\tOutput file name.  Defaults to stdout.\n");
    fprintf(stderr, "  -lp norm\tThe p-value to use for lpnorm.  Default: 2\n");
    fprintf(stderr, "  -fmt cl|print\tPrint format.  cl for column list print for pretty print.\n              \tDefault: cl\n");
    fprintf(stderr, "  -iter max_iter\tMaximum number of iterations Default:1000\n");
    fprintf(stderr, "  -tol tolerance\tTolerance level for convergance. Default: 1e-7\n");
    fprintf(stderr, "  -nfactors n\tNumber of factors to compute.  Default: 5\n");
    fprintf(stderr, "  -precision p\tSet numeric precision for pretty printing. Default: 0\n");
    fprintf(stderr, "\nCommands:\n");
    for(i=0; i<ncmd; i++) {
	fprintf(stderr, "  %s\t%s\n", cmdlist[i].name, cmdlist[i].description);
    }
    exit(-1);
}


void
cmd_write_tensor(cmdargs *args, const char *suffix, int seq, tensor_view *v)
{
    FILE *file;
    char *fname;
    int sufflen;
    int seqlen;

    sufflen = suffix ? strlen(suffix) : 0;
    seqlen = seq >= 0 ? ((int)ceil(log10(seq))+1):0;

    /* get the file stream */
    if(args->outfile) {
	/* compute the file name */
	if(sufflen+seqlen == 0) {
	    fname = malloc(strlen(args->outfile)+1);
	    strcpy(fname, args->outfile);
	} else {
	    fname = malloc(strlen(args->out_prefix) + seqlen + sufflen + 10);
	    if(sufflen && seqlen) {
		sprintf(fname, "%s-%s%d.tns", args->out_prefix, suffix, seq);
	    } else if(sufflen) {
		sprintf(fname, "%s-%s.tns", args->out_prefix, suffix);
	    } else if(seqlen) {
		sprintf(fname, "%s-%d.tns", args->out_prefix, seq);
	    }
	}

	/* open the file and cleanup */
	file = fopen(fname, "w");
	if(!file) {
	    fprintf(stderr, "Could not open %s for writing\n", fname);
	}
	free(fname);
    } else {
	/* use stdout */
	file = stdout;
	if(sufflen && seqlen) {
	    fprintf(file, "%s%d\n", suffix, seq);
	} else if(sufflen) {
	    fprintf(file, "%s\n", suffix);
	}
    }

    /* write the tensor in its appropriate format */
    if(args->fmt == CL) {
	tensor_view_write(file, v);
    } else {
	tensor_view_fprint(file, v, args->precision);
    }

    /* close the file (if it is not stdout) */
    if(args->outfile) fclose(file);
}


tensor_view *
cmd_read_tensor(char *fname)
{
    tensor_view *result;
    tensor_view *u;
    sptensor *stns;
    FILE *file;

    file = fopen(fname, "r");
    stns = sptensor_read(file);
    fclose(file);
    
    u = sptensor_view_alloc(stns);
    result = tensor_view_deep_copy(u);
    TVFREE(u);
    sptensor_free(stns);

    return result;

}
