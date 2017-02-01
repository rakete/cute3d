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

void bsp_polygon_create(struct BspPolygon* polygon) {
    polygon->start = 0;
    polygon->size = 0;

    polygon->normal[0] = 0.0f;
    polygon->normal[1] = 0.0f;
    polygon->normal[2] = 0.0f;

    polygon->divider = -1;

    polygon->cut.parent = -1;
    polygon->cut.sibling = -1;
}

void bsp_node_bounds_create(Vec3f min, Vec3f max, size_t num_polygons, struct BspNodeBounds* bounds) {
    bounds->half_width = (max[0] - min[0])/2.0f;
    bounds->half_height = (max[1] - min[1])/2.0f;
    bounds->half_depth = (max[2] - min[2])/2.0f;
    bounds->center[0] = min[0] + bounds->half_width;
    bounds->center[1] = min[1] + bounds->half_height;
    bounds->center[2] = min[2] + bounds->half_depth;
    bounds->num_polygons = num_polygons;
}

void bsp_node_create(struct BspNodeBounds bounds, struct BspNode* node) {
    node->divider = 0;

    node->bounds.half_width = bounds.half_width;
    node->bounds.half_height = bounds.half_height;
    node->bounds.half_depth = bounds.half_depth;
    node->bounds.center[0] = bounds.center[0];
    node->bounds.center[1] = bounds.center[1];
    node->bounds.center[2] = bounds.center[2];
    node->bounds.num_polygons = bounds.num_polygons;

    node->tree.parent = -1;
    node->tree.front = -1;
    node->tree.back = -1;
    node->tree.index = 0;
    node->tree.depth = 0;

    node->state.empty = false;
    node->state.solid = false;
    node->state.disabled = false;
}

WARN_UNUSED_RESULT size_t bsp_tree_alloc_attributes(struct BspTree* tree, size_t n) {
    if( n == 0 ) {
        return 0;
    }

    size_t attributes_alloc = BSP_ATTRIBUTES_ALLOC;
    while( attributes_alloc < n ) {
        attributes_alloc += BSP_ATTRIBUTES_ALLOC;
    }

    size_t new_capacity = tree->attributes.capacity + attributes_alloc;

    VERTEX_TYPE* new_vertices_array = realloc(tree->attributes.vertices, new_capacity * VERTEX_SIZE * sizeof(VERTEX_TYPE));
    NORMAL_TYPE* new_normals_array = realloc(tree->attributes.normals, new_capacity * NORMAL_SIZE * sizeof(NORMAL_TYPE));
    TEXCOORD_TYPE* new_texcoords_array = realloc(tree->attributes.texcoords, new_capacity * TEXCOORD_SIZE * sizeof(TEXCOORD_TYPE));
    COLOR_TYPE* new_colors_array = realloc(tree->attributes.colors, new_capacity * COLOR_SIZE * sizeof(COLOR_TYPE));
    if( new_vertices_array != NULL &&
        new_normals_array != NULL &&
        new_texcoords_array != NULL &&
        new_colors_array != NULL )
    {
        tree->attributes.vertices = new_vertices_array;
        tree->attributes.normals = new_normals_array;
        tree->attributes.texcoords = new_texcoords_array;
        tree->attributes.colors = new_colors_array;
        tree->attributes.capacity = new_capacity;
        return attributes_alloc;
    }

    return 0;
}

WARN_UNUSED_RESULT size_t bsp_tree_alloc_polygons(struct BspTree* tree, size_t n) {
    if( n == 0 ) {
        return 0;
    }

    size_t polygons_alloc = BSP_POLYGONS_ALLOC;
    while( polygons_alloc < n ) {
        polygons_alloc += BSP_POLYGONS_ALLOC;
    }

    size_t new_capacity = tree->polygons.capacity + polygons_alloc;

    struct BspPolygon* new_array = realloc(tree->polygons.array, new_capacity * sizeof(struct BspPolygon));
    if( new_array != NULL ) {
        tree->polygons.array = new_array;
        tree->polygons.capacity = new_capacity;
        return polygons_alloc;
    }

    return 0;
}

WARN_UNUSED_RESULT size_t bsp_tree_alloc_nodes(struct BspTree* tree, size_t n) {
    if( n == 0 ) {
        return 0;
    }

    size_t nodes_alloc = BSP_NODES_ALLOC;
    while( nodes_alloc < n ) {
        nodes_alloc += BSP_NODES_ALLOC;
    }

    size_t new_capacity = tree->nodes.capacity + nodes_alloc;

    struct BspNode* new_array = realloc(tree->nodes.array, new_capacity * sizeof(struct BspNode));
    if( new_array != NULL ) {
        tree->nodes.array = new_array;
        tree->nodes.capacity = new_capacity;
        return nodes_alloc;
    }

    return 0;
}

int32_t bsp_tree_add_node(struct BspTree* tree, struct BspNodeBounds bounds, struct BspNode** result) {
    if( tree->nodes.occupied + 1 >= tree->nodes.capacity ) {
        size_t alloc_nodes_result = 0;
        alloc_nodes_result = bsp_tree_alloc_nodes(tree, 1);
        log_assert( alloc_nodes_result >= 1 );
    }

    size_t node_i = tree->nodes.occupied;
    struct BspNode* node = &tree->nodes.array[node_i];
    bsp_node_create(bounds, node);

    tree->nodes.occupied += 1;

    *result = node;
    return node_i;
}

int32_t bsp_tree_add_polygon(struct BspTree* tree, size_t polygon_size, const Vec3f polygon_normal, VERTEX_TYPE* polygon_vertices, struct BspPolygon** result) {
    size_t polygon_start = tree->attributes.occupied;

    if( tree->attributes.occupied + polygon_size >= tree->attributes.capacity ) {
        size_t alloc_attributes_result = 0;
        alloc_attributes_result = bsp_tree_alloc_attributes(tree, polygon_size);
        log_assert( alloc_attributes_result >= polygon_size );
    }

    memcpy(&tree->attributes.vertices[polygon_start*VERTEX_SIZE], polygon_vertices, polygon_size*VERTEX_SIZE*sizeof(VERTEX_TYPE));
    tree->attributes.occupied += polygon_size;

    if( tree->polygons.occupied + 1 >= tree->polygons.capacity ) {
        size_t alloc_polygons_result = 0;
        alloc_polygons_result = bsp_tree_alloc_polygons(tree, 1);
        log_assert( alloc_polygons_result >= 1 );
    }
    log_assert( tree->polygons.capacity > tree->polygons.occupied + 1 );

    size_t polygon_i = tree->polygons.occupied;
    struct BspPolygon* polygon = &tree->polygons.array[polygon_i];
    bsp_polygon_create(polygon);

    polygon->start = polygon_start; //polygon_i*polygon_size*VERTEX_SIZE;
    polygon->size = polygon_size;

    if( polygon_normal == NULL ) {
        polygon_compute_normal(polygon_size, VERTEX_SIZE, &tree->attributes.vertices[polygon_start*VERTEX_SIZE], polygon->normal);
    } else {
        vec_copy3f(polygon_normal, polygon->normal);
    }

    tree->polygons.occupied += 1;

    *result = polygon;
    return polygon_i;
}

void bsp_tree_create_from_solid(struct Solid* solid, struct BspTree* tree) {
    size_t alloc_attributes_result = bsp_tree_alloc_attributes(tree, solid->attributes_size);
    log_assert( alloc_attributes_result >= solid->attributes_size );

    size_t num_polygons = solid->indices_size/3;
    size_t alloc_polygons_result = bsp_tree_alloc_polygons(tree, num_polygons);
    log_assert( alloc_polygons_result >= num_polygons );

    size_t alloc_nodes_result = bsp_tree_alloc_nodes(tree, num_polygons);
    log_assert( alloc_nodes_result >= num_polygons );

    struct BspBuildArrays arrays;
    bsp_build_arrays_create(&arrays);
    size_t alloc_arrays_result = bsp_build_arrays_alloc(&arrays, 2 * alloc_polygons_result, alloc_polygons_result);
    log_assert( alloc_arrays_result > alloc_polygons_result );

    Vec3f min = {0};
    Vec3f max = {0};
    for( size_t indices_i = 0; indices_i < solid->indices_size; indices_i += 3 ) {

        VERTEX_TYPE tree_polygon_vertices[3*VERTEX_SIZE];
        for( size_t vertex_i = 0; vertex_i < 3; vertex_i++ ) {
            uint32_t src_i = solid->indices[indices_i+vertex_i];
            const VecP* src = &solid->vertices[src_i*VERTEX_SIZE];

            VecP* dst = &tree_polygon_vertices[vertex_i*VERTEX_SIZE];
            vec_copy3f(src, dst);

            vec_minmax(src, min, max);
        }

        size_t tree_polygon_size = 3;
        struct BspPolygon* tree_polygon = NULL;
        size_t poly_i = bsp_tree_add_polygon(tree, tree_polygon_size, NULL, tree_polygon_vertices, &tree_polygon);

        arrays.front.polygons[poly_i] = poly_i;
        arrays.front.occupied += 1;
    }

    struct BspNodeBounds bounds = {};
    bsp_node_bounds_create(min, max, num_polygons, &bounds);

    struct BspNode* root = NULL;
    bsp_tree_add_node(tree, bounds, &root);

    bsp_build_recur(tree, root, &arrays, 0, num_polygons, &arrays.front);

    log_fail(__FILE__, __LINE__, "BUILT BSP TREE... OR NOT?\n");
}

void bsp_build_arrays_create(struct BspBuildArrays* arrays) {
    arrays->front.polygons = NULL;
    arrays->front.capacity = 0;
    arrays->front.occupied = 0;

    arrays->back.polygons = NULL;
    arrays->back.capacity = 0;
    arrays->back.occupied = 0;
}

size_t bsp_build_arrays_alloc(struct BspBuildArrays* arrays, size_t front_n, size_t back_n) {
    size_t result = 0;

    if( front_n > 0 ) {
        size_t front_alloc = BSP_BUILD_ARRAYS_ALLOC;
        while( front_alloc < front_n ) {
            front_alloc += BSP_BUILD_ARRAYS_ALLOC;
        }

        size_t new_front_capacity = arrays->front.capacity + front_alloc;

        int32_t* new_front_array = realloc(arrays->front.polygons, new_front_capacity * sizeof(int32_t));
        if( new_front_array != NULL ) {
            arrays->front.polygons = new_front_array;
            arrays->front.capacity = new_front_capacity;
            result += front_alloc;
        }
    }

    if( back_n > 0 ) {
        size_t back_alloc = BSP_BUILD_ARRAYS_ALLOC;
        while( back_alloc < back_n ) {
            back_alloc += BSP_BUILD_ARRAYS_ALLOC;
        }

        size_t new_back_capacity = arrays->back.capacity + back_alloc;

        int32_t* new_back_array = realloc(arrays->back.polygons, new_back_capacity * sizeof(int32_t));
        if( new_back_array != NULL ) {
            arrays->back.polygons = new_back_array;
            arrays->back.capacity = new_back_capacity;
            result += back_alloc;
        }
    }

    return result;
}

void bsp_build_select_balanced_divider(const struct BspTree* tree, struct BspNodeBounds bounds, size_t num_polygons, const int32_t* polygon_indices, size_t max_steps, int32_t* selected_divider) {
    *selected_divider = 0;

    if( num_polygons <= 1 ) {
        return;
    }

    float node_width = bounds.half_width*2.0f;
    float node_height = bounds.half_height*2.0f;
    float node_depth = bounds.half_depth*2.0f;

    Vec3f normal_comparison_axis = {0};
    if( node_width <= node_height && node_width <= node_depth ) {
        vec_copy3f((Vec4f)X_AXIS, normal_comparison_axis);
    } else if( node_height <= node_width && node_height <= node_depth ) {
        vec_copy3f((Vec4f)Y_AXIS, normal_comparison_axis);
    } else if( node_depth <= node_width && node_depth <= node_height ) {
        vec_copy3f((Vec4f)Z_AXIS, normal_comparison_axis);
    }

    size_t step_size = 1;
    if( max_steps < num_polygons && max_steps > 0 ) {
        float fstep_size = (float)num_polygons / (float)max_steps;
        log_assert( fstep_size >= 0.0f );
        step_size = (size_t)(fstep_size+1.0f);
    }


    float min_dot = FLT_MAX;
    float min_center_distance = FLT_MAX;
    int32_t best_i = 0;
    float best_score = 0.0f;
    for( size_t polygon_i = 0; polygon_i < num_polygons; polygon_i += step_size ) {
        int32_t index_i = polygon_indices[polygon_i];
        int32_t start_i = tree->polygons.array[index_i].start;
        float current_score = 0.0f;

        float dot = FLT_MAX;
        vec_dot(tree->polygons.array[index_i].normal, normal_comparison_axis, &dot);
        if( fabs(dot) <= min_dot+CUTE_EPSILON ) {
            min_dot = fabs(dot);
            current_score += 1.0f;
        }

        Vec3f average_vertex = {0.0f, 0.0f, 0.0f};
        for( size_t size_i = 0; size_i < tree->polygons.array[index_i].size; size_i++ ) {
            size_t attribute_i = (start_i + size_i) * VERTEX_SIZE;
            vec_add(average_vertex, &tree->attributes.vertices[attribute_i], average_vertex);
        }
        vec_mul1f(average_vertex, 1.0f/(float)tree->polygons.array[index_i].size, average_vertex);

        Vec3f center_vector = {0};
        vec_sub(bounds.center, average_vertex, center_vector);
        float center_distance = FLT_MAX;
        vec_length(center_vector, &center_distance);

        if( fabs(center_distance) <= min_center_distance+CUTE_EPSILON ) {
            min_center_distance = fabs(center_distance);
            current_score += 2.0f;
        }

        if( current_score >= best_score ) {
            best_score = current_score;
            best_i = index_i;
        }

    }

    *selected_divider = best_i;
}

void bsp_build_recur(struct BspTree* tree, struct BspNode* node, struct BspBuildArrays* arrays, size_t loop_start, size_t loop_end, struct BspBuildPartition* partition) {
    size_t num_polygons = loop_end - loop_start;
    log_assert( num_polygons > 0 );

    bsp_build_select_balanced_divider(tree, node->bounds, num_polygons, &partition->polygons[loop_start], num_polygons/10, &node->divider);

    const struct BspPolygon node_divider = tree->polygons.array[node->divider];
    const VERTEX_TYPE* node_divider_vertices = &tree->attributes.vertices[node_divider.start*VERTEX_SIZE];

    draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 0, 0, 255}, 0.01f, node_divider.size, node_divider_vertices, node_divider.normal);
    draw_vec(&global_static_canvas, 0, (Mat)IDENTITY_MAT, (Color){255, 0, 0, 255}, 0.01f, node_divider.normal, &node_divider_vertices[3], 1.0f, 0.1f);
    //draw_plane(&global_static_canvas, MAX_CANVAS_LAYERS-1, (Mat)IDENTITY_MAT, (Color){120, 120, 150, 127}, node_divider.normal, &node_divider_vertices[3], 10.0f);

    size_t front_start = arrays->front.occupied;
    Vec3f front_min = {0};
    Vec3f front_max = {0};

    size_t back_start = arrays->back.occupied;
    Vec3f back_min = {0};
    Vec3f back_max = {0};

    for( size_t loop_i = loop_start; loop_i < loop_end; loop_i++ ) {
        size_t polygon_i = partition->polygons[loop_i];
        if( (int32_t)polygon_i == node->divider ) {
            continue;
        }

        struct BspPolygon current_polygon = tree->polygons.array[polygon_i];
        size_t current_polygon_size = current_polygon.size;
        const VERTEX_TYPE* current_polygon_vertices = &tree->attributes.vertices[current_polygon.start*VERTEX_SIZE];

        size_t result_size = current_polygon_size;
        struct PolygonCutPoint result_points[current_polygon_size];
        enum PolygonCutType result_type = polygon_cut(current_polygon_size, VERTEX_SIZE, current_polygon_vertices,
                                                      node_divider.normal, node_divider_vertices,
                                                      result_size, result_points);

        int32_t front_index = -1;
        int32_t back_index = -1;
        switch(result_type) {
            case POLYGON_COPLANNAR:
            case POLYGON_FRONT:
                front_index = polygon_i;
                break;
            case POLYGON_BACK:
                back_index = polygon_i;
                break;
            case POLYGON_SPANNING:
                if( result_points[0].num_cuts > 0 ) {
                    tree->polygons.array[polygon_i].divider = node->divider;

                    size_t new_poly_size = current_polygon_size+result_points[0].num_cuts;

                    size_t front_occupied = 0;
                    float front_vertices[new_poly_size*VERTEX_SIZE];

                    size_t back_occupied = 0;
                    float back_vertices[new_poly_size*VERTEX_SIZE];

                    for( size_t result_i = 0; result_i < result_size; result_i++ ) {
                        if( result_points[result_i].type == POLYGON_BACK ) {
                            vec_copy3f(&current_polygon_vertices[result_i*VERTEX_SIZE], &back_vertices[back_occupied*VERTEX_SIZE]);
                            back_occupied += 1;
                        } else if( result_points[result_i].type == POLYGON_FRONT ) {
                            vec_copy3f(&current_polygon_vertices[result_i*VERTEX_SIZE], &front_vertices[front_occupied*VERTEX_SIZE]);
                            front_occupied += 1;
                        } else if( result_points[result_i].type == POLYGON_COPLANNAR ) {
                            vec_copy3f(&current_polygon_vertices[result_i*VERTEX_SIZE], &back_vertices[back_occupied*VERTEX_SIZE]);
                            back_occupied += 1;
                            vec_copy3f(&current_polygon_vertices[result_i*VERTEX_SIZE], &front_vertices[front_occupied*VERTEX_SIZE]);
                            front_occupied += 1;
                        }

                        if( result_points[result_i].interpolation_index > -1 ) {
                            const VecP* a = &current_polygon_vertices[result_i*VERTEX_SIZE];
                            const VecP* b = &current_polygon_vertices[result_points[result_i].interpolation_index*VERTEX_SIZE];
                            Vec3f r = {0};
                            vec_lerp(b, a, result_points[result_i].interpolation_value, r);

                            vec_copy3f(r, &back_vertices[back_occupied*VERTEX_SIZE]);
                            back_occupied += 1;
                            vec_copy3f(r, &front_vertices[front_occupied*VERTEX_SIZE]);
                            front_occupied += 1;
                        }
                    }

                    struct BspPolygon* front_polygon = NULL;
                    front_index = bsp_tree_add_polygon(tree, front_occupied, current_polygon.normal, front_vertices, &front_polygon);
                    front_polygon->cut.parent = polygon_i;
                    front_polygon->cut.sibling = back_index;

                    struct BspPolygon* back_polygon = NULL;
                    back_index = bsp_tree_add_polygon(tree, back_occupied, current_polygon.normal, back_vertices, &back_polygon);
                    back_polygon->cut.parent = polygon_i;
                    back_polygon->cut.sibling = front_index;

                    tree->polygons.occupied += 2;

                    draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT,  (Color){255, 255, 0, 255}, 0.01f, front_occupied, front_vertices, current_polygon.normal);
                    draw_polygon_wire(&global_static_canvas, 0, (Mat)IDENTITY_MAT,  (Color){255, 255, 0, 255}, 0.01f, back_occupied, back_vertices, current_polygon.normal);
                }
                break;
        }

        if( front_index > -1 ) {
            if( arrays->front.occupied + 1 >= arrays->front.capacity ) {
                size_t alloc_arrays_result = bsp_build_arrays_alloc(arrays, 1, 0);
                log_assert( alloc_arrays_result > 1 );
            }

            arrays->front.polygons[arrays->front.occupied] = front_index;
            arrays->front.occupied += 1;

            for( size_t polygon_point_i = 0; polygon_point_i < tree->polygons.array[front_index].size; polygon_point_i++ ) {
                size_t vertex_i = tree->polygons.array[front_index].start + polygon_point_i;
                vec_minmax(&tree->attributes.vertices[vertex_i], front_min, front_max);
            }
        }

        if( back_index > -1 ) {
            if( arrays->back.occupied + 1 >= arrays->back.capacity ) {
                size_t alloc_arrays_result = bsp_build_arrays_alloc(arrays, 0, 1);
                log_assert( alloc_arrays_result > 1 );
            }

            arrays->back.polygons[arrays->back.occupied] = back_index;
            arrays->back.occupied += 1;

            for( size_t polygon_point_i = 0; polygon_point_i < tree->polygons.array[back_index].size; polygon_point_i++ ) {
                size_t vertex_i = tree->polygons.array[back_index].start + polygon_point_i;
                vec_minmax(&tree->attributes.vertices[vertex_i], back_min, back_max);
            }
        }
    }

    size_t front_end = arrays->front.occupied;
    size_t back_end = arrays->back.occupied;
    if( front_end - front_start > 1 ) {
        struct BspNodeBounds front_bounds = {};
        bsp_node_bounds_create(front_min, front_max, front_end - front_start, &front_bounds);

        struct BspNode* front_node = NULL;
        bsp_tree_add_node(tree, front_bounds, &front_node);
        bsp_build_recur(tree, front_node, arrays, front_start, front_end, &arrays->front);
    } else if( front_end - front_start == 1 ) {
        printf("front recur end\n");
    }

    if( back_end - back_start > 1 ) {
        struct BspNodeBounds back_bounds = {};
        bsp_node_bounds_create(back_min, back_max, back_end - back_start, &back_bounds);

        struct BspNode* back_node = NULL;
        bsp_tree_add_node(tree, back_bounds, &back_node);
        bsp_build_recur(tree, back_node, arrays, back_start, back_end, &arrays->back);
    } else if( back_end - back_start == 1 ) {
        printf("back recur end\n");
    }

}
