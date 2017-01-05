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

enum PolygonCutResult {
    POLYGON_COPLANNAR = 0, // when poly lies on cutting plane
    POLYGON_FRONT, // when poly is entirely on front
    POLYGON_BACK, // when poly is entirely on back
    POLYGON_CUT_FROM_FRONT, // when poly is cut, and first poly vertex is on front
    POLYGON_CUT_FROM_BACK // when poly is cut, and first poly vertex is on back
};

enum PolygonCutResult polygon_cut(size_t polygon_size, size_t point_size, float* polygon,
                                  Vec3f plane_normal, Vec3f plane_point,
                                  size_t result_size, int32_t* result_cut_starts, float* result_cut_offsets)
{
    log_assert( polygon_size >= 3 );

    int32_t poly_type = POLYGON_COPLANNAR;

    float w = 0.0f;
    vec_dot(plane_normal, plane_point, &w);
#ifdef CUTE_BUILD_MSVC
    float* point_distances = _alloca(sizeof(float) * polygon_size);
#else
    float point_distances[polygon_size];
#endif
    for( size_t polygon_i = 0; polygon_i < polygon_size; polygon_i++) {
        const VecP* p = &polygon[polygon_i*point_size];

        float p_distance = 0.0f;
        vec_dot(plane_normal, p, &p_distance);
        p_distance -= w;

        if( p_distance < -CUTE_EPSILON ) {
            poly_type |= POLYGON_BACK;
        } else if( p_distance > CUTE_EPSILON ) {
            poly_type |= POLYGON_FRONT;
        } else {
            poly_type |= POLYGON_COPLANNAR;
        }

        point_distances[polygon_i] = p_distance;
    }

    size_t num_cuts = 0;
    for( size_t i = num_cuts; i < result_size; i++ ) {
        result_cut_starts[i] = -1;
        result_cut_offsets[i] = 0.0f;
    }

    return poly_type;

    /* const VecP* a = &polygon[(polygon_size-1)*point_size]; */
    /* float a_distance = 0.0f; */
    /* vec_dot(plane_normal, a, &a_distance); */
    /* a_distance -= w; */

    /* const VecP* b = &polygon[0]; */
    /* float b_distance = 0.0f; */

    /* for( size_t i = 0; i < polygon_size; i++ ) { */
    /*     b = &polygon[i*point_size]; */
    /*     b_distance = 0.0f; */
    /*     vec_dot(plane_normal, b, &b_distance); */
    /*     b_distance -= w; */

    /*     if( poly_type == POLYGON_COPLANNAR ) { */
    /*         if( a_distance < -CUTE_EPSILON ) { */
    /*             poly_type = POLYGON_BACK; */
    /*         } else if( a_distance > CUTE_EPSILON ) { */
    /*             poly_type = POLYGON_FRONT; */
    /*         } */
    /*     } */

    /*     if( (a_distance < -CUTE_EPSILON && b_distance > CUTE_EPSILON) || */
    /*         (a_distance > CUTE_EPSILON && b_distance < -CUTE_EPSILON) ) */
    /*     { */
    /*         if( num_cuts < result_size ) { */
    /*             result_cut_starts[num_cuts] = i==0 ? polygon_size-1 : i; */
    /*             result_cut_offsets[num_cuts] = fabs(a_distance)/fabs(a_distance+b_distance); */
    /*             num_cuts += 1; */
    /*         } */
    /*     } */

    /*     a = b; */
    /*     a_distance = b_distance; */
    /* } */


    /* if( num_cuts > 0 ) { */
    /*     if( poly_type == POLYGON_BACK ) { */
    /*         poly_type = POLYGON_CUT_FROM_BACK; */
    /*     } else if( poly_type == POLYGON_FRONT ) { */
    /*         poly_type = POLYGON_CUT_FROM_FRONT; */
    /*     } */
    /* } */
}

void bsp_tree_build(struct BspTree* tree, struct Solid* solid) {
    tree->attributes.vertices = malloc(solid->attributes_size * VERTEX_SIZE * sizeof(VERTEX_TYPE));
    log_assert( tree->attributes.vertices != NULL );
    tree->attributes.capacity = solid->attributes_size;

    size_t num_polygons = 0;
    for( size_t indices_i = 0; indices_i < solid->indices_size; indices_i++ ) {
        uint32_t index_i = solid->indices[indices_i];
        tree->attributes.vertices[indices_i*VERTEX_SIZE+0] = solid->vertices[index_i*VERTEX_SIZE+0];
        tree->attributes.vertices[indices_i*VERTEX_SIZE+1] = solid->vertices[index_i*VERTEX_SIZE+1];
        tree->attributes.vertices[indices_i*VERTEX_SIZE+2] = solid->vertices[index_i*VERTEX_SIZE+2];
        num_polygons += 1;
    }
    tree->attributes.occupied = num_polygons;
    num_polygons /= 3;

    struct BspPoly divider = {0};
    divider.polygon = &tree->attributes.vertices[3*6*VERTEX_SIZE];
    divider.size = 3;

    polygon_normal(divider.size, VERTEX_SIZE, divider.polygon, divider.normal);
    vec_print("divider.normal: ", divider.normal);

    draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 0, 0, 255}, 0.01f, divider.size, divider.polygon, divider.normal);
    draw_vec(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 0, 0, 255}, 0.01f, divider.normal, &divider.polygon[3], 1.0f, 0.1f);
    draw_plane(&global_static_canvas, MAX_CANVAS_LAYERS-1, (Mat)IDENTITY_MAT, (Color){255, 120, 150, 127}, divider.normal, &divider.polygon[3], 10.0f);

    for( size_t polygon_i = 0; polygon_i < num_polygons; polygon_i++ ) {
        size_t cuts_polygon_size = 3;
        float* cuts_polygon = &tree->attributes.vertices[polygon_i*cuts_polygon_size*VERTEX_SIZE];

        int32_t cuts_result_starts[3] = {0};
        float cuts_result_offsets[3] = {0};
        size_t cuts_result_size = 3;
        enum PolygonCutResult poly_type = polygon_cut(cuts_polygon_size, VERTEX_SIZE, cuts_polygon,
                                                      divider.normal, divider.polygon,
                                                      cuts_result_size, cuts_result_starts, cuts_result_offsets);

        Vec3f cuts_polygon_normal = {0};
        polygon_normal(3, VERTEX_SIZE, cuts_polygon, cuts_polygon_normal);
        switch(poly_type) {
            case POLYGON_COPLANNAR:
                draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 255, 255, 255}, 0.01f, cuts_result_size, cuts_polygon, cuts_polygon_normal);
                break;
            case POLYGON_FRONT:
                draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 0, 255, 255}, 0.01f, cuts_result_size, cuts_polygon, cuts_polygon_normal);
                break;
            case POLYGON_BACK:
                draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){0, 0, 255, 255}, 0.01f, cuts_result_size, cuts_polygon, cuts_polygon_normal);
                break;
            case POLYGON_CUT_FROM_FRONT:
                draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 255, 0, 255}, 0.01f, cuts_result_size, cuts_polygon, cuts_polygon_normal);
                for( size_t i = 0; i < cuts_result_size; i++ ) {
                    if( cuts_result_starts[i] > -1 ) {
                        int32_t a_i = cuts_result_starts[i];
                        int32_t b_i = a_i >= (int32_t)cuts_polygon_size-1 ? 0 : a_i+1;
                        printf("%d %d\n", a_i, b_i);
                        Vec3f u = {0};
                        vec_sub(&cuts_polygon[b_i*VERTEX_SIZE], &cuts_polygon[a_i*VERTEX_SIZE], u);
                        //vec_normalize(u, u);
                        //vec_mul1f(u, cuts_result_offsets[i], u);
                        draw_vec(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 0, 0, 255}, 0.05f, u, &cuts_polygon[a_i*VERTEX_SIZE], 1.0f, 1.0f);
                    }
                }
                break;
            case POLYGON_CUT_FROM_BACK:
                draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){0, 255, 255, 255}, 0.01f, cuts_result_size, cuts_polygon, cuts_polygon_normal);
                break;
        }
    }

    log_fail(__FILE__, __LINE__, "BUILT BSP TREE... OR NOT?\n");
}
