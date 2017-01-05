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

#ifndef BSP_DEFAULT_ALLOC
#define BSP_DEFAULT_ALLOC 4096
#endif

void polygon_triangulate(size_t polygon_size, size_t point_size, float* polygon, size_t result_size, size_t* result);

void polygon_normal(size_t polygon_size, size_t point_size, float* polygon, Vec3f result_normal);

// a bsp tree is not a geometrical structure itself, it is more
// like a description of a mesh that can be used to, for example,
// render or modify it efficiently

// triangles get put into binary branches by whether side of a leaf triangle they are of
// so we need to construct all triangles from a vertex_buffer and element_buffer
struct BspTree;

struct BspPoly {
    float* polygon;
    size_t size;
    Vec3f normal;
};

struct BspNode {
    struct BspPoly divider;

    struct {
        struct BspNode* nodes;
        size_t capacity;
        size_t occupied;
    } front;

    struct {
        struct BspNode* nodes;
        size_t capacity;
        size_t occupied;
    } back;
};

void bsp_node_create(struct BspNode* node);
void bsp_node_destroy(struct BspNode* node);

void bsp_node_build(struct BspNode* node, const struct BspTree* tree, size_t triangles_size, int32_t triangles);

struct BspTree {
    struct {
        float* vertices;
        size_t capacity;
        size_t occupied;
    } attributes;
};

void bsp_tree_create(struct BspTree* tree);

void bsp_tree_build(struct BspTree* tree, struct Solid* solid);

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
