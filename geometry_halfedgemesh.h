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
    Vec position;
    unsigned int edge;
};

struct HalfEdgeFace {
    unsigned int size;
    unsigned int edge;
};

struct HalfEdge {
    Vec normal;
    Texcoord texcoord;
    Color color;

    unsigned int vertex;

    unsigned int face;

    unsigned int next;
    unsigned int this;
    unsigned int prev;
    unsigned int other;
};

struct HalfEdgeMesh {
    unsigned int size;

    struct {
        unsigned int capacity;
        unsigned int reserved;
        struct HalfEdgeVertex* array;
    } vertices;

    struct {
        unsigned int capacity;
        unsigned int reserved;
        struct HalfEdgeFace* array;
    } faces;

    struct {
        unsigned int capacity;
        unsigned int reserved;
        struct HalfEdge* array;
    } edges;
};

// since I need to make use of realloc, a destructor is needed
int halfedgemesh_create(struct HalfEdgeMesh* mesh);
int halfedgemesh_destroy(struct HalfEdgeMesh* mesh);

// allocation functions, return allocated bytes or zero
unsigned int halfedgemesh_alloc_vertices(struct HalfEdgeMesh* mesh, unsigned int n);
unsigned int halfedgemesh_alloc_faces(struct HalfEdgeMesh* mesh, unsigned int n);
unsigned int halfedgemesh_alloc_edges(struct HalfEdgeMesh* mesh, unsigned int n);

// appending a complete solid at once, that encodes the assumption that the input data is
// in a certain format, so I choose to use only this instead of a more generic function
void halfedgemesh_append(struct HalfEdgeMesh* mesh, const struct Solid* solid);

// flush is supposed to put the data into contiuous arrays suitable for gl buffers, so for rendering
// this is probably most usefull for testing, I don't think the data structure should be used to
// hold renderables
void halfedgemesh_flush(const struct HalfEdgeMesh* mesh, struct Solid* solid);

int halfedgemesh_face_normal(struct HalfEdgeMesh* mesh, unsigned int face_i, Vec3f equal_normal, Vec3f average_normal);
int halfedgemesh_face_iterate(struct HalfEdgeMesh* mesh, unsigned int face_i, struct HalfEdge** edge, unsigned int* edge_i, unsigned int* i);
int halfedgemesh_vertex_iterate(struct HalfEdgeMesh* mesh, unsigned int vertex_i, struct HalfEdge** edge, unsigned int* edge_i, unsigned int* i);

// compress is supposed to look at faces and collapse smaller faces into a larger face with size face_size,
// if the normals are equal. so for example a box made out of triangles could be compressed into a box of
// quads
// - if I implement this to only collapse faces into convex polygons, I could support face_size > 3 and would
// not have to implement a complex triangulation algorithm
// - this could also clean up and shrink arrays in case I'd ever implement something that needs to delete vertices,
// edges or faces
void halfedgemesh_compress(struct HalfEdgeMesh* mesh);

void halfedgemesh_verify(struct HalfEdgeMesh* mesh);

#endif