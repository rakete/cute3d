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

void polygon_corner_area(size_t polygon_size, size_t point_size, const float* polygon, size_t corner_i, float* result) {
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

size_t polygon_corner_remove(size_t polygon_size, size_t point_size, size_t type_size, const void* polygon, size_t corner_i, void* result) {
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

void polygon_normal(size_t polygon_size, size_t point_size, const float* polygon, Vec3f result_normal) {
    log_assert( polygon_size >= 3 );

    const VecP* a = &polygon[0*point_size];
    const VecP* b = &polygon[1*point_size];
    const VecP* c = &polygon[2*point_size];

    Vec3f edge_ba = {0};
    vec_sub(a, b, edge_ba);

    Vec3f edge_bc = {0};
    vec_sub(c, b, edge_bc);

    vec_cross(edge_bc, edge_ba, result_normal);
    vec_normalize(result_normal, result_normal);
}

enum PolygonCutType polygon_cut(size_t polygon_size, size_t point_size, const float* polygon,
                                const Vec3f plane_normal, const Vec3f plane_point,
                                size_t result_size, struct PolygonCutPoints* result_points)
{
    log_assert( polygon_size >= 3 );
    log_assert( result_size >= polygon_size );

    enum PolygonCutType return_type = POLYGON_COPLANNAR;

    float w = 0.0f;
    vec_dot(plane_normal, plane_point, &w);
    for( size_t polygon_i = 0; polygon_i < polygon_size; polygon_i++) {
        const VecP* p = &polygon[polygon_i*point_size];

        float p_distance = 0.0f;
        vec_dot(plane_normal, p, &p_distance);
        p_distance -= w;

        result_points[polygon_i].interpolation_index = -1;
        result_points[polygon_i].interpolation_value = 0.0f;
        result_points[polygon_i].num_cuts = 0;
        result_points[polygon_i].distance = 0.0f;
        result_points[polygon_i].type = POLYGON_COPLANNAR;

        if( p_distance < -CUTE_EPSILON ) {
            result_points[polygon_i].type = POLYGON_BACK;
        } else if( p_distance > CUTE_EPSILON ) {
            result_points[polygon_i].type = POLYGON_FRONT;
        } else {
            result_points[polygon_i].type = POLYGON_COPLANNAR;
        }
        return_type |= result_points[polygon_i].type;

        result_points[polygon_i].distance = p_distance;
    }

    if( return_type == POLYGON_SPANNING ) {
        for( size_t polygon_i = 0; polygon_i < polygon_size; polygon_i++ ) {
            size_t polygon_j = (polygon_i+1) % polygon_size;

            enum PolygonCutType type_i = result_points[polygon_i].type;
            enum PolygonCutType type_j = result_points[polygon_j].type;
            if( (type_i | type_j) == POLYGON_SPANNING ) {
                result_points[polygon_i].interpolation_index = polygon_j;

                float distance_i = result_points[polygon_i].distance;
                float distance_j = result_points[polygon_j].distance;

                result_points[polygon_i].interpolation_value = fabs(distance_i)/(fabs(distance_i)+fabs(distance_j));
                log_assert( result_points[polygon_i].interpolation_value >= 0.0f );
                log_assert( result_points[polygon_i].interpolation_value <= 1.0f );

                result_points[polygon_i].num_cuts = result_points[0].num_cuts + 1;
                result_points[0].num_cuts = result_points[polygon_i].num_cuts;
            }
        }
    }

    return return_type;
}

void polygon_triangulate(size_t polygon_size, size_t point_size, const float* polygon, size_t result_size, size_t* result) {

}
