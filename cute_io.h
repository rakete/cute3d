/* cute3d, a simplistic opengl based engine written in C */
/* Copyright (C) 2013 Andreas Raster */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef CUTE_IO_H
#define CUTE_IO_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "limits.h"


#define ENGINE_ROOT "/home/rakete/interactive/cute3d/"

char* read_file(const char* filename, int32_t* length);

size_t rle_encode(unsigned char* input, size_t size, size_t bytes, unsigned char flag, unsigned char** output);

#endif
