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
// is the first and a size which tells me how many vertices belong to the polygon, again, remember the
// indices unit is vertices, so start*VERTEX_SIZE is the actual index into tree->attributes.vertices
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
    Vec3f half_size;
    Vec3f center;
};

void bsp_node_bounds_create(Vec3f min, Vec3f max, struct BspBounds* bounds);

enum BspSide {
    BSP_NOSIDE = -1,
    BSP_FRONT = 0,
    BSP_BACK
};

// - nodes are what the trees structure is built of, a node can be at a branch or at a leaf position
// - divider is a index that specifies which divider polygon was used to divide the polygons of this
// node into front and back subtrees
// - bounds enclose all vertices which belong to the subtrees of this node
// - tree.parent is a index which specifies a parent node
// - tree.front and tree.back are indices to the nodes that represent the front and back subtrees
// - tree.index is this nodes index inside the tree array
// - tree.depth is how deep this node is in the tree, how far this node is away from the root
struct BspNode {
    int32_t divider;
    size_t num_polygons;

    struct BspBounds bounds;

    struct {
        enum BspSide side;
        int32_t parent;
        int32_t front;
        int32_t back;
        int32_t index;
        int32_t depth;
    } tree;
};

void bsp_node_create(struct BspNode* node);

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
// between back_i < front_i+num_polygons should contain all back polygon indices, so in that way
// you can interpret a node as containing multiple polygons (as nodes)
// - remember that while the tree is originally filled with triangles, the polygons are not neccessarily
// triangles, they are polygons! when the build algorithm cuts a triangle, it can result in a
// triangle and a quad, and that quad may be cut again... so when looking at the polygons you
// may need to triangulate them before using them
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

// - bsp_tree_add_node will add a new empty node to the tree, connected to the parent index,
// and it returns the index of the new node
// - it is important to fill the node after adding it, the bsp_tree_add_node leaves it completely
// empty apart from the parent, index and depth values, that why this function takes a pointer
// to a pointer to a node, so the user gets a pointer to the added node after adding it
int32_t bsp_tree_add_node(struct BspTree* tree, int32_t parent, enum BspSide side, struct BspNode** node);
// - bsp_tree_add_polygon adds a new polygon and also fills it with the attributes supplied in the
// polygon_attributes argument
int32_t bsp_tree_add_polygon(struct BspTree* tree, size_t polygon_size, const Vec3f polygon_normal, struct ParameterAttributes polygon_attributes, struct BspPolygon** polygon);

// - bsp_tree_create_from_solid is the function a user is supposed to call when he wants to create
// a bsp tree, from a solid obviously, I planned to have other functions for halfedgemesh etc.
// - it returns a pointer to the root node
struct BspNode* bsp_tree_create_from_solid(struct Solid* solid, struct BspTree* tree);

// - beyond this point only data structures and functions for building the tree are defined
// - these are supposed to be called internally only
// - since I rewrote my original recursive building function to be iterative, I needed to implement
// my own stack, that what struct BspBuildStackFrame and BspBuildStack are
// - each BspBuildStackFrame contains everything that I need to keep track of to continue building
// a branch once I have finished with the other, so for example when the iterative building starts,
// it will first pick a divider and sort polygons into front and back partitions, then put two stack
// frames on the stack, one for the back partition and one for the front partition, then on the
// next iteration it pops the stack and continues building with the front partition and does
// the same thing there. this continues until there are no polygons left in the front partition, at
// which point the only frame left on the stack is the frame for the first back partition, which
// gets popped and the building algorithm starts going down that branch
// - tree side tells me which branch I am working on, I need this to select the correct partition to
// process
// - parent_index I need to append the next node to the correct parent index
// - partiton_start and partition_end are the indices into the partition which I need
// to process for this branch
// - bounds_min and bounds_max are neccessary because I calculate them when processing the polygons
// for a branch, but only need them when creating a node for the next branch into which I sorted
// those polygons (front or back)
// - notice I could easily create nodes right after processing the polygons, and therefore eliminate
// some of these variables I need to keep here, but I choose not to because when I create the nodes
// only right before processing a branch, the nodes in the tree will be ordered as such that all nodes
// between front_i and back_i belong to the front branch, and all nodes between back_i and
// front_i+num_polygons belong to the back branch, if I change when the nodes are created, I also
// change that ordering!
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

// - I needed a way to accumulate all indices that need to be processed for the next
// front or back branch, a partition is temporary storage for those indices
// - the indices are only appended, there is no reuse or clearing involved, so the same
// index will be appended repeatedly as often as the corresponding polygon shows up
// in a sub-branch (it 'disappears' when it is selected as divider and becomes a node)
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

// - I decided against making create and destroy functions for frame, stack and partion, they are
// all handled together by state_create and state_destroy
void bsp_build_state_create(struct BspBuildState* arrays);
void bsp_build_state_destroy(struct BspBuildState* arrays);

// - bsp_build_select_balanced_divider is in the internal section but should eventually
// go in the public section when I ever have more then only this one function to select
// a divider
// - it goes through the polygons represented by polygon_indices, from loop_start to loop_end
// and selects a polygon that is as near as possible to the center of bounds, the argument
// max_steps can be used to restrict how many polygons will be tested so that this function
// won't take too much time
// - it returns the index of the polygon that represents a divider plane that should result
// in the most balanced cut of the mesh
int32_t bsp_build_select_balanced_divider(const struct BspTree* tree, struct BspBounds bounds, size_t loop_start, size_t loop_end, const int32_t* polygon_indices, size_t max_steps);

// - bsp_build builds a bsp tree, it is not very user friendly
// - to use bsp_build first the input mesh should be processed and tree->attributes
// and tree->polygons must be filled with the attributes and polygons of the mesh,
// then state is to be initialized and the state->front partition should be filled with
// the indices of the polygons inside the tree to be processed (basicly the indices
// should just be numbers from 0 to num_polygons, so that should not be very difficult),
// and then finally a stack frame must be created and pushed on state->state which
// contains the neccessary data to process the root node: side is whatever side contains
// the initial polygon indices (BSP_FRONT in this example), parent should be -1 since
// the root has no parent, partition_start is probably 0 and partition_start is
// num_polygons, and bounds_min and bounds_max are the vec3fs containing the minimum
// and maximum xyz values of the meshes vertices
// - just use bsp_create_from_solid or at least look at its source for an example
// how to use this function
// - I added root_frame argument to make the creation of the stack frame representing
// the root branch explicit
// - I added triangulation to the build process then realized it may have a non
// significant impact on the performance, so I added the bool triangulate argument
// so that I can easily disable triangulation if needed
struct BspNode* bsp_build(struct BspTree* tree, struct BspBuildStackFrame root_frame, bool triangulate, struct BspBuildState* state);

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
