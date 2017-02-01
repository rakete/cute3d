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

#include "geometry_solid.h"
#include "geometry_types.h"
#include "geometry_draw.h"
#include "geometry_polygon.h"

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

struct BspNodeBounds {
    float half_width;
    float half_height;
    float half_depth;
    Vec3f center;
    size_t num_polygons;
};

void bsp_node_bounds_create(Vec3f min, Vec3f max, size_t num_polygons, struct BspNodeBounds* bounds);

struct BspNode {
    int32_t divider;

    struct BspNodeBounds bounds;

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

void bsp_node_create(struct BspNodeBounds bounds, struct BspNode* node);

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

WARN_UNUSED_RESULT size_t bsp_tree_alloc_attributes(struct BspTree* tree, size_t n);
WARN_UNUSED_RESULT size_t bsp_tree_alloc_polygons(struct BspTree* tree, size_t n);
WARN_UNUSED_RESULT size_t bsp_tree_alloc_nodes(struct BspTree* tree, size_t n);

int32_t bsp_tree_add_node(struct BspTree* tree, struct BspNodeBounds bounds, struct BspNode** node);
//void bsp_tree_connect_node(struct BspTree* tree, struct BspNode* node, struct BspNode* front, struct BspNode* back);

int32_t bsp_tree_add_polygon(struct BspTree* tree, size_t polygon_size, const Vec3f polygon_normal, VERTEX_TYPE* polygon_vertices, struct BspPolygon** polygon);
//void bsp_tree_set_polygon_vertices(struct BspTree* tree, const struct BspPolygon* polygon, size_t vertices_size, float* vertices);

void bsp_tree_create_from_solid(struct Solid* solid, struct BspTree* tree);

struct BspBuildPartition {
    int32_t* polygons;
    size_t capacity;
    size_t occupied;
};

struct BspBuildArrays {
    struct BspBuildPartition front;
    struct BspBuildPartition back;
};

void bsp_build_arrays_create(struct BspBuildArrays* arrays);
WARN_UNUSED_RESULT size_t bsp_build_arrays_alloc(struct BspBuildArrays* arrays, size_t front_n, size_t back_n);

void bsp_build_select_balanced_divider(const struct BspTree* tree, struct BspNodeBounds bounds, size_t num_polygons, const int32_t* polygon_indices, size_t max_steps, int32_t* selected_divider);
void bsp_build_recur(struct BspTree* tree, struct BspNode* node, struct BspBuildArrays* arrays, size_t start, size_t end, struct BspBuildPartition* partition);

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
