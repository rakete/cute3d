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

#include "geometry_polygon.h"

void polygon_corner_area(size_t polygon_size, size_t point_size, float* polygon, size_t corner_i, float* result) {
    log_assert( corner_i < polygon_size );

    size_t a_i = corner_i == 0 ? polygon_size-1 : corner_i-1;
    size_t b_i = corner_i;
    size_t c_i = corner_i == polygon_size-1 ? 0 : corner_i+1;

    const VecP* a = &polygon[a_i*point_size];
    const VecP* b = &polygon[b_i*point_size];
    const VecP* c = &polygon[c_i*point_size];

    Vec3f edge_ba = {0};
    vec_sub(a, b, edge_ba);
    Vec3f edge_bc = {0};
    vec_sub(c, b, edge_bc);

    float area = 0.0;
    Vec3f edge_cross = {0};
    vec_cross(edge_ba, edge_bc, edge_cross);
    vec_length(edge_cross, &area);
    area = area/2.0f;

    *result = area;
}

size_t polygon_corner_remove(size_t polygon_size, size_t point_size, size_t type_size, void* polygon, size_t corner_i, void* result) {
    log_assert( corner_i < polygon_size );

    size_t j = 0;
    for( size_t i = 0; i < polygon_size; i++ ) {
        if( i != corner_i ) {
            void* dst = ((char*)result)+(j*point_size*type_size);
            void* src = ((char*)polygon)+(i*point_size*type_size);
            memcpy(dst, src, type_size*point_size);
            j += 1;
        }
    }

    size_t result_size = polygon_size - 1;
    return result_size;
}
