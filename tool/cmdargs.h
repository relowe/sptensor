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
#ifndef CMDARGS_H
#define CMDARGS_H
#include <sptensor/vector.h>

/* 
 * A structure for holding command line arguments for sptensor
 */
typedef struct cmdargs
{
    char *outfile;    /* NULL if we are using stdout */
    char *out_prefix; /* Prefix of the outfile.  NULL if we are using stdout */
    double lp;        /* p-value for lpnorm functions */
    enum {CL, PRINT} fmt; /* print format */
    int iter;         /* maximum iterations */
    double tol;       /* tolerance level */
    int nfactors;     /* number of factors */
    int precision;    /* pretty print precision */
    vector *args;     /* additional arguments (not captured here) */
} cmdargs;


/* parse the command line and return a cmd args struct */
cmdargs *cmdargs_parse(int argc, char **argv);

/* free command arguments */
void cmdargs_free(cmdargs *args);
#endif
