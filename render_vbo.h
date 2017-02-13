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

#ifndef RENDER_VBO_H
#define RENDER_VBO_H

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "string.h"
#include "math.h"
#include "inttypes.h"

#include "math_camera.h"
#include "math_pivot.h"

#include "geometry_solid.h"
#include "geometry_halfedgemesh.h"

#include "driver_log.h"
#include "driver_ogl.h"
#include "driver_sdl2.h"

#include "render_shader.h"

#ifndef MAX_VBO_PHASES
#define MAX_VBO_PHASES 1
#endif

#ifndef VBO_ALLOC
#define VBO_ALLOC 32786
#endif

#ifndef IBO_ALLOC
#define IBO_ALLOC 32786
#endif

WARN_UNUSED_RESULT int32_t init_vbo();

enum VboScheduling {
    VBO_MANY_BUFFER = 0,
    VBO_BIG_BUFFER
};

// meshes are made up of primitives, like triangles, quads, lines, points
// a primitive is made up of attributes, those could be vertices, normals, texcoords
// each element itself is just a bunch of numbers, like three floats for a vertex, or two ints for a texcoord,
// these are components
//
// vbos are buffers that contain the components
// for example a vertex array might contain floats of which three at a time make up a single vertex
// there are several different types of arrays that are all managed in struct Vbo
//
// now the attributes are just logical units, the buffers that contain the components also
// contain the attributes, and just like the components make up attributes, the attributes make up
// primitives, e.g. three float components make one vertex element, and three vertex attributes
// make one triangle primitive
struct Vbo {
    struct VboBuffer {
        GLuint id;
        GLenum usage;
    } _internal_buffer[MAX_VBO_PHASES][MAX_SHADER_ATTRIBUTES];
    struct VboBuffer* buffer;

    struct VboComponents {
        uint32_t size; // the number of components per element (eg a vertex3 element has three components)
        GLenum type; // the gl type of the individual components (probably GL_float)
        uint32_t bytes; // size of a single component (sizeof GL_float)
    } components[MAX_SHADER_ATTRIBUTES];

    // - the units of these are in attributes, capacity is universally used for the different attribute buffers,
    // which may all have different numbers of components, so then these must indicate for example how many
    // vertices are in a buffer, and to get the actual buffer size in bytes these must be multiplied by
    // components[buffer]->size * components[buffer]->bytes
    size_t capacity; // size of the whole buffer
    size_t occupied; // actual space used by meshes

    // - this is not used yet, I wanted to have a system at some point in the future where I could stream attributes
    // into vbos constantly, and rotate between multiple vbos so that rendering and uploading could all happen simultanously,
    // just like it is described in my opengl insights book, this is just an idea on how to do that
    /* struct VboScheduler{ */
    /*     uint32_t phase; */
    /*     int32_t dirty[MAX_VBO_PHASES]; */
    /*     GLsync fence[MAX_VBO_PHASES]; */
    /*     enum VboScheduling type; */
    /*     size_t offset; */
    /* } scheduler; */
    /* void vbo_wait(struct Vbo* vbo); */
    /* void vbo_sync(struct Vbo* vbo); */
};

void vbo_create(struct Vbo* p);

void vbo_print(struct Vbo* vbo);

void vbo_add_buffer(struct Vbo* vbo,
                    uint32_t i,
                    uint32_t component_n,
                    GLenum component_t,
                    GLenum usage);

WARN_UNUSED_RESULT size_t vbo_alloc(struct Vbo* vbo, size_t n);

WARN_UNUSED_RESULT size_t vbo_available_capacity(struct Vbo* vbo);

void* vbo_map(struct Vbo* vbo, int32_t i, size_t offset, size_t length, GLbitfield access);
GLboolean vbo_unmap(struct Vbo* vbo, int32_t i);

struct Ibo {
    struct IboBuffer {
        GLuint id; // index buffer
        GLenum usage;
    } _internal_buffer[MAX_VBO_PHASES];
    struct IboBuffer* buffer;

    // type information about individual index values
    struct IboIndex {
        GLenum type; // something GL_UNSIGNED_INT
        uint32_t bytes; // sizeof type
    } index;

    // the primitives (like triangles)
    struct IboPrimitives {
        GLenum type; // something like GL_TRIANGLES
        uint32_t size; // how many attributes per primitive
    } primitives;

    // the unit here is indices, not primitives
    size_t capacity; // size of the buffer
    size_t occupied; // space already used

#ifndef CUTE_BUILD_ES2
    // glDrawElementsBaseVertex is not in OpenGL es2
    size_t base; // base vertex index
#endif
};

void ibo_create(GLenum primitive_type, GLenum index_type, GLenum usage, struct Ibo* ibo);

void ibo_print(struct Ibo* ibo);

WARN_UNUSED_RESULT size_t ibo_alloc(struct Ibo* ibo, size_t n);

WARN_UNUSED_RESULT size_t ibo_available_capacity(struct Ibo* ibo);

void* ibo_map(struct Ibo* ibo, size_t offset, size_t length, GLbitfield access);
GLboolean ibo_unmap(struct Ibo* ibo);

// meshes are made up of primitives
// to construct those primitives a fixed number of attributes is combined together, a triangle for
// example might be made up of three vertices, normals, texcoords
// since these come from the arrays that are stored in vbos, there is an additional type of buffer
// called the index buffer that contains not components, but indices into the vbos
struct VboMesh {
    struct Vbo* vbo;
    struct VboMeshAttributes {
        size_t offset; // offset in vbo buffers

        // - capacity in vbomesh is occupied in vbo
        // - occupied in vbomesh is the actual used space that has attributes in it
        // - same unit as the ones in struct Vbo
        // - there is only one capacity, but one occupied for each attribute, because when
        // we allocate, we always allocate all attributes, then capacity represents all
        // space available for all attributes, but occupied needs to be specific for each
        // attribute because we append each one in a seperate vbo_mesh_append_attributes
        // call and therefore need to keep track of how much is occupied seperately
        size_t capacity; // capacity of mesh in vbo
        size_t occupied[MAX_SHADER_ATTRIBUTES]; // information about how many attributes are occupied by this mesh per buffer
    } attributes;

    struct Ibo* ibo;
    struct VboMeshIndices {
        size_t offset;
        size_t capacity;
        size_t occupied;
    } indices;

    // - to prevent z fighting a random small offset value is put here that can be
    // used in a rendering function to offset the mesh by a small amount
    float z_offset;

#ifndef CUTE_BUILD_ES2
    GLuint vao;
#endif
};

void vbo_mesh_create(struct Vbo* vbo, struct Ibo* ibo, struct VboMesh* mesh);

void vbo_mesh_print(struct VboMesh* mesh);

// put a common pattern to check if a mesh is last in vbo in here so I don't wonder about it
// everytime I come back to this code/have to only document it once, here
bool vbo_mesh_test_last_attributes(struct VboMesh* mesh);
bool vbo_mesh_test_last_indices(struct VboMesh* mesh);

// functions to allocate new space in a mesh
WARN_UNUSED_RESULT size_t vbo_mesh_alloc_attributes(struct VboMesh* mesh, size_t n);
WARN_UNUSED_RESULT size_t vbo_mesh_alloc_indices(struct VboMesh* mesh, size_t n);

// clearing just resets the occupied counter to 0
void vbo_mesh_clear_attributes(struct VboMesh* mesh);
void vbo_mesh_clear_indices(struct VboMesh* mesh);

// append adds new stuff at the end of occupied, allocates new capacity if neccessary
WARN_UNUSED_RESULT size_t vbo_mesh_append_attributes(struct VboMesh* mesh, int32_t i, uint32_t components_size, GLenum components_type, size_t n, void* data);
WARN_UNUSED_RESULT size_t vbo_mesh_append_indices(struct VboMesh* mesh, size_t n, void* data);

void vbo_mesh_create_from_solid(const struct Solid* solid, struct Vbo* const vbo, struct Ibo* const ibo, struct VboMesh* mesh);

// - this used to be halfedgemesh_flush function which I rewrote and put here because this originally
// used a struct Solid as output, which was a very unfitting choise, so this now fills a vbomesh instead
void vbo_mesh_create_from_halfedgemesh(const struct HalfEdgeMesh* halfedgemesh, struct Vbo* const vbo, struct Ibo* const ibo, struct VboMesh* mesh);

void vbo_mesh_render(struct VboMesh* mesh, struct Shader* shader, const struct Camera* camera, const Mat model_matrix);

#endif

// mesh operations
// create:
//   initialize mesh struct with default values, does not allocate any space
// append:
//   append geometry to cache, takes array type as argument
//   1. check if mesh is the last or only mesh in buffer, if it isn't
//      return without appending anything
//   2. check if buffer size is too small to fit more values
//   3. resize buffer if neccessary
//   4. append values, increase buffer_used counter, adjust mesh subrange
// freespace:
//   return number of bytes that is left in the buffer after the mesh, this can
//   be used to test if more geometry can be appended to a mesh
// duplicate:
//  copies mesh geometry to end of buffer and returns pointer to new mesh that can
//  be used to append more geometry
// retain:
// triangle/quad:
//   creates a triangle/quad element
// destroy:
// union:

// OUTDATED (?) STUFF BELOW

// a buffer has a CAPACITY
// space in a buffer is OCCUPIED by meshes
// a mesh represents a certain chunk of buffer space, which itself has
// a CAPACITY of which space is OCCUPIED by the actual geometry

// buffer: 0+++++++++++++++++++++++++++++++++++++++++1--------------------2
// mesh1 : 3**********4=======5
// mesh2 :                     6***************7=====8
// free  :            9-------10              11--------------------------12

// 0 is just 0, it never changes
// 1 is buffer->occupied, it changes when a new mesh is created,
//   the last meshs size grows, data is appended to the last mesh that
//   is bigger then the remaining space in the last mesh
// 2 is buffer->capacity, it changes when the buffer grows
// 3 and 6 are mesh1->offset and mesh2->offset, they never change
// 4 and 7 are mesh1->offset + mesh1->occupied and mesh2->offset + mesh2->occupied
//   they change when data is added to the mesh
// 5 and 8 are mesh1->offset + mesh1->capacity and mesh2->offset + mesh2->capacity,
//   they can only change for the last mesh in the buffer, when data is added
//   that makes the used space grow beyond the meshes size (it may at some point be
//   possible to grow a mesh beyond its capacity even when it is not the last mesh of
//   a buffer by implementing some way that copies the mesh from its position to the
//   last position, invaldiates the old one and appends data to the new one)
// range 9-10 is the free space in mesh1, it is computed with mesh1->capacity -
//   mesh1->occupied
// range 11-12 is a special case, it is actually the free space range of mesh2
//   (mesh2->capacity - mesh2->occupied) together with the free space range of the whole
//   buffer, which can be computed with the function vbo_available_capacity(vbo), so:
//   (mesh2->capacity - mesh2->occupied) + vbo_available_capacity(vbo)
