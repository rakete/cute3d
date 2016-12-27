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

#include "geometry_bsp.h"

void polygon_normal(size_t polygon_size, size_t point_size, float* polygon, Vec3f result_normal) {
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

// - returns -1 when first vertex on backside, returns 1 when first vertex on frontside
// - first on backside means first cut goes from back to front
// - result will be filled with -1, when results contain int >0 then means cuts
// at those indices in polygon
// - returns 0 when coplanar
int32_t polygon_cut(size_t polygon_size, size_t point_size, float* polygon,
                    Vec3f plane_normal, Vec3f plane_point,
                    size_t result_size, int32_t* result_cut_starts, float* result_cut_offsets)
{
    log_assert( polygon_size >= 3 );

    int32_t poly_coplanar = 0;
    int32_t poly_front = 1;
    int32_t poly_back = 2;
    int32_t poly_spanning = 3;

    int32_t ret_code = 0;

    float w = 0.0f;
    vec_dot(plane_normal, plane_point, &w);

    const VecP* a = &polygon[(polygon_size-1)*point_size];
    float a_distance = 0.0f;
    vec_dot(plane_normal, a, &a_distance);
    a_distance -= w;

    int32_t poly_type = poly_coplanar;
    if( a_distance < CUTE_EPSILON ) {
        poly_type = poly_back;
    } else if( a_distance > CUTE_EPSILON ) {
        poly_type = poly_front;
    }

    const VecP* b = &polygon[0];
    float b_distance = 0.0f;
    for( size_t i = 0; i < polygon_size; i++ ) {
        b = &polygon[i*point_size];
        b_distance = 0.0f;
        vec_dot(plane_normal, b, &b_distance);
        b_distance -= w;

        if( (a_distance < CUTE_EPSILON && b_distance > CUTE_EPSILON) ||
            (a_distance > CUTE_EPSILON && b_distance < CUTE_EPSILON) )
        {
            printf("%f %f\n", a_distance, b_distance);
        }

        a = b;
        a_distance = b_distance;
    }

    return 0;
}


void bsp_tree_build(struct BspTree* tree, struct Solid* solid) {
    tree->attributes.vertices = malloc(solid->indices_size * VERTEX_SIZE * sizeof(VERTEX_TYPE));
    tree->attributes.capacity = solid->indices_size;
    tree->attributes.occupied = 0;

    size_t num_polygons = 0;
    for( size_t indices_i = 0; indices_i < solid->indices_size; indices_i++ ) {
        uint32_t index_i = solid->indices[indices_i];
        tree->attributes.vertices[indices_i*VERTEX_SIZE+0] = solid->vertices[index_i*VERTEX_SIZE+0];
        tree->attributes.vertices[indices_i*VERTEX_SIZE+1] = solid->vertices[index_i*VERTEX_SIZE+1];
        tree->attributes.vertices[indices_i*VERTEX_SIZE+2] = solid->vertices[index_i*VERTEX_SIZE+2];
        num_polygons += 1;
    }
    num_polygons /= 3;

    struct BspPoly divider = {0};
    divider.polygon = tree->attributes.vertices;
    divider.size = 3;

    polygon_normal(divider.size, VERTEX_SIZE, divider.polygon, divider.normal);
    vec_print("divider.normal: ", divider.normal);

    //draw_solid_triangle(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 0, 0, 255}, 0.005f, solid, divider.polygon);
    //draw_vec(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 0, 0, 255}, 0.005f, divider.normal, &divider.polygon[3], 1.0f, 0.1f);

    for( size_t polygon_i = 1; polygon_i < num_polygons; polygon_i++ ) {
        int32_t cut_starts[2] = {0};
        float cut_offsets[2] = {0};
        polygon_cut(3, VERTEX_SIZE, &tree->attributes.vertices[polygon_i*3*VERTEX_SIZE],
                    divider.normal, divider.polygon,
                    3, cut_starts, cut_offsets);
    }

    log_fail(__FILE__, __LINE__, "BUILT BSP TREE... OR NOT?\n");
}
