/*
    Library parameters.
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


/* Maximum memory to use for dtensors (default 100 MB) */
unsigned int sptensor_max_memory = 100 * 1024 * 1024;

/* 1 for verbose output, 0 for no output (default 0) */
unsigned int sptensor_verbose = 0;

/* default norm level to use in tensor algorithms (default 2) */
int stpensor_default_lp = 2;
