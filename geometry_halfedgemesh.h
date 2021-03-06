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

#ifndef GEOMETRY_HALFEDGEMESH_H
#define GEOMETRY_HALFEDGEMESH_H

#include "limits.h"

#include "math_types.h"
#include "math_geometry.h"

#include "geometry_solid.h"

#include "driver_platform.h"

#ifndef HALFEDGEMESH_VERTICES_ALLOC
#define HALFEDGEMESH_VERTICES_ALLOC 512
#endif

#ifndef HALFEDGEMESH_FACES_ALLOC
#define HALFEDGEMESH_FACES_ALLOC 512
#endif

#ifndef HALFEDGEMESH_EDGES_ALLOC
#define HALFEDGEMESH_EDGES_ALLOC 512
#endif

struct HalfEdge;

// halfedge meshes are more efficient when I need to do operations that involve neighborhoods
// of vertices, I implemented them originally for collision detection, but they might be handy
// for anything that attempt to modify the mesh as well

// the mesh data (vertices, normals, ...) is not stored in a continuous array like in the solids,
// this means I can't just upload the mesh to the gpu and render, but I first have to iterate the
// data structure and fill the buffer, I thought about storing the data in extra arrays in struct
// HalfEdgeMesh, but that would be impossible for the indices (so I'd have to iterate anyways),
// and isn't as straightforward since this mesh has unique vertices, so that means that for each
// vertex there may be multiple normals, texcoords, etc, and I would have to manage more state
// to keep it all well aligned plus it would still need preprocessing since gl can only assign
// one attribute per vertex
struct HalfEdgeVertex {
    // - position looks like it would be nice in struct HalfEdge, but its here because it is more
    // convenient to be able to insert the vertices seperately from the edges
    Vec4f position;

    // - the index of any of the outgoing edges of this vertex
    int32_t edge;
    int32_t this;
};

struct HalfEdgeFace {
    Vec4f normal;
    int32_t size;
    // - the index of any of the edges of this face
    int32_t edge;
    int32_t this;
};

struct HalfEdge {
    // - why put these here, and not in vertex? because we want to represent all kinds of meshes,
    // but mostly ones with vertices that each have their own normal/color/texcoord, just like
    // I do for solids so I can get hard edges when rendering
    Vec4f normal;
    Texcoord texcoord;
    Color color;

    // - the index of the vertex this edge points to
    int32_t vertex;

    int32_t face;

    int32_t next;
    int32_t this;
    int32_t prev;
    int32_t other;
};

struct HalfEdgeMesh {
    int32_t size;

    struct {
        size_t capacity;
        size_t occupied;
        struct HalfEdgeVertex* array;
    } vertices;

    struct {
        size_t capacity;
        size_t occupied;
        struct HalfEdgeFace* array;
    } faces;

    struct {
        size_t capacity;
        size_t occupied;
        struct HalfEdge* array;
    } edges;
};

// since I need to make use of realloc, a destructor is needed
void halfedgemesh_create(struct HalfEdgeMesh* mesh);
void halfedgemesh_destroy(struct HalfEdgeMesh* mesh);

// allocation functions, return allocated bytes or zero
WARN_UNUSED_RESULT size_t halfedgemesh_alloc_vertices(struct HalfEdgeMesh* mesh, size_t n);
WARN_UNUSED_RESULT size_t halfedgemesh_alloc_faces(struct HalfEdgeMesh* mesh, size_t n);
WARN_UNUSED_RESULT size_t halfedgemesh_alloc_edges(struct HalfEdgeMesh* mesh, size_t n);

// - appending a complete solid at once, that encodes the assumption that the input data is
// in a certain format, so I choose to use only this instead of a more generic function
void halfedgemesh_append(struct HalfEdgeMesh* mesh, const struct Solid* solid);

int32_t halfedgemesh_face_iterate(const struct HalfEdgeMesh* mesh, int32_t face_i, struct HalfEdge** edge, int32_t* edge_i, int32_t* i);
int32_t halfedgemesh_vertex_iterate(const struct HalfEdgeMesh* mesh, int32_t vertex_i, struct HalfEdge** edge, int32_t* edge_i, int32_t* i);

void halfedgemesh_vertex_surface_normal(const struct HalfEdgeMesh* mesh, int32_t vertex_i, Vec3f surface_normal);

// - optimize is supposed to look at faces and collapse smaller faces into a larger face with size face_size,
// if the normals are equal. so for example a box made out of triangles could be compressed into a box of
// quads
// - if I implement this to only collapse faces into convex polygons, I could support face_size > 3 and would
// not have to implement a complex triangulation algorithm
// - this could also clean up and shrink arrays in case I'd ever implement something that needs to delete vertices,
// edges or faces
void halfedgemesh_optimize(struct HalfEdgeMesh* mesh);

void halfedgemesh_verify(const struct HalfEdgeMesh* mesh);

#endif
