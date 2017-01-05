/* Cute3D, a simple opengl based framework for writing interactive realtime applications */

/* Copyright (C) 2013-2017 Andreas Raster */

/* This file is part of Cute3D. */

/* Cute3D is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* Cute3D is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with Cute3D.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef IO_PATH_H
#define IO_PATH_H

#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

#include "driver_log.h"

#define PATH_SEPERATOR '/'

bool path_file_exists(const char* path);

bool path_search_path(char* search_path, const char* filename, size_t found_path_size, char* found_path);

#endif
