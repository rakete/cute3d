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

void bsp_node_bounds_create(Vec3f min, Vec3f max, struct BspBounds* bounds) {
    bounds->half_width = (max[0] - min[0])/2.0f;
    bounds->half_height = (max[1] - min[1])/2.0f;
    bounds->half_depth = (max[2] - min[2])/2.0f;
    bounds->center[0] = min[0] + bounds->half_width;
    bounds->center[1] = min[1] + bounds->half_height;
    bounds->center[2] = min[2] + bounds->half_depth;
}

void bsp_node_create(struct BspNode* node) {
    node->divider = 0;
    node->num_polygons = 0;

    node->bounds.half_width = 0.0f;
    node->bounds.half_height = 0.0f;
    node->bounds.half_depth = 0.0f;
    node->bounds.center[0] = 0.0f;
    node->bounds.center[1] = 0.0f;
    node->bounds.center[2] = 0.0f;

    node->tree.parent = -1;
    node->tree.front = -1;
    node->tree.back = -1;
    node->tree.index = -1;
    node->tree.depth = 0;

    node->state.empty = false;
    node->state.solid = false;
    node->state.disabled = false;
}

void bsp_tree_create(struct BspTree* tree) {
    tree->attributes.vertices = NULL;
    tree->attributes.normals = NULL;
    tree->attributes.texcoords = NULL;
    tree->attributes.colors = NULL;
    tree->attributes.capacity = 0;
    tree->attributes.occupied = 0;

    tree->polygons.array = NULL;
    tree->polygons.capacity = 0;
    tree->polygons.occupied = 0;

    tree->nodes.array = NULL;
    tree->nodes.capacity = 0;
    tree->nodes.occupied = 0;
}

void bsp_tree_destroy(struct BspTree* tree) {
    if( tree->attributes.capacity > 0 ) {
        free(tree->attributes.vertices);
        free(tree->attributes.normals);
        free(tree->attributes.texcoords);
        free(tree->attributes.colors);
    }

    if( tree->polygons.capacity > 0 ) {
        free(tree->polygons.array);
    }

    if( tree->nodes.capacity > 0 ) {
        free(tree->nodes.array);
    }

    bsp_tree_create(tree);
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

int32_t bsp_tree_add_node(struct BspTree* tree, int32_t parent, struct BspNode** result) {
    if( tree->nodes.occupied + 1 >= tree->nodes.capacity ) {
        size_t alloc_nodes_result = 0;
        alloc_nodes_result = bsp_tree_alloc_nodes(tree, 1);
        log_assert( alloc_nodes_result >= 1 );
    }

    size_t node_i = tree->nodes.occupied;
    struct BspNode* node = &tree->nodes.array[node_i];
    bsp_node_create(node);

    node->tree.index = node_i;
    node->tree.parent = parent;
    if( parent > -1 ) {
        node->tree.depth = tree->nodes.array[parent].tree.depth + 1;
    }

    tree->nodes.occupied += 1;

    *result = node;
    return node_i;
}

int32_t bsp_tree_add_polygon(struct BspTree* tree, size_t polygon_size, const Vec3f polygon_normal, struct ParameterAttributes polygon_attributes, struct BspPolygon** result) {
    size_t polygon_start = tree->attributes.occupied;

    if( tree->attributes.occupied + polygon_size >= tree->attributes.capacity ) {
        size_t alloc_attributes_result = 0;
        alloc_attributes_result = bsp_tree_alloc_attributes(tree, polygon_size);
        log_assert( alloc_attributes_result >= polygon_size );
    }

    log_assert( polygon_attributes.vertices != NULL );
    memcpy(&tree->attributes.vertices[polygon_start*VERTEX_SIZE], polygon_attributes.vertices, polygon_size*VERTEX_SIZE*sizeof(VERTEX_TYPE));

    char* zeros = NULL;
    if( polygon_attributes.normals == NULL || polygon_attributes.texcoords != NULL || polygon_attributes.colors != NULL ) {
        zeros = calloc(polygon_size * 4 * sizeof(float), sizeof(char));
        log_assert( zeros != NULL );
    }

    if( polygon_attributes.normals != NULL ) {
        memcpy(&tree->attributes.normals[polygon_start*NORMAL_SIZE], polygon_attributes.normals, polygon_size*NORMAL_SIZE*sizeof(NORMAL_TYPE));
    } else {
        memcpy(&tree->attributes.normals[polygon_start*NORMAL_SIZE], zeros, polygon_size*NORMAL_SIZE*sizeof(NORMAL_TYPE));
    }

    if( polygon_attributes.texcoords != NULL ) {
        memcpy(&tree->attributes.texcoords[polygon_start*TEXCOORD_SIZE], polygon_attributes.texcoords, polygon_size*TEXCOORD_SIZE*sizeof(TEXCOORD_TYPE));
    } else {
        memcpy(&tree->attributes.texcoords[polygon_start*TEXCOORD_SIZE], zeros, polygon_size*TEXCOORD_SIZE*sizeof(TEXCOORD_TYPE));
    }

    if( polygon_attributes.colors != NULL ) {
        memcpy(&tree->attributes.colors[polygon_start*COLOR_SIZE], polygon_attributes.colors, polygon_size*COLOR_SIZE*sizeof(COLOR_TYPE));
    } else {
        memcpy(&tree->attributes.colors[polygon_start*COLOR_SIZE], zeros, polygon_size*COLOR_SIZE*sizeof(COLOR_TYPE));
    }

    if( zeros != NULL ) {
        free(zeros);
    }

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

struct BspNode* bsp_tree_create_from_solid(struct Solid* solid, struct BspTree* tree) {
    bsp_tree_create(tree);

    size_t alloc_attributes_result = bsp_tree_alloc_attributes(tree, solid->attributes_size);
    log_assert( alloc_attributes_result >= solid->attributes_size );

    size_t num_polygons = solid->indices_size/3;
    size_t alloc_polygons_result = bsp_tree_alloc_polygons(tree, num_polygons);
    log_assert( alloc_polygons_result >= num_polygons );

    size_t alloc_nodes_result = bsp_tree_alloc_nodes(tree, num_polygons);
    log_assert( alloc_nodes_result >= num_polygons );

    struct BspBuildState state;
    bsp_build_state_create(&state);

    size_t alloc_front_result = bsp_build_partition_alloc(&state.front, 2 * alloc_polygons_result);
    log_assert( alloc_front_result >= 2 * alloc_polygons_result );

    size_t alloc_back_result = bsp_build_partition_alloc(&state.back, alloc_polygons_result);
    log_assert( alloc_back_result >= alloc_polygons_result );

    Vec3f min = {0};
    Vec3f max = {0};
    for( size_t indices_i = 0; indices_i < solid->indices_size; indices_i += 3 ) {

        VERTEX_TYPE tree_polygon_vertices[3*VERTEX_SIZE];
        NORMAL_TYPE tree_polygon_normals[3*NORMAL_SIZE];
        TEXCOORD_TYPE tree_polygon_texcoords[3*TEXCOORD_SIZE];
        COLOR_TYPE tree_polygon_colors[3*COLOR_SIZE];
        for( size_t vertex_i = 0; vertex_i < 3; vertex_i++ ) {
            uint32_t src_i = solid->indices[indices_i+vertex_i];
            const VertexP* src_vertex = &solid->vertices[src_i*VERTEX_SIZE];
            const NormalP* src_normal = &solid->normals[src_i*NORMAL_SIZE];
            const TexcoordP* src_texcoord = &solid->texcoords[src_i*TEXCOORD_SIZE];
            const ColorP* src_color = &solid->colors[src_i*COLOR_SIZE];

            VertexP* dst_vertex = &tree_polygon_vertices[vertex_i*VERTEX_SIZE];
            vertex_copy(src_vertex, dst_vertex);

            NormalP* dst_normal = &tree_polygon_normals[vertex_i*NORMAL_SIZE];
            normal_copy(src_normal, dst_normal);

            TexcoordP* dst_texcoord = &tree_polygon_texcoords[vertex_i*TEXCOORD_SIZE];
            texcoord_copy(src_texcoord, dst_texcoord);

            ColorP* dst_color = &tree_polygon_colors[vertex_i*COLOR_SIZE];
            color_copy(src_color, dst_color);

            vec_minmax(src_vertex, min, max);
        }

        size_t tree_polygon_size = 3;
        struct BspPolygon* tree_polygon = NULL;
        struct ParameterAttributes parameter_attributes = {
            .vertices = tree_polygon_vertices,
            .normals = tree_polygon_normals,
            .texcoords = tree_polygon_texcoords,
            .colors = tree_polygon_colors
        };
        size_t poly_i = bsp_tree_add_polygon(tree, tree_polygon_size, NULL, parameter_attributes, &tree_polygon);

        state.front.polygons[poly_i] = poly_i;
        state.front.occupied += 1;
    }

    struct BspBuildStackFrame root_frame;
    root_frame.tree_side = BSP_FRONT;
    root_frame.parent_index = -1;
    root_frame.partition_start = 0;
    root_frame.partition_end = num_polygons;
    vec_copy3f(min, root_frame.bounds_min);
    vec_copy3f(max, root_frame.bounds_max);

    struct BspNode* root = bsp_build(tree, root_frame, &state);

    bsp_build_state_destroy(&state);

    return root;
}

size_t bsp_build_stack_pop(struct BspBuildStack* stack, struct BspBuildStackFrame* frame) {
    if( stack->occupied > 0 ) {
        stack->occupied -= 1;
        memcpy(frame, &stack->frames[stack->occupied], sizeof(struct BspBuildStackFrame));
    }

    return stack->occupied;
}

size_t bsp_build_stack_push(struct BspBuildStack* stack, struct BspBuildStackFrame frame) {
    if( stack->occupied + 1 >= stack->capacity ) {
        size_t alloc_result = bsp_build_stack_alloc(stack, 1);
        log_assert( alloc_result >= 1 );
    }

    if( stack->occupied + 1 < stack->capacity ) {
        stack->frames[stack->occupied] = frame;
        stack->occupied += 1;
    }

    return stack->occupied;
}


size_t bsp_build_stack_alloc(struct BspBuildStack* stack, size_t n) {
    size_t result = 0;

    if( n > 0 ) {
        size_t alloc = BSP_BUILD_STACK_ALLOC;
        while( alloc < n ) {
            alloc += BSP_BUILD_STACK_ALLOC;
        }

        size_t new_capacity = stack->capacity + alloc;

        struct BspBuildStackFrame* new_frames = realloc(stack->frames, new_capacity * sizeof(struct BspBuildStackFrame));
        if( new_frames != NULL ) {
            stack->frames = new_frames;
            stack->capacity = new_capacity;
            result += alloc;
        }
    }

    return result;
}

size_t bsp_build_partition_alloc(struct BspBuildPartition* partition, size_t n) {
    size_t result = 0;

    if( n > 0 ) {
        size_t alloc = BSP_BUILD_ARRAYS_ALLOC;
        while( alloc < n ) {
            alloc += BSP_BUILD_ARRAYS_ALLOC;
        }

        size_t new_capacity = partition->capacity + alloc;

        int32_t* new_array = realloc(partition->polygons, new_capacity * sizeof(int32_t));
        if( new_array != NULL ) {
            partition->polygons = new_array;
            partition->capacity = new_capacity;
            result += alloc;
        }
    }

    return result;
}

void bsp_build_state_create(struct BspBuildState* state) {
    state->front.polygons = NULL;
    state->front.capacity = 0;
    state->front.occupied = 0;

    state->back.polygons = NULL;
    state->back.capacity = 0;
    state->back.occupied = 0;

    state->stack.frames = NULL;
    state->stack.capacity = 0;
    state->stack.occupied = 0;
}

void bsp_build_state_destroy(struct BspBuildState* state) {
    if( state->front.capacity > 0 ) {
        free(state->front.polygons);
    }

    if( state->back.capacity > 0 ) {
        free(state->back.polygons);
    }

    if( state->stack.capacity > 0 ) {
        free(state->stack.frames);
    }

    bsp_build_state_create(state);
}

int32_t bsp_build_select_balanced_divider(const struct BspTree* tree, struct BspBounds bounds, size_t loop_start, size_t loop_end, const int32_t* polygon_indices, size_t max_steps) {
    size_t num_polygons = loop_end - loop_start;
    log_assert( num_polygons > 0 );
    if( num_polygons <= 2 ) {
        return polygon_indices[loop_start];
    }

    float node_width = bounds.half_width*2.0f;
    float node_height = bounds.half_height*2.0f;
    float node_depth = bounds.half_depth*2.0f;

    Vec3f normal_comparison_axis = {0};
    if( node_width <= node_height && node_width <= node_depth ) {
        vec_copy3f((Vec4f)X_AXIS, normal_comparison_axis);
    } else if( node_height <= node_width && node_height <= node_depth ) {
        vec_copy3f((Vec4f)Y_AXIS, normal_comparison_axis);
    } else {
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
    for( size_t polygon_i = loop_start; polygon_i < loop_end; polygon_i += step_size ) {
        int32_t index_i = polygon_indices[polygon_i];
        int32_t start_i = tree->polygons.array[index_i].start;
        float current_score = 0.0f;

        float dot = FLT_MAX;
        vec_dot(tree->polygons.array[index_i].normal, normal_comparison_axis, &dot);
        if( fabs(dot) <= min_dot+10.0f*CUTE_EPSILON ) {
            min_dot = fabs(dot);
            current_score += 1.0f + polygon_i*CUTE_EPSILON;
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

        if( fabs(center_distance) <= min_center_distance+1.0f*CUTE_EPSILON ) {
            min_center_distance = fabs(center_distance);
            current_score += 1.0f + polygon_i*CUTE_EPSILON;
        }

        if( current_score >= best_score ) {
            if( polygon_i > loop_start ) {
                best_score = current_score;
            }
            best_i = index_i;
        }

    }

    return best_i;
}

struct BspNode* bsp_build(struct BspTree* tree, struct BspBuildState* state) {
    if( state->stack.occupied == 0 ) {
        return NULL;
    }

    struct BspNode* root = &tree->nodes.array[tree->nodes.occupied];

    while( state->stack.occupied > 0 ) {
        struct BspBuildStackFrame parent_frame;
        bsp_build_stack_pop(&state->stack, &parent_frame);

        size_t loop_start = parent_frame.partition_start;
        size_t loop_end = parent_frame.partition_end;
        int32_t grandparent_i = parent_frame.parent_index;
        enum BspSide parent_side = parent_frame.tree_side;

        Vec3f parent_min = {0};
        vec_copy3f(parent_frame.bounds_min, parent_min);

        Vec3f parent_max = {0};
        vec_copy3f(parent_frame.bounds_max, parent_max);

        struct BspBounds bounds = {};
        bsp_node_bounds_create(parent_min, parent_max, &bounds);

        struct BspNode* node = NULL;
        int32_t parent_i = bsp_tree_add_node(tree, grandparent_i, bounds, &node);
        if( grandparent_i > -1 && parent_side == BSP_FRONT ) {
            tree->nodes.array[grandparent_i].tree.front = parent_i;
        } else if( grandparent_i > -1 && parent_side == BSP_BACK ) {
            tree->nodes.array[grandparent_i].tree.back = parent_i;
        }

        struct BspBuildPartition* partition = &state->front;
        if( parent_side == BSP_BACK ) {
            partition = &state->back;
        }

        size_t num_polygons = loop_end - loop_start;
        log_assert( num_polygons > 0 );

        int32_t node_divider_i = bsp_build_select_balanced_divider(tree, tree->nodes.array[parent_i].bounds, loop_start, loop_end, partition->polygons, num_polygons/10);
        tree->nodes.array[parent_i].divider = node_divider_i;

        const struct BspPolygon node_divider = tree->polygons.array[node_divider_i];
        Vertex node_divider_vertex = {0};
        vertex_copy(&tree->attributes.vertices[node_divider.start*VERTEX_SIZE], node_divider_vertex);

        size_t front_start = state->front.occupied;
        Vec3f front_min = {0};
        Vec3f front_max = {0};

        size_t back_start = state->back.occupied;
        Vec3f back_min = {0};
        Vec3f back_max = {0};

        for( size_t loop_i = loop_start; loop_i < loop_end; loop_i++ ) {
            size_t polygon_i = partition->polygons[loop_i];
            if( (int32_t)polygon_i == node_divider_i ) {
                continue;
            }

            const struct BspPolygon current_polygon = tree->polygons.array[polygon_i];
            size_t current_polygon_size = current_polygon.size;
            const VERTEX_TYPE* current_polygon_vertices = &tree->attributes.vertices[current_polygon.start*VERTEX_SIZE];
            const NORMAL_TYPE* current_polygon_normals = &tree->attributes.normals[current_polygon.start*NORMAL_SIZE];
            const TEXCOORD_TYPE* current_polygon_texcoords = &tree->attributes.texcoords[current_polygon.start*TEXCOORD_SIZE];
            const COLOR_TYPE* current_polygon_colors = &tree->attributes.colors[current_polygon.start*COLOR_SIZE];

            size_t result_size = current_polygon_size;
            struct PolygonCutPoint result_points[current_polygon_size];
            enum PolygonCutType result_type = polygon_cut(current_polygon_size, VERTEX_SIZE, current_polygon_vertices,
                                                          node_divider.normal, node_divider_vertex,
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
                        tree->polygons.array[polygon_i].divider = node_divider_i;

                        size_t new_poly_size = current_polygon_size+result_points[0].num_cuts;

                        size_t front_occupied = 0;
                        VERTEX_TYPE front_vertices[new_poly_size*VERTEX_SIZE];
                        NORMAL_TYPE front_normals[new_poly_size*NORMAL_SIZE];
                        TEXCOORD_TYPE front_texcoords[new_poly_size*TEXCOORD_SIZE];
                        COLOR_TYPE front_colors[new_poly_size*COLOR_SIZE];

                        size_t back_occupied = 0;
                        VERTEX_TYPE back_vertices[new_poly_size*VERTEX_SIZE];
                        NORMAL_TYPE back_normals[new_poly_size*NORMAL_SIZE];
                        TEXCOORD_TYPE back_texcoords[new_poly_size*TEXCOORD_SIZE];
                        COLOR_TYPE back_colors[new_poly_size*COLOR_SIZE];

                        for( size_t result_i = 0; result_i < result_size; result_i++ ) {
                            if( result_points[result_i].type == POLYGON_BACK || result_points[result_i].type == POLYGON_COPLANNAR ) {
                                vertex_copy(&current_polygon_vertices[result_i*VERTEX_SIZE], &back_vertices[back_occupied*VERTEX_SIZE]);
                                normal_copy(&current_polygon_normals[result_i*NORMAL_SIZE], &back_normals[back_occupied*NORMAL_SIZE]);
                                texcoord_copy(&current_polygon_texcoords[result_i*TEXCOORD_SIZE], &back_texcoords[back_occupied*TEXCOORD_SIZE]);
                                color_copy(&current_polygon_colors[result_i*COLOR_SIZE], &back_colors[back_occupied*COLOR_SIZE]);
                                back_occupied += 1;
                            }

                            if( result_points[result_i].type == POLYGON_FRONT || result_points[result_i].type == POLYGON_COPLANNAR ) {
                                vertex_copy(&current_polygon_vertices[result_i*VERTEX_SIZE], &front_vertices[front_occupied*VERTEX_SIZE]);
                                normal_copy(&current_polygon_normals[result_i*NORMAL_SIZE], &front_normals[front_occupied*NORMAL_SIZE]);
                                texcoord_copy(&current_polygon_texcoords[result_i*TEXCOORD_SIZE], &front_texcoords[front_occupied*TEXCOORD_SIZE]);
                                color_copy(&current_polygon_colors[result_i*COLOR_SIZE], &front_colors[front_occupied*COLOR_SIZE]);
                                front_occupied += 1;
                            }

                            if( result_points[result_i].interpolation_index > -1 ) {
                                const VertexP* vertex_a = &current_polygon_vertices[result_i*VERTEX_SIZE];
                                const VertexP* vertex_b = &current_polygon_vertices[result_points[result_i].interpolation_index*VERTEX_SIZE];
                                Vertex vertex_r = {0};
                                vertex_lerp(vertex_b, vertex_a, result_points[result_i].interpolation_value, vertex_r);

                                const NormalP* normal_a = &current_polygon_normals[result_i*NORMAL_SIZE];
                                const NormalP* normal_b = &current_polygon_normals[result_points[result_i].interpolation_index*NORMAL_SIZE];
                                Normal normal_r = {0};
                                normal_lerp(normal_b, normal_a, result_points[result_i].interpolation_value, normal_r);

                                const TexcoordP* texcoord_a = &current_polygon_texcoords[result_i*TEXCOORD_SIZE];
                                const TexcoordP* texcoord_b = &current_polygon_texcoords[result_points[result_i].interpolation_index*TEXCOORD_SIZE];
                                Texcoord texcoord_r = {0};
                                texcoord_lerp(texcoord_b, texcoord_a, result_points[result_i].interpolation_value, texcoord_r);

                                const ColorP* color_a = &current_polygon_colors[result_i*COLOR_SIZE];
                                const ColorP* color_b = &current_polygon_colors[result_points[result_i].interpolation_index*COLOR_SIZE];
                                Color color_r = {0};
                                color_lerp(color_b, color_a, result_points[result_i].interpolation_value, color_r);

                                vertex_copy(vertex_r, &back_vertices[back_occupied*VERTEX_SIZE]);
                                normal_copy(normal_r, &back_normals[back_occupied*NORMAL_SIZE]);
                                texcoord_copy(texcoord_r, &back_texcoords[back_occupied*TEXCOORD_SIZE]);
                                color_copy(color_r, &back_colors[back_occupied*COLOR_SIZE]);
                                back_occupied += 1;

                                vertex_copy(vertex_r, &front_vertices[front_occupied*VERTEX_SIZE]);
                                normal_copy(normal_r, &front_normals[front_occupied*NORMAL_SIZE]);
                                texcoord_copy(texcoord_r, &front_texcoords[front_occupied*TEXCOORD_SIZE]);
                                color_copy(color_r, &front_colors[front_occupied*COLOR_SIZE]);
                                front_occupied += 1;
                            }
                        }

                        struct BspPolygon* front_polygon = NULL;
                        struct ParameterAttributes parameter_front_attributes = {
                            .vertices = front_vertices,
                            .normals = front_normals,
                            .texcoords = front_texcoords,
                            .colors = front_colors
                        };
                        front_index = bsp_tree_add_polygon(tree, front_occupied, current_polygon.normal, parameter_front_attributes, &front_polygon);
                        front_polygon->cut.parent = polygon_i;
                        front_polygon->cut.sibling = back_index;

                        struct BspPolygon* back_polygon = NULL;
                        struct ParameterAttributes parameter_back_attributes = {
                            .vertices = back_vertices,
                            .normals = back_normals,
                            .texcoords = back_texcoords,
                            .colors = back_colors
                        };
                        back_index = bsp_tree_add_polygon(tree, back_occupied, current_polygon.normal, parameter_back_attributes, &back_polygon);
                        back_polygon->cut.parent = polygon_i;
                        back_polygon->cut.sibling = front_index;

                        tree->polygons.occupied += 2;
                    }
                    break;
            }

            if( front_index > -1 ) {
                if( state->front.occupied + 1 >= state->front.capacity ) {
                    size_t alloc_state_result = bsp_build_partition_alloc(&state->front, 1);
                    log_assert( alloc_state_result > 1 );
                }

                state->front.polygons[state->front.occupied] = front_index;
                state->front.occupied += 1;

                for( size_t polygon_point_i = 0; polygon_point_i < tree->polygons.array[front_index].size; polygon_point_i++ ) {
                    size_t vertex_i = tree->polygons.array[front_index].start + polygon_point_i;
                    vec_minmax(&tree->attributes.vertices[vertex_i], front_min, front_max);
                }
            }

            if( back_index > -1 ) {
                if( state->back.occupied + 1 >= state->back.capacity ) {
                    size_t alloc_state_result = bsp_build_partition_alloc(&state->back, 1);
                    log_assert( alloc_state_result > 1 );
                }

                state->back.polygons[state->back.occupied] = back_index;
                state->back.occupied += 1;

                for( size_t polygon_point_i = 0; polygon_point_i < tree->polygons.array[back_index].size; polygon_point_i++ ) {
                    size_t vertex_i = tree->polygons.array[back_index].start + polygon_point_i;
                    vec_minmax(&tree->attributes.vertices[vertex_i], back_min, back_max);
                }
            }
        }

        size_t front_end = state->front.occupied;
        size_t back_end = state->back.occupied;

        if( back_end - back_start > 0 ) {
            struct BspBuildStackFrame back_frame;
            back_frame.tree_side = BSP_BACK;
            back_frame.parent_index = parent_i;
            back_frame.partition_start = back_start;
            back_frame.partition_end = back_end;
            vec_copy3f(back_min, back_frame.bounds_min);
            vec_copy3f(back_max, back_frame.bounds_max);
            bsp_build_stack_push(&state->stack, back_frame);
        } else if( back_end - back_start == 0 ) {
            struct BspNode* solid_node = NULL;
            int32_t solid_i = bsp_tree_add_node(tree, node_i, &solid_node);
            solid_node->state.solid = true;

            tree->nodes.array[node_i].tree.back = solid_i;
        }

        if( front_end - front_start > 0 ) {
            struct BspBuildStackFrame front_frame;
            front_frame.tree_side = BSP_FRONT;
            front_frame.parent_index = parent_i;
            front_frame.partition_start = front_start;
            front_frame.partition_end = front_end;
            vec_copy3f(front_min, front_frame.bounds_min);
            vec_copy3f(front_max, front_frame.bounds_max);
            bsp_build_stack_push(&state->stack, front_frame);
        } else if( front_end - front_start == 0 ) {
            struct BspNode* empty_node = NULL;
            int32_t empty_i = bsp_tree_add_node(tree, node_i, &empty_node);
            empty_node->state.empty = true;

            tree->nodes.array[node_i].tree.front = empty_i;
        }

    }

    return root;
}
