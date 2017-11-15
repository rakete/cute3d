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

#ifndef GEOMETRY_BSP_H
#define GEOMETRY_BSP_H

#include "stdint.h"
#include "stddef.h"

#include "math_matrix.h"
#include "math_draw.h"
#include "math_geometry.h"

#include "geometry_solid.h"
#include "geometry_types.h"
#include "geometry_polygon.h"
#include "geometry_parameter.h"

#ifndef BSP_ATTRIBUTES_ALLOC
#define BSP_ATTRIBUTES_ALLOC 2048
#endif

#ifndef BSP_POLYGONS_ALLOC
#define BSP_POLYGONS_ALLOC 512
#endif

#ifndef BSP_NODES_ALLOC
#define BSP_NODES_ALLOC 512
#endif

#ifndef BSP_BUILD_ARRAYS_ALLOC
#define BSP_BUILD_ARRAYS_ALLOC 512
#endif

#ifndef BSP_BUILD_STACK_ALLOC
#define BSP_BUILD_STACK_ALLOC 512
#endif

// - the point of the bsp tree is to take an existing triangle mesh and sort those triangles into binary
// branches lying in front or behind a dividing plane (selected from the triangles too), while doing this
// sorting I have to occasionally cut triangles into two polygons if they intersect the dividing plane
// - the vertices and their attributes are stored in struct BspTree in a continuous arrays, and whenever
// I cut a polygon, I append new vertices to these arrays so that I leave the old polygon vertices where
// they are and get two new polygons, which their vertices in correct order, appended to the end of the
// arrays
// - the struct BspPolygon represents such a polygon, it contains a start index which tells me which vertex
// is the first and a size which tells me how many vertices belong to the polygon
// - it also contains a normal because I need those when constructing the bsp tree and dont want to recalculate
// them repeatedly
// - divider is a index which specifies which other polygon divided this polygon, if any
// - cut.parent is a index, which specifies the original polygon that has been cut to result in this
// new polygon
// - cut.sibling is a index which specifies the other half of this polygon if this resulted from a cut
struct BspPolygon {
    size_t start;
    size_t size;
    Vec3f normal;

    int32_t divider;

    struct {
        int32_t parent;
        int32_t sibling;
    } cut;
};

void bsp_polygon_create(struct BspPolygon* poly);

// - convenient to have and easy to gather while constructing the bsp tree, this is used to store the bounds
// of each subtree for later use
struct BspBounds {
    float half_width;
    float half_height;
    float half_depth;
    Vec3f center;
    size_t num_polygons;
};

void bsp_node_bounds_create(Vec3f min, Vec3f max, size_t num_polygons, struct BspBounds* bounds);

// - nodes are what the trees structure is built of, a node can be at a branch or at a leaf position
// - divider is a index that specifies which divider polygon was used to divide the polygons of this
// node into front and back subtrees
// - bounds enclose all vertices which belong to the subtrees of this node
// - tree.parent is a index which specifies a parent node
// - tree.front and tree.back are indices to the nodes that represent the front and back subtrees
// - tree.index is this nodes index inside the tree array
// - tree.depth is how deep this node is in the tree, how far this node is away from the root
// - state.empty and state.solid are for leaf nodes to specify if they sit on the 'outside' (empty)
// or 'inside' (solid) of the mesh
struct BspNode {
    int32_t divider;

    struct BspBounds bounds;

    struct {
        int32_t parent;
        int32_t front;
        int32_t back;
        int32_t index;
        int32_t depth;
    } tree;

    struct {
        bool empty;
        bool solid;
        bool disabled;
    } state;
};

void bsp_node_create(struct BspBounds bounds, struct BspNode* node);

// - the bsp tree itself contains just arrays containing the structs described above
// - the attribute arrays are supposed to be filled with vertex data with non shared vertices,
// the same way as it is organized in an unoptimized solid
// - it follows my common pattern where I have a pointer, a capacity and an occupied counter
// grouped together to form a growing array
// - notice that the polygons and the nodes are seperate, every node contains a index into the
// nodes array, the divider, which tells me which polygon has been used as divider at that
// tree branch, this means that there is one node for each polygon, but this may change later
// because I think it may make sense to have a bsp tree where a node contains multiple polygons
// - despite each node containing exactly one polygon, the way the tree is built should result in
// a nodes array such that if I look at a node, and take its front index and its back index, then
// the nodes between front_i < back_i should contain all front polygon indices, and the nodes
// between back_i < front_i+num_polygons should contain all back polygon indices
struct BspTree {
    struct {
        VERTEX_TYPE* vertices;
        NORMAL_TYPE* normals;
        TEXCOORD_TYPE* texcoords;
        COLOR_TYPE* colors;
        size_t capacity;
        size_t occupied;
    } attributes;

    struct {
        struct BspPolygon* array;
        size_t capacity;
        size_t occupied;
    } polygons;

    struct {
        struct BspNode* array;
        size_t capacity;
        size_t occupied;
    } nodes;
};

void bsp_tree_create(struct BspTree* tree);
void bsp_tree_destroy(struct BspTree* tree);

WARN_UNUSED_RESULT size_t bsp_tree_alloc_attributes(struct BspTree* tree, size_t n);
WARN_UNUSED_RESULT size_t bsp_tree_alloc_polygons(struct BspTree* tree, size_t n);
WARN_UNUSED_RESULT size_t bsp_tree_alloc_nodes(struct BspTree* tree, size_t n);

int32_t bsp_tree_add_node(struct BspTree* tree, int32_t parent, struct BspBounds bounds, struct BspNode** node);
int32_t bsp_tree_add_polygon(struct BspTree* tree, size_t polygon_size, const Vec3f polygon_normal, struct ParameterAttributes polygon_attributes, struct BspPolygon** polygon);

void bsp_tree_create_from_solid(struct Solid* solid, struct BspTree* tree);

enum BspSide {
    BSP_FRONT = 0,
    BSP_BACK
};

struct BspBuildStackFrame {
    enum BspSide tree_side;
    int32_t parent_index;

    size_t partition_start;
    size_t partition_end;

    Vec3f bounds_min;
    Vec3f bounds_max;
};

struct BspBuildStack {
    struct BspBuildStackFrame* frames;
    size_t capacity;
    size_t occupied;
};

size_t bsp_build_stack_pop(struct BspBuildStack* stack, struct BspBuildStackFrame* frame);
size_t bsp_build_stack_push(struct BspBuildStack* stack, struct BspBuildStackFrame frame);

WARN_UNUSED_RESULT size_t bsp_build_stack_alloc(struct BspBuildStack* stack, size_t n);

struct BspBuildPartition {
    int32_t* polygons;
    size_t capacity;
    size_t occupied;
};

WARN_UNUSED_RESULT size_t bsp_build_partition_alloc(struct BspBuildPartition* partition, size_t n);

struct BspBuildState {
    struct BspBuildStack stack;

    struct BspBuildPartition front;
    struct BspBuildPartition back;
};

void bsp_build_state_create(struct BspBuildState* arrays);
void bsp_build_state_destroy(struct BspBuildState* arrays);

int32_t bsp_build_select_balanced_divider(const struct BspTree* tree, struct BspBounds bounds, size_t loop_start, size_t loop_end, const int32_t* polygon_indices, size_t max_steps);

struct BspNode* bsp_build(struct BspTree* tree, struct BspBuildState* state);

// 1. select a primitive (triangle) not yet part of the tree
// 2. go through all other triangles seperating them in two
//    branches of triangles in front, and triangles behind the
//    selected one
// 3. seperation is done by testing every vertex of a triangle
//    against the selected triangles normal (project on normal,
//    decide by sign of dot product)
//
// 3.1 if a triangles vertices are on both side of the selected
//     triangle, this triangle is seperated by the selected triangle
//     into two new triangles, and needs to be cut
// 3.2 cutting a triangle means finding two new vertices along those
//     edges that are defined by vertices on opposite sides of the
//     selected triangle
// 3.3 additionally, after finding the two new vertices, new normals
//     and new texcoords need to be computed as well
// 3.4 eventually we run out of space for new vertices etc, so we have
//     to reserve memory locally and cache the new stuff until we can
//     push it to the gpu later
//
// 4. triangles have to be sorted into two leafes seperating them into
//    in-front-of-root and behind-of-root, then the whole process has
//    to be repeated for each of the triangles in the two leafes, seperating
//    them even further and creating the tree, until every leaf of
//    the tree contains exactly one triangle
//
//
// so I am going to have to make a few assumptions here:
// - I get triangles as input, in form of indices
// - the seperation test needs face normals, I can compute
//   those by taking the crossproduct of two edges of a
//   triangle

#endif
