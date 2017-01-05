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

#include "io_rle.h"

int32_t main(int32_t argc, char *argv[]) {
    unsigned char input[1024];
    for( int32_t i = 0; i < 1024; i++ ) {
        input[i] = 99;
    }
    input[1023] = 100;

    uint64_t input2[2] = { 1,2 };
    unsigned char* output = NULL;

    size_t allocated = rle_encode((unsigned char*)input2, sizeof(input2), 1, 0xaa, &output);

    for( int32_t i = 0; i < allocated; i++ ) {
        printf("%d ", output[i]);
    }
    printf("\n%lu %lu\n", sizeof(input2), allocated);

    return 0;
}
