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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cmdargs.h"

static char *
extract_prefix(char *str)
{
    int end;  /* bounds of the core name in the string */
    int i;
    char *name;

    /* null pointers get no prefix */
    if(!str) return NULL;

    /* find the last . if there is one */
    for(i=strlen(str)-1; i>=0; i--) {
	if(str[i] == '.') {
	    break;
	}
    }
    end = i;
    if(end == 0) {
	end = strlen(str);
    }

    /* compute length of the new string and allocate it */
    name = malloc(sizeof(end+2));

    /* produce and return the result */
    strncpy(name, str, end);
    name[end] = '\0';
    return name;
}


cmdargs *
cmdargs_parse(int argc, char **argv)
{
    cmdargs *args;
    int i;

    /* allocate the struct and set up the defaults */
    args = malloc(sizeof(cmdargs));
    args->outfile = NULL;
    args->out_prefix = NULL;
    args->lp = 2.0;
    args->fmt = CL;
    args->iter = 1000;
    args->tol = 1e-7;
    args->nfactors = 5;
    args->precision = 0;
    args->args = vector_alloc(sizeof(char*), 8);

    /* run through the list of arguments, processing and pushing as we go */
    for(i=1; i<argc; i++) {
	if(!strcmp(argv[i], "-o")) {
	    args->outfile = argv[++i];
	    args->out_prefix = extract_prefix(args->outfile);
	} else if(!strcmp(argv[i], "-lp")) {
	    args->lp = atoi(argv[++i]);
	} else if(!strcmp(argv[i], "-fmt")) {
	    args->fmt = strcmp(argv[++i], "print") ? CL : PRINT;
	} else if(!strcmp(argv[i], "-iter")) {
	    args->iter = atoi(argv[++i]);
	} else if(!strcmp(argv[i], "-tol")) {
	    args->tol = atof(argv[++i]);
	} else if(!strcmp(argv[i], "-nfactors")) {
	    args->nfactors = atoi(argv[++i]);
	} else if(!strcmp(argv[i], "-precision")) {
	    args->precision = atoi(argv[++i]);
	} else {
	    vector_push_back(args->args, &argv[i]);
	}
    }
    return args;
}


void
cmdargs_free(cmdargs *args)
{
    if(args->out_prefix) {
	free(args->out_prefix);
    }
    vector_free(args->args);
    free(args);
}
