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

    polygon->cut.divider = -1;
    polygon->cut.parent = -1;
    polygon->cut.sibling = -1;
}

void bsp_node_bounds_create(Vec3f min, Vec3f max, struct BspBounds* bounds) {
    bounds->half_size[0] = (max[0] - min[0])/2.0f;
    bounds->half_size[1] = (max[1] - min[1])/2.0f;
    bounds->half_size[2] = (max[2] - min[2])/2.0f;
    bounds->center[0] = min[0] + bounds->half_size[0];
    bounds->center[1] = min[1] + bounds->half_size[1];
    bounds->center[2] = min[2] + bounds->half_size[2];
}

void bsp_node_create(struct BspNode* node) {
    node->divider = 0;
    node->num_polygons = 0;

    node->bounds.half_size[0] = 0.0f;
    node->bounds.half_size[1] = 0.0f;
    node->bounds.half_size[2] = 0.0f;
    node->bounds.center[0] = 0.0f;
    node->bounds.center[1] = 0.0f;
    node->bounds.center[2] = 0.0f;

    node->tree.side = BSP_NOSIDE;
    node->tree.parent = -1;
    node->tree.front = -1;
    node->tree.back = -1;
    node->tree.index = -1;
    node->tree.depth = 0;
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

int32_t bsp_tree_add_node(struct BspTree* tree, int32_t parent, enum BspSide side, size_t num_polygons, struct BspBounds bounds, int32_t divider, struct BspNode** result) {
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

    node->tree.side = side;
    if( side == BSP_FRONT ) {
        tree->nodes.array[parent].tree.front = node_i;
    } else if( side == BSP_BACK ) {
        tree->nodes.array[parent].tree.back = node_i;
    }

    node->tree.depth = tree->nodes.array[parent].tree.depth + 1;

    node->num_polygons = num_polygons;
    node->bounds = bounds;
    node->divider = divider;

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

    // - all attributes beside the vertices are optional, so this array of zeros is copied into the bsp tree instead
    // when a attribute pointer is NULL
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

    polygon->start = polygon_start;
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

int32_t bsp_tree_test_polygon(const struct BspTree* tree, int32_t start_i,
                              size_t polygon_size, const float* polygon_vertices,
                              struct BspNode* result_node, enum PolygonCutType* result_cut_type, size_t result_size, struct PolygonCutPoint* result_points)
{
    log_assert(start_i >= 0);
    int32_t test_i = start_i;

    while( true ) {
        const struct BspNode* test_node = &tree->nodes.array[test_i];

        const struct BspPolygon test_divider = tree->polygons.array[test_node->divider];
        Vertex test_divider_vertex = {0};
        vertex_copy(&tree->attributes.vertices[test_divider.start*VERTEX_SIZE], test_divider_vertex);

        *result_cut_type = polygon_cut_test(polygon_size, VERTEX_SIZE, polygon_vertices,
                                            test_divider.normal, test_divider_vertex,
                                            result_size, result_points);

        switch(*result_cut_type) {
            case POLYGON_COPLANNAR:
            case POLYGON_FRONT:
                if( test_node->tree.front > -1 ) {
                    test_i = test_node->tree.front;
                } else {
                    result_node->tree.parent = test_i;
                    result_node->tree.side = BSP_FRONT;
                    return -1;
                }
                test_i = test_node->tree.front;
                break;
            case POLYGON_BACK:
                if( test_node->tree.back > -1 ) {
                    test_i = test_node->tree.back;
                } else {
                    result_node->tree.parent = test_i;
                    result_node->tree.side = BSP_BACK;
                    return -1;
                }
                break;
            case POLYGON_SPANNING:
                return test_i;
                break;
        }
    }

    return -1;
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

    Vec3f min = {FLT_MAX, FLT_MAX, FLT_MAX};
    Vec3f max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
    for( size_t indices_i = 0; indices_i < solid->indices_size; indices_i += 3 ) {

        // - the for loop goes through the indices of the solid, gets the attributes according
        // to the index from the solid and copies them into the tree->attributes, while doing
        // this it also collects the maximum and minimum xyz values
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

        // - it also adds polygons to the tree, it assumes every polygon is a triangle, which
        // should also be true when handling solids
        size_t tree_polygon_size = 3;
        struct BspPolygon* tree_polygon = NULL;
        struct ParameterAttributes parameter_attributes = {
            .vertices = tree_polygon_vertices,
            .normals = tree_polygon_normals,
            .texcoords = tree_polygon_texcoords,
            .colors = tree_polygon_colors
        };
        int32_t poly_i = bsp_tree_add_polygon(tree, tree_polygon_size, NULL, parameter_attributes, &tree_polygon);

        // - the state.front partition gets filled with the indices of the polygons in the tree,
        // this is later used by bsp_build when sorting the polygons into front and back
        state.front.polygons[poly_i] = poly_i;
        state.front.occupied += 1;
    }

    // - an initial stack frame is created for bsp_build to pop when it starts, it contains
    // the range of all added polygons, and the collected min max vec3fs, and contains with which
    // side of the paritions to start (BSP_FRONT in this case, because we filled that above)
    struct BspBuildStackFrame root_frame;
    root_frame.tree_side = BSP_FRONT;
    root_frame.parent_index = -1;
    root_frame.partition_start = 0;
    root_frame.partition_end = num_polygons;
    vec_copy3f(min, root_frame.bounds_min);
    vec_copy3f(max, root_frame.bounds_max);

    struct BspNode* root = bsp_build(tree, root_frame, false, &state);

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

    // - I want to divide through its smallest dimension because I assumed that way
    // I would get less cuts that I need to compute, never actually verified that
    // assumption
    // - so I look at the bounding box dimension, and copy the axis along which the
    // mesh has the smallest size into normal_comparison_axis, so that later I can
    // compare polygon normals to it and use the dot product to determine if a normal
    // is more perpendicular to that comparison axis, which means the polygon is
    // aligned in such a way as to better cut the mesh through its minimal dimension
    float node_width = bounds.half_size[0]*2.0f;
    float node_height = bounds.half_size[1]*2.0f;
    float node_depth = bounds.half_size[2]*2.0f;

    Vec3f normal_comparison_axis = {0};
    if( node_width <= node_height && node_width <= node_depth ) {
        vec_copy3f((Vec4f)X_AXIS, normal_comparison_axis);
    } else if( node_height <= node_width && node_height <= node_depth ) {
        vec_copy3f((Vec4f)Y_AXIS, normal_comparison_axis);
    } else {
        vec_copy3f((Vec4f)Z_AXIS, normal_comparison_axis);
    }

    // - I found this trick somewhere but I forgot where exactly, its just some way
    // to round correctly so that I get a max_steps number of steps through all
    // polygons when I step by step_size
    size_t step_size = 1;
    if( max_steps < num_polygons && max_steps > 0 ) {
        float fstep_size = (float)num_polygons / (float)max_steps;
        log_assert( fstep_size >= 0.0f );
        step_size = (size_t)(fstep_size+1.0f);
    }

    // - go through polygons while increasing by step_size so that we only test max_steps
    // polygons
    // - the test consist of comparing the polygon normal with the minimum dimension axis
    // from above, and computing the center-to-center distance
    // - the polygon is then scored according to these tests, meaning that if the polygon
    // has a normal that is 'more' orthognal compared to the minimum axis of all previously
    // tested polygons, and that is nearer to the center of the mesh then all previously tested
    // polygons, then it is scored higher and potentially selected as new divider candidate
    // to return
    float min_dot = FLT_MAX;
    float min_center_distance = FLT_MAX;
    int32_t best_i = 0;
    float best_score = 0.0f;
    for( size_t polygon_i = loop_start; polygon_i < loop_end; polygon_i += step_size ) {
        int32_t index_i = polygon_indices[polygon_i];
        int32_t start_i = tree->polygons.array[index_i].start;
        log_assert(start_i >= 0);
        float current_score = 0.0f;

        float dot = FLT_MAX;
        vec_dot(tree->polygons.array[index_i].normal, normal_comparison_axis, &dot);
        if( fabs(dot) <= min_dot+10.0f*CUTE_EPSILON ) {
            min_dot = fabs(dot);
            // - scoring and then selecting based on the score is less then ideal, the first
            // polygon tested will always be the best polygon since I initialize min_dot
            // and min_center_distance to FLT_MAX, so to prevent always selecting the first
            // polygon I use the if( polygon_i > loop_start ) condition below when setting
            // the best_score, and I also put in these terms where I increase the score
            // by a small amount when for later polygons, so that this method favours
            // later polygons that match the test over earlier polygons
            // - this is not tested well but seems to work good enough, may be worth
            // looking into again at a later point in time
            current_score += 1.0f + polygon_i*CUTE_EPSILON;
        }

        Vec3f average_vertex = {0.0f, 0.0f, 0.0f};
        for( size_t size_i = 0; size_i < tree->polygons.array[index_i].size; size_i++ ) {
            size_t attribute_i = ((size_t)start_i + size_i) * VERTEX_SIZE;
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

struct BspNode* bsp_build(struct BspTree* tree, struct BspBuildStackFrame root_frame, bool triangulate, struct BspBuildState* state) {
    bsp_build_stack_push(&state->stack, root_frame);

    if( state->stack.occupied == 0 ) {
        return NULL;
    }

    struct BspNode* root = &tree->nodes.array[tree->nodes.occupied];

    // - this while loop runs as long as there are frames left on the stack, first thing it does
    // is pop the last pushed frame and then processed the associated branch, so at the end of this while
    // I add frames for every subbranch that I want to process next, if there is nothing to process left
    // I do not add anything to the stack and eventually this while loop ends
    // - a short summary of what happens inside this while: after popping the last stack frame and using
    // its values to initialze various local variables, I first select a polygon as divider out of all
    // polygons that belong to the current branch, then I go through all other polygons and test them
    // against the divider polygon and decide if they are in front, behind or are being cut by the
    // divider, I handle them accordingly by appending their index to the front or back partition or by
    // cutting them into to new polygons and appending those to the front and back partitions, this
    // is repeated until I have added a node for every polygon into the tree
    while( state->stack.occupied > 0 ) {
        // - first thing to be done, pop the stack frame
        struct BspBuildStackFrame parent_frame;
        bsp_build_stack_pop(&state->stack, &parent_frame);

        size_t loop_start = parent_frame.partition_start;
        size_t loop_end = parent_frame.partition_end;
        int32_t parent_i = parent_frame.parent_index;
        enum BspSide parent_side = parent_frame.tree_side;

        Vec3f parent_min = {0};
        vec_copy3f(parent_frame.bounds_min, parent_min);

        Vec3f parent_max = {0};
        vec_copy3f(parent_frame.bounds_max, parent_max);

        struct BspBounds bounds = {0};
        bsp_node_bounds_create(parent_min, parent_max, &bounds);

        size_t num_polygons = loop_end - loop_start;
        log_assert( num_polygons > 0 );

        struct BspBuildPartition* partition = &state->front;
        if( parent_side == BSP_BACK ) {
            partition = &state->back;
        }

        // - this calls bsp_build_select_balanced_divider to determine a good divider
        // to be used for this branch
        int32_t node_divider_i = bsp_build_select_balanced_divider(tree, bounds, loop_start, loop_end, partition->polygons, num_polygons/10);

        // - here a node is added the is going to represent the current branch
        struct BspNode* node = NULL;
        int32_t node_i = bsp_tree_add_node(tree, parent_i, parent_side, num_polygons, bounds, node_divider_i, &node);

        const struct BspPolygon node_divider = tree->polygons.array[node_divider_i];
        Vertex node_divider_vertex = {0};
        vertex_copy(&tree->attributes.vertices[node_divider.start*VERTEX_SIZE], node_divider_vertex);

        size_t front_start = state->front.occupied;
        Vec3f front_min = {FLT_MAX, FLT_MAX, FLT_MAX};
        Vec3f front_max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

        size_t back_start = state->back.occupied;
        Vec3f back_min = {FLT_MAX, FLT_MAX, FLT_MAX};
        Vec3f back_max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

        // - this for loop goes through all polygons that belong to a branch and tests them against
        // the selected dividing polygon, it puts the polygon index into either the front or back partition,
        // or if the divider cuts through the polygon then it uses the polygon_cut function to create
        // new polygons from the cut and inserts them into the tree
        for( size_t loop_i = loop_start; loop_i < loop_end; loop_i++ ) {
            log_assert(partition->polygons[loop_i] >= 0);
            size_t polygon_i = (size_t)partition->polygons[loop_i];
            if( (int32_t)polygon_i == node_divider_i ) {
                continue;
            }

            const struct BspPolygon current_polygon = tree->polygons.array[polygon_i];
            size_t current_polygon_size = current_polygon.size;
            const VERTEX_TYPE* current_polygon_vertices = &tree->attributes.vertices[current_polygon.start*VERTEX_SIZE];
            const NORMAL_TYPE* current_polygon_normals = &tree->attributes.normals[current_polygon.start*NORMAL_SIZE];
            const TEXCOORD_TYPE* current_polygon_texcoords = &tree->attributes.texcoords[current_polygon.start*TEXCOORD_SIZE];
            const COLOR_TYPE* current_polygon_colors = &tree->attributes.colors[current_polygon.start*COLOR_SIZE];

            // - the polygon_cut is done here, it doubles as front/back test and its result_type is used in the
            // switch below to decide what to do with the current polygon
            size_t result_size = current_polygon_size;
            struct PolygonCutPoint result_points[current_polygon_size];
            enum PolygonCutType result_type = polygon_cut_test(current_polygon_size, VERTEX_SIZE, current_polygon_vertices,
                                                               node_divider.normal, node_divider_vertex,
                                                               result_size, result_points);

            // - I changed the building algorithm so that it (optionally) triangulates the polygons the result
            // from a cut, before it did not, and because when you cut a triangle you get a triangle and a quad,
            // and when you triangulate a quad you get two triangles, that means we need to handle (potentially)
            // two front polygons or two back polygons, thats why these guys are arrays of size 2
            int32_t front_index[2] = {-1, -1};
            int32_t back_index[2] = {-1, -1};
            switch(result_type) {
                // - when the polygon_cut function result_type indicates that this polygon is either coplannar to
                // the divider or completley in front or completely behind it, then this switch puts it into either
                // the front or back partition by setting front/back_index equal to this polygons index,
                // front/back_index is then later appended to the front or back partition when it is > -1
                case POLYGON_COPLANNAR:
                case POLYGON_FRONT:
                    front_index[0] = polygon_i;
                    // - accumulate min and max vec3fs for front here (and back just below obviously), this
                    // was previously done after the current switch statement but is now done inside because
                    // this way I can use the front_vertices for accumulation in the POLYGON_SPANNING case
                    for( size_t vertex_i = 0; vertex_i < current_polygon_size; vertex_i++ ) {
                        vec_minmax(&current_polygon_vertices[vertex_i*VERTEX_SIZE], front_min, front_max);
                    }
                    break;
                case POLYGON_BACK:
                    back_index[0] = polygon_i;
                    for( size_t vertex_i = 0; vertex_i < current_polygon_size; vertex_i++ ) {
                        vec_minmax(&current_polygon_vertices[vertex_i*VERTEX_SIZE], back_min, back_max);
                    }
                    break;
                case POLYGON_SPANNING:
                    if( result_points[0].num_cuts > 0 ) {
                        tree->polygons.array[polygon_i].cut.divider = node_divider_i;

                        // - if the polygon is cut through by the divider, then we need to create two new polygons,
                        // and these arrays here are what we use to collect the attributes for the two new polygons,
                        // so that means
                        // - we allocate these arrays locally and make them both large enough to hold the whole
                        // original polygon + the number of new vertices, which should actually give us something
                        // that is one vertex too large, but what the hell
                        log_assert(result_points[0].num_cuts >= 0);
                        size_t new_poly_size = current_polygon_size+(size_t)result_points[0].num_cuts;

                        VERTEX_TYPE front_vertices[new_poly_size*VERTEX_SIZE];
                        NORMAL_TYPE front_normals[new_poly_size*NORMAL_SIZE];
                        TEXCOORD_TYPE front_texcoords[new_poly_size*TEXCOORD_SIZE];
                        COLOR_TYPE front_colors[new_poly_size*COLOR_SIZE];

                        VERTEX_TYPE back_vertices[new_poly_size*VERTEX_SIZE];
                        NORMAL_TYPE back_normals[new_poly_size*NORMAL_SIZE];
                        TEXCOORD_TYPE back_texcoords[new_poly_size*TEXCOORD_SIZE];
                        COLOR_TYPE back_colors[new_poly_size*COLOR_SIZE];

                        // - for loop through the results from polygon_cut and put vertices into front and back
                        // - if there is an interpolation value then that means we are on an edge which has been cut, and we need
                        // to lerp the current vertex to the next vertex with the interpolation value to receive a new vertex that
                        // lies exactly where the edge was cut, we then put this new vertex into front AND back because its part
                        // of both resulting polygons
                        // - this code only looks like so much because the same thing for four different attributes
                        size_t front_occupied = new_poly_size;
                        size_t back_occupied = new_poly_size;
                        polygon_cut_split(current_polygon_size, (struct ParameterConstAttributes){ .vertices = current_polygon_vertices, .normals = current_polygon_normals, .texcoords = current_polygon_texcoords, .colors = current_polygon_colors },
                                          result_size, result_points,
                                          &front_occupied, (struct ParameterAttributes){ .vertices = front_vertices, .normals = front_normals, .texcoords = front_texcoords, .colors = front_colors},
                                          &back_occupied, (struct ParameterAttributes){ .vertices = back_vertices, .normals = back_normals, .texcoords = back_texcoords, .colors = back_colors});

                        // - we are still in the case that handles a cut polygon, the switch statement ends after the break; below!
                        // - now we are about to use the resulting attributes from the cut above to add new polygons, but before we
                        // do that we accumulate min and max vec3fs for both the front and back polygons that we got from the cut,
                        // this is done here because we can just easily just iterate over the vertices from the cut, which should be
                        // at most 4, instead of iterating over them later after the switch is done and after we triangulated the
                        // 4 vertices, which should then be 6 vertices to iterate over (also accessing them later is ugly, so this is
                        // just nicer)
                        for( size_t vertex_i = 0; vertex_i < front_occupied; vertex_i++ ) {
                            vec_minmax(&front_vertices[vertex_i*VERTEX_SIZE], front_min, front_max);
                        }

                        for( size_t vertex_i = 0; vertex_i < back_occupied; vertex_i++ ) {
                            vec_minmax(&back_vertices[vertex_i*VERTEX_SIZE], back_min, back_max);
                        }

                        struct BspPolygon* front_polygon[2] = {NULL, NULL};
                        struct ParameterAttributes parameter_front_attributes = {
                            .vertices = front_vertices,
                            .normals = front_normals,
                            .texcoords = front_texcoords,
                            .colors = front_colors
                        };

                        struct BspPolygon* back_polygon[2] = {NULL, NULL};
                        struct ParameterAttributes parameter_back_attributes = {
                            .vertices = back_vertices,
                            .normals = back_normals,
                            .texcoords = back_texcoords,
                            .colors = back_colors
                        };

                        // - this section adds the new polygons that resulted from the cut to the tree
                        // - if the one of the polygons to add has more then 3 vertices, then we need to triangulate it (optionally),
                        // since when triangulate is true we _always_ triangulate, we can only get a quad here to triangulate,
                        // which makes it easy: just take the first three vertices, and then the first, third and last vertex
                        // - so the first bsp_tree_add_polygon call just adds the first 3 vertices, then I loop over the vertices
                        // and rearrange them, then add the second triangle
                        // - notice that the indices in other[3] = {0,2,3} are the indices where I pick from, and these
                        // need to be increasing indices, something like {2,3,0} would not work because the for loop would overwrite the
                        // value at index 0 with the value found at index 2
                        if( triangulate && front_occupied > 3 ) {
                            log_assert(front_occupied == 4);

                            front_index[0] = bsp_tree_add_polygon(tree, 3, current_polygon.normal, parameter_front_attributes, &front_polygon[0]);
                            front_polygon[0]->cut.parent = polygon_i;

                            size_t other[3] = {0,2,3};
                            for( size_t triangle_i = 0; triangle_i < 3; triangle_i++ ) {
                                vertex_copy(&front_vertices[other[triangle_i]*VERTEX_SIZE], &front_vertices[triangle_i*VERTEX_SIZE]);
                                normal_copy(&front_normals[other[triangle_i]*NORMAL_SIZE], &front_normals[triangle_i*NORMAL_SIZE]);
                                texcoord_copy(&front_texcoords[other[triangle_i]*TEXCOORD_SIZE], &front_texcoords[triangle_i*TEXCOORD_SIZE]);
                                color_copy(&front_colors[other[triangle_i]*COLOR_SIZE], &front_colors[triangle_i*COLOR_SIZE]);
                            }
                            front_index[1] = bsp_tree_add_polygon(tree, 3, current_polygon.normal, parameter_front_attributes, &front_polygon[1]);
                            front_polygon[1]->cut.parent = polygon_i;
                        } else {
                            front_index[0] = bsp_tree_add_polygon(tree, front_occupied, current_polygon.normal, parameter_front_attributes, &front_polygon[0]);
                            front_polygon[0]->cut.parent = polygon_i;
                        }

                        // - same as above, but for back polygon
                        if( triangulate && back_occupied > 3 ) {
                            log_assert(back_occupied == 4);

                            back_index[0] = bsp_tree_add_polygon(tree, 3, current_polygon.normal, parameter_back_attributes, &back_polygon[0]);
                            back_polygon[0]->cut.parent = polygon_i;

                            size_t other[3] = {0,2,3};
                            for( size_t triangle_i = 0; triangle_i < 3; triangle_i++ ) {
                                vertex_copy(&back_vertices[other[triangle_i]*VERTEX_SIZE], &back_vertices[triangle_i*VERTEX_SIZE]);
                                normal_copy(&back_normals[other[triangle_i]*NORMAL_SIZE], &back_normals[triangle_i*NORMAL_SIZE]);
                                texcoord_copy(&back_texcoords[other[triangle_i]*TEXCOORD_SIZE], &back_texcoords[triangle_i*TEXCOORD_SIZE]);
                                color_copy(&back_colors[other[triangle_i]*COLOR_SIZE], &back_colors[triangle_i*COLOR_SIZE]);
                            }
                            back_index[1] = bsp_tree_add_polygon(tree, 3, current_polygon.normal, parameter_back_attributes, &back_polygon[1]);
                            back_polygon[1]->cut.parent = polygon_i;
                        } else {
                            back_index[0] = bsp_tree_add_polygon(tree, back_occupied, current_polygon.normal, parameter_back_attributes, &back_polygon[0]);
                            back_polygon[0]->cut.parent = polygon_i;
                        }

                        // - set siblings here when I know bot back_ and front_index
                        front_polygon[0]->cut.sibling = back_index[0];
                        if( front_polygon[1] != NULL ) {
                            // - it is not immediatly obvious, but there should never be a situation where when the front_polygon
                            // is triangulated into two triangles, the back polygon is triangulated into two triangles as well, thats
                            // why I assert here that there is only one back polygon
                            log_assert(back_polygon[1] == NULL);
                            front_polygon[1]->cut.sibling = back_index[0];
                        }
                        back_polygon[0]->cut.sibling = front_index[0];
                        if( back_polygon[1] != NULL ) {
                            log_assert(front_polygon[1] == NULL);
                            back_polygon[1]->cut.sibling = front_index[0];
                        }
                    }
                    // - there is no need to put an else here, polygon_cut should not report a cut for a single coplannar vertex,
                    // so only if there are actually vertices in front and behind the divider polygon_cut will return POLYGON_SPANNING
                    break;
            }

            // - after the switch statement where we decided where to put the current polygon or cut it into two
            // new polygons, we append the front_ or back_index to the front or back partition, or both to both
            // partitions, those polygons will be then processed in the future when we process those sub-branches
            // -+we also look at each polygon vertex here and accumulate the min and max vec3fs for the sub-branches+
            // - accumulation of min and max is NOT done here anymore, it moved into the switch statement above
            if( front_index[1] > -1 ) {
                if( state->front.occupied + 2 >= state->front.capacity ) {
                    size_t alloc_state_result = bsp_build_partition_alloc(&state->front, 2);
                    log_assert( alloc_state_result > 2 );
                }

                state->front.polygons[state->front.occupied+0] = front_index[0];
                state->front.polygons[state->front.occupied+1] = front_index[1];
                state->front.occupied += 2;
            } else if( front_index[0] > -1 ) {
                if( state->front.occupied + 1 >= state->front.capacity ) {
                    size_t alloc_state_result = bsp_build_partition_alloc(&state->front, 1);
                    log_assert( alloc_state_result > 1 );
                }

                state->front.polygons[state->front.occupied] = front_index[0];
                state->front.occupied += 1;
            }

            if( back_index[1] > -1 ) {
                if( state->back.occupied + 2 >= state->back.capacity ) {
                    size_t alloc_state_result = bsp_build_partition_alloc(&state->back, 2);
                    log_assert( alloc_state_result > 2 );
                }

                state->back.polygons[state->back.occupied+0] = back_index[0];
                state->back.polygons[state->back.occupied+1] = back_index[1];
                state->back.occupied += 2;

            } else if( back_index[0] > -1 ) {
                if( state->back.occupied + 1 >= state->back.capacity ) {
                    size_t alloc_state_result = bsp_build_partition_alloc(&state->back, 1);
                    log_assert( alloc_state_result > 1 );
                }

                state->back.polygons[state->back.occupied] = back_index[0];
                state->back.occupied += 1;
            }
        }

        // - front_start and back_start are set above before processing and adding polygons to the front
        // and back partitions, together with front_end and back_end the front_start/end and back_start/end
        // tuples define the range of polygons to be processed in the front and back sub-branches
        size_t front_end = state->front.occupied;
        size_t back_end = state->back.occupied;

        // - test if back_end - back_start > 0 to determine if there are polygons to be processed in the back
        // sub-branch, if that is true we need to push a new stack frame onto the stack that describes that
        // sub-branch
        // - if there are no polygons to be processed in the back sub-branch, then we mark the current node
        // as solid node, meaning that behind this nodes divider is inside the mesh
        // - I changed the code so that when there are no polygons in a sub-branch we do nothing, meaning
        // the tree.front and tree.back indices stay -1 for the current node and no state.solid and state.empty
        // booleans are neccessary
        if( back_end - back_start > 0 ) {
            struct BspBuildStackFrame back_frame;
            back_frame.tree_side = BSP_BACK;
            back_frame.parent_index = node_i;
            back_frame.partition_start = back_start;
            back_frame.partition_end = back_end;
            vec_copy3f(back_min, back_frame.bounds_min);
            vec_copy3f(back_max, back_frame.bounds_max);

            bsp_build_stack_push(&state->stack, back_frame);
        }

        // - same as above, but for front
        if( front_end - front_start > 0 ) {
            struct BspBuildStackFrame front_frame;
            front_frame.tree_side = BSP_FRONT;
            front_frame.parent_index = node_i;
            front_frame.partition_start = front_start;
            front_frame.partition_end = front_end;
            vec_copy3f(front_min, front_frame.bounds_min);
            vec_copy3f(front_max, front_frame.bounds_max);

            bsp_build_stack_push(&state->stack, front_frame);
        }

    }

    return root;
}
