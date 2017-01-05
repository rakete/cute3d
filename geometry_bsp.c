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

void bsp_node_create(struct BspNode* node) {
    node->divider = 0;

    node->bounds.half_width = 0.0f;
    node->bounds.half_height = 0.0f;
    node->bounds.half_depth = 0.0f;
    node->bounds.center[0] = 0.0f;
    node->bounds.center[1] = 0.0f;
    node->bounds.center[2] = 0.0f;

    node->tree.parent = -1;
    node->tree.front = -1;
    node->tree.back = -1;

    node->state.empty = false;
    node->state.solid = false;
    node->state.disabled = false;
}

WARN_UNUSED_RESULT size_t bsp_tree_alloc_attributes(struct BspTree* tree, size_t n) {
    size_t attributes_alloc = BSP_ATTRIBUTES_ALLOC;
    while( attributes_alloc < n ) {
        attributes_alloc += BSP_ATTRIBUTES_ALLOC;
    }

    float* new_array = realloc(tree->attributes.vertices, attributes_alloc * VERTEX_SIZE * sizeof(VERTEX_TYPE));
    if( new_array != NULL ) {
        tree->attributes.vertices = new_array;
        tree->attributes.capacity += attributes_alloc;
        return attributes_alloc;
    }

    return 0;
}

WARN_UNUSED_RESULT size_t bsp_tree_alloc_polygons(struct BspTree* tree, size_t n) {
    size_t polygons_alloc = BSP_POLYGONS_ALLOC;
    while( polygons_alloc < n ) {
        polygons_alloc += BSP_POLYGONS_ALLOC;
    }

    struct BspPoly* new_array = realloc(tree->polygons.array, polygons_alloc * sizeof(struct BspPoly));
    if( new_array != NULL ) {
        tree->polygons.array = new_array;
        tree->polygons.capacity += polygons_alloc;
        return polygons_alloc;
    }

    return 0;
}

WARN_UNUSED_RESULT size_t bsp_tree_alloc_nodes(struct BspTree* tree, size_t n) {
    size_t nodes_alloc = BSP_NODES_ALLOC;
    while( nodes_alloc < n ) {
        nodes_alloc += BSP_NODES_ALLOC;
    }

    struct BspNode* new_array = realloc(tree->nodes.array, nodes_alloc * sizeof(struct BspNode));
    if( new_array != NULL ) {
        tree->nodes.array = new_array;
        tree->nodes.capacity += nodes_alloc;
        return nodes_alloc;
    }

    return 0;
}

enum BspDividerHeuristic {
    BSP_MAXIMIZE_BALANCE = 0,
    BSP_MINIMIZE_SPLITS
};

void bsp_select_balanced_divider(const struct BspTree* tree, const struct BspNode* node, size_t num_polygons, const int32_t* polygon_indices, int32_t* selected_divider) {
    *selected_divider = 0;

    float node_width = node->bounds.half_width*2.0f;
    float node_height = node->bounds.half_height*2.0f;
    float node_depth = node->bounds.half_depth*2.0f;

    Vec3f normal_comparison_axis = {0};
    if( node_width <= node_height && node_width <= node_depth ) {
        vec_copy3f((Vec4f)X_AXIS, normal_comparison_axis);
    } else if( node_height <= node_width && node_height <= node_depth ) {
        vec_copy3f((Vec4f)Y_AXIS, normal_comparison_axis);
    } else if( node_depth <= node_width && node_depth <= node_height ) {
        vec_copy3f((Vec4f)Z_AXIS, normal_comparison_axis);
    }

    float min_dot = FLT_MAX;
    Vec3f min_diff_center = {FLT_MAX, FLT_MAX, FLT_MAX};
    int32_t best_i = -1;
    for( size_t polygon_i = 0; polygon_i < num_polygons; polygon_i++ ) {
        int32_t index_i = polygon_indices[polygon_i];
        int32_t start_i = tree->polygons.array[index_i].start;

        float dot = FLT_MAX;
        vec_dot(tree->polygons.array[index_i].normal, normal_comparison_axis, &dot);
        if( fabs(dot) < min_dot ) {
            min_dot = fabs(dot);
            best_i = polygon_i;
        }

        Vec3f average_vertex = {0.0f, 0.0f, 0.0f};
        for( size_t size_i = 0; size_i < tree->polygons.array[index_i].size; size_i++ ) {

        }
    }
}

void bsp_tree_create_from_solid(struct BspTree* tree, struct Solid* solid) {
    size_t alloc_attributes_result = bsp_tree_alloc_attributes(tree, solid->attributes_size);
    log_assert( alloc_attributes_result >= solid->attributes_size );

    size_t num_polygons = solid->indices_size/3;
    size_t alloc_polygons_result = bsp_tree_alloc_polygons(tree, num_polygons);
    log_assert( alloc_polygons_result >= num_polygons );

    size_t alloc_nodes_result = bsp_tree_alloc_nodes(tree, num_polygons);
    log_assert( alloc_nodes_result >= num_polygons );

    int32_t* workset_polygons_front = malloc(alloc_polygons_result * sizeof(int32_t));
    log_assert( workset_polygons_front != NULL );
    int32_t* workset_polygons_back = malloc(alloc_polygons_result * sizeof(int32_t));
    log_assert( workset_polygons_back != NULL );

    float min_x = FLT_MAX;
    float min_y = FLT_MAX;
    float min_z = FLT_MAX;
    float max_x = -FLT_MAX;
    float max_y = -FLT_MAX;
    float max_z = -FLT_MAX;
    for( size_t indices_i = 0; indices_i < solid->indices_size+1; indices_i++ ) {
        uint32_t src_i = solid->indices[indices_i];

        if( indices_i < solid->indices_size ) {
            VecP* src = &solid->vertices[src_i*VERTEX_SIZE];
            VecP* dst = &tree->attributes.vertices[indices_i*VERTEX_SIZE];
            vec_copy3f(src, dst);
            tree->attributes.occupied += 1;

            if( src[0] < min_x ) {
                min_x = src[0];
            }
            if( src[1] < min_y ) {
                min_y = src[1];
            }
            if( src[2] < min_z ) {
                min_z = src[2];
            }

            if( src[0] > max_x ) {
                max_x = src[0];
            }
            if( src[1] > max_y ) {
                max_y = src[1];
            }
            if( src[2] > max_z ) {
                max_z = src[2];
            }
        }

        if( indices_i > 0 && indices_i % 3 == 0 ) {
            size_t poly_i = indices_i / 3 - 1;
            tree->polygons.array[poly_i].start = poly_i*3*VERTEX_SIZE;
            tree->polygons.array[poly_i].size = 3;
            polygon_normal(3, VERTEX_SIZE, &tree->attributes.vertices[poly_i*3*VERTEX_SIZE], tree->polygons.array[poly_i].normal);
            tree->polygons.occupied += 1;

            workset_polygons_front[poly_i] = poly_i;
        }

    }

    struct BspNode* root = &tree->nodes.array[0];
    bsp_node_create(root);
    tree->nodes.occupied = 1;
    root->bounds.half_width = (max_x - min_x)/2.0f;
    root->bounds.half_height = (max_y - min_y)/2.0f;
    root->bounds.half_depth = (max_z - min_z)/2.0f;
    root->bounds.center[0] = min_x + root->bounds.half_width;
    root->bounds.center[1] = min_y + root->bounds.half_height;
    root->bounds.center[2] = min_z + root->bounds.half_depth;

    root->divider = 0;    bsp_select_balanced_divider(tree, root, num_polygons, workset_polygons_front, &root->divider);

    struct BspPoly* root_divider = &tree->polygons.array[root->divider];
    const float* root_divider_polygon = &tree->attributes.vertices[root_divider->start];

    draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 0, 0, 255}, 0.01f, root_divider->size, root_divider_polygon, root_divider->normal);
    draw_vec(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 0, 0, 255}, 0.01f, root_divider->normal, &root_divider_polygon[3], 1.0f, 0.1f);
    /* draw_plane(&global_static_canvas, MAX_CANVAS_LAYERS-1, (Mat)IDENTITY_MAT, (Color){120, 120, 150, 127}, root_divider->normal, &root_divider_polygon[3], 10.0f); */

    for( size_t polygon_i = 0; polygon_i < num_polygons; polygon_i++ ) {
        size_t cuts_polygon_size = 3;
        const float* cuts_polygon = &tree->attributes.vertices[polygon_i*cuts_polygon_size*VERTEX_SIZE];

        size_t result_size = cuts_polygon_size;
        struct PolygonCutPoints result_points[cuts_polygon_size];
        enum PolygonCutType result_type = polygon_cut(cuts_polygon_size, VERTEX_SIZE, cuts_polygon,
                                                      root_divider->normal, root_divider_polygon,
                                                      result_size, result_points);

        Vec3f cuts_polygon_normal = {0};
        polygon_normal(3, VERTEX_SIZE, cuts_polygon, cuts_polygon_normal);
        switch(result_type) {
            case POLYGON_COPLANNAR:
                //draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 255, 255, 255}, 0.01f, result_size, cuts_polygon, cuts_polygon_normal);
                break;
            case POLYGON_FRONT:
                //draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 0, 255, 255}, 0.01f, result_size, cuts_polygon, cuts_polygon_normal);
                break;
            case POLYGON_BACK:
                //draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){0, 0, 255, 255}, 0.01f, result_size, cuts_polygon, cuts_polygon_normal);
                break;
            case POLYGON_SPANNING:
                //draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 255, 0, 255}, 0.01f, result_size, cuts_polygon, cuts_polygon_normal);

                if( result_points[0].num_cuts > 0 ) {
                    size_t new_poly_size = cuts_polygon_size+result_points[0].num_cuts+10;

                    size_t front_occupied = 0;
                    float front_vertices[new_poly_size*VERTEX_SIZE];

                    size_t back_occupied = 0;
                    float back_vertices[new_poly_size*VERTEX_SIZE];

                    for( size_t result_i = 0; result_i < result_size; result_i++ ) {
                        if( result_points[result_i].type == POLYGON_BACK ) {
                            vec_copy3f(&cuts_polygon[result_i*VERTEX_SIZE], &back_vertices[back_occupied*VERTEX_SIZE]);
                            back_occupied += 1;
                        } else if( result_points[result_i].type == POLYGON_FRONT ) {
                            vec_copy3f(&cuts_polygon[result_i*VERTEX_SIZE], &front_vertices[front_occupied*VERTEX_SIZE]);
                            front_occupied += 1;
                        } else if( result_points[result_i].type == POLYGON_COPLANNAR ) {
                            vec_copy3f(&cuts_polygon[result_i*VERTEX_SIZE], &back_vertices[back_occupied*VERTEX_SIZE]);
                            back_occupied += 1;
                            vec_copy3f(&cuts_polygon[result_i*VERTEX_SIZE], &front_vertices[front_occupied*VERTEX_SIZE]);
                            front_occupied += 1;
                        }

                        if( result_points[result_i].interpolation_index > -1 ) {
                            const VecP* a = &cuts_polygon[result_i*VERTEX_SIZE];
                            const VecP* b = &cuts_polygon[result_points[result_i].interpolation_index*VERTEX_SIZE];
                            Vec3f r = {0};
                            vec_lerp(b, a, result_points[result_i].interpolation_value, r);

                            vec_copy3f(r, &back_vertices[back_occupied*VERTEX_SIZE]);
                            back_occupied += 1;
                            vec_copy3f(r, &front_vertices[front_occupied*VERTEX_SIZE]);
                            front_occupied += 1;
                        }
                    }

                    //printf("front_occupied: %lu\n", front_occupied);
                    //draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT, red, 0.01f, front_occupied, front_vertices, cuts_polygon_normal);
                    //draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT, white, 0.01f, back_occupied, back_vertices, cuts_polygon_normal);
                }
                break;
        }
    }

    log_fail(__FILE__, __LINE__, "BUILT BSP TREE... OR NOT?\n");
}
