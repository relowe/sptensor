/*
    This program pretty prints a tensor.
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
#include <sptensor.h>
#include <sptensorio.h>
#include <view.h>
#include <tensor_math.h>


int main(int argc, char **argv)
{
    sptensor *t;
    tensor_view *tv;
    int precision;
    FILE *file;

    if(argc != 2 && argc != 3) {
	fprintf(stderr, "\nUsage: %s file [precision]\n\n", argv[0]);
	exit(-1);
    }
    
    if(argc == 3) {
	precision = atoi(argv[2]);
    } else {
	precision = 2;
    }

    file = fopen(argv[1], "r");
    t = sptensor_read(file);
    tv = sptensor_view_alloc(t);
    tensor_view_print(tv, precision);
    TVFREE(tv);
    sptensor_free(t);
}
