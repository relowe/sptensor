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
#ifndef COMMANDS_H
#define COMMANDS_H
#include <sptensor/sptensor.h>
#include "cmdargs.h"

/* complex operations */
void cmd_ntfd(cmdargs *args);
void cmd_distance(cmdargs *args);
void cmd_norm_factor(cmdargs *args);

/* series application */
void cmd_matrix_product(cmdargs *args);
void cmd_nmode_product(cmdargs *args);
void cmd_tensor_product(cmdargs *args);
void cmd_tensor_add(cmdargs *args);
void cmd_tensor_sub(cmdargs *args);
void cmd_tensor_scale(cmdargs *args);

/* single function calls */
void cmd_lpnorm(cmdargs *args);
void cmd_slice(cmdargs *args);
void cmd_transpose(cmdargs *args);
void cmd_unfold(cmdargs *args);
void cmd_print(cmdargs *args);
void cmd_ccd_construct(cmdargs *args);

/* write command output */
void cmd_write_tensor(cmdargs *args, const char *suffix, int seq, tensor_view *v);

/* read a tensor from its filename */
tensor_view *cmd_read_tensor(char *fname);

#endif
