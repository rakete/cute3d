#ifndef GEOMETRY_HALFEDGEMESH_H
#define GEOMETRY_HALFEDGEMESH_H

#include "limits.h"

#include "math_types.h"

#include "geometry_solid.h"

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
    // position looks like it would be nice in struct HalfEdge, but its here because it is more
    // convenient to be able to insert the vertices seperately from the edges
    Vec4f position;
    int32_t edge;
};

struct HalfEdgeFace {
    Vec4f normal;
    int32_t size;
    int32_t edge;
};

struct HalfEdge {
    // - why put these here, and not in vertex? because we want to represent all kinds of meshes,
    // but mostly ones with vertices that each have their own normal/color/texcoord, just like
    // I do for solids so I can get hard edges when rendering
    Vec4f normal;
    Texcoord texcoord;
    Color color;

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
int32_t halfedgemesh_create(struct HalfEdgeMesh* mesh);
int32_t halfedgemesh_destroy(struct HalfEdgeMesh* mesh);

// allocation functions, return allocated bytes or zero
size_t halfedgemesh_alloc_vertices(struct HalfEdgeMesh* mesh, size_t n);
size_t halfedgemesh_alloc_faces(struct HalfEdgeMesh* mesh, size_t n);
size_t halfedgemesh_alloc_edges(struct HalfEdgeMesh* mesh, size_t n);

// appending a complete solid at once, that encodes the assumption that the input data is
// in a certain format, so I choose to use only this instead of a more generic function
void halfedgemesh_append(struct HalfEdgeMesh* mesh, const struct Solid* solid);

int32_t halfedgemesh_face_normal(struct HalfEdgeMesh* mesh, int32_t face_i, int32_t all_edges, Vec3f equal_normal, Vec3f average_normal);
int32_t halfedgemesh_face_iterate(struct HalfEdgeMesh* mesh, int32_t face_i, struct HalfEdge** edge, int32_t* edge_i, int32_t* i);
int32_t halfedgemesh_vertex_iterate(struct HalfEdgeMesh* mesh, int32_t vertex_i, struct HalfEdge** edge, int32_t* edge_i, int32_t* i);

// optimize is supposed to look at faces and collapse smaller faces into a larger face with size face_size,
// if the normals are equal. so for example a box made out of triangles could be compressed into a box of
// quads
// - if I implement this to only collapse faces into convex polygons, I could support face_size > 3 and would
// not have to implement a complex triangulation algorithm
// - this could also clean up and shrink arrays in case I'd ever implement something that needs to delete vertices,
// edges or faces
void halfedgemesh_optimize(struct HalfEdgeMesh* mesh);

void halfedgemesh_verify(const struct HalfEdgeMesh* mesh);

#endif
