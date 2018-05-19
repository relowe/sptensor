/*
    sptensortest: This program tests the sptensor functions.
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
#include <sptensorio.h>
#include <stdlib.h>
#include <time.h>

int
main(int argc, char **argv)
{
    sptensor *sp;
    FILE *file;

    /* read the tensor */
    file = fopen(argv[1], "r");
    sp = sptensor_read(file);
    fclose(file);

    /* print the tensor */
    sptensor_write(stdout, sp);
}
