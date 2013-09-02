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

#include "io.h"

char* read_file(const char* filename, uint64_t* length) {
    FILE* file = fopen(filename, "rb");

    if( ! file ) {
        char* path = malloc(strlen(ENGINE_ROOT) + strlen(filename) + 1);
        sprintf(path, "%s%s\0", ENGINE_ROOT, filename);
        file = fopen(path, "rb");
    }

    if( ! file ) {
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    *length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* contents = malloc(*length);
    fread(contents, *length, 1, file);
    fclose(file);
    
    return contents;
}
