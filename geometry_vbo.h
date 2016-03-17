/* cute3d, a simplistic opengl based engine written in C */
/* Copyright (C) 2013 Andreas Raster */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */
#ifndef GEOMETRY_VBO_H
#define GEOMETRY_VBO_H

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "string.h"
#include "math.h"

#include "driver_log.h"
#include "driver_ogl.h"
#include "driver_sdl2.h"
#include "driver_shader.h"

#ifndef MAX_VBO_PHASES
#define MAX_VBO_PHASES 1
#endif

int32_t init_vbo() __attribute__((warn_unused_result));

enum VboScheduling {
    VBO_MANY_BUFFER = 0,
    VBO_BIG_BUFFER
};

// meshes are made up of primitives, like triangles, quads, lines, points
// a primitive is made up of attributes, those could be vertices, normals, texcoords
// each element itself is just a bunch of numbers, like three floats for a vertex, or two ints for a texcoord
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
    //   which may all have different numbers of components, so then these must indicate for example how many
    //   vertices are in a buffer, and to get the actual buffer size, these must be multiplied by
    //   components[buffer]->size
    size_t capacity; // size of the whole buffer
    size_t occupied; // actual space used by meshes

    struct VboScheduler{
        uint32_t phase;
        int32_t dirty[MAX_VBO_PHASES];
        GLsync fence[MAX_VBO_PHASES];
        enum VboScheduling type;
        size_t offset;
    } scheduler;
};

void vbo_create(struct Vbo* p);
void vbo_destroy(struct Vbo* p);

void vbo_print(struct Vbo* vbo);

void vbo_add_buffer(struct Vbo* vbo,
                    uint32_t i,
                    uint32_t component_n,
                    GLenum component_t,
                    GLenum usage);

size_t vbo_alloc(struct Vbo* vbo, size_t n) __attribute__((warn_unused_result));

size_t vbo_available_capacity(struct Vbo* vbo) __attribute__((warn_unused_result));
size_t vbo_available_bytes(struct Vbo* vbo, int32_t i) __attribute__((warn_unused_result));

void* vbo_map(struct Vbo* vbo, int32_t i, size_t offset, size_t length, GLbitfield access);
GLboolean vbo_unmap(struct Vbo* vbo, int32_t i);

void vbo_wait(struct Vbo* vbo);
void vbo_sync(struct Vbo* vbo);

// meshes are made up of primitives
// to construct those primitives a fixed number of attributes is combined together, a triangle for
// example might be made up of three vertices, normals, texcoords
// since these come from the arrays that are stored in vbos, there is an additional type of buffer
// called the index buffer that contains not components, but indices into the vbos
struct VboMesh {
    struct Vbo* vbo;

    size_t offset; // offset in vbo buffers

    // - capacity in vbomesh is occupied in vbo
    // - occupied in vbomesh is the actual used space that has attributes in it
    // - same unit as the ones in struct Vbo
    size_t capacity; // capacity of mesh in vbo
    size_t occupied[MAX_SHADER_ATTRIBUTES]; // information about how many attributes are occupied by this mesh per buffer

    // information about the index type used in the primitives buffer
    struct VboMeshIndex {
        GLenum type; // something GL_UNSIGNED_INT
        uint32_t bytes; // sizeof type
    } index;

    // the primitives (like triangles)
    struct VboMeshPrimitives {
        GLenum type; // something like GL_TRIANGLES
        uint32_t size; // how many attributes per primitive
    } primitives;

    // - this is the buffer that contains the actual indices making up the primitives
    // - I could put this into vbo instead, it is possible since glDrawElements can take an
    // offset into a buffer as last argument, I don't remember why I originally put this
    // in vbomesh instead, but I suspect because it made things somehow easier to implement
    // - as of now, I decided against refactoring this part and putting the indices into
    // vbo because the existing solution should be more flexible for meshes where I have
    // a fixed amount of vertices and then change the indices to get dynamic geometry, that was,
    // I believe, my original intention here
    // - of course, if I would instead stream my data every frame, this solution is not optimal
    // because I would have to map/bind every index buffer for every mesh, instead of just
    // mapping/binding one big index buffer once, as I could do with the attributes buffers
    // in vbo
    // - I just came back to this again thinking about putting the index buffer somehow into
    // the vbo instead of here, the reason I wanted to do this is so that memory management
    // becomes easier, without the index buffer, the meshes can just be treated like local stack
    // variables, they'll deallocate automatically once they go out of scope, with the index
    // buffer allocated on the gpu, I have to explicitly deconstruct every mesh
    // - one problem with this right here: I can not create a local VboMesh and then return
    // a copy, the indices pointer will always be wrong! moving this into Vbo would help
    // - when thinking about implementing color picking, to minimize drawcalls, it would be
    // better if the indices would be in one big buffer instead of multiple smaller ones, then
    // I could just pass a whole vbo (and ibo?) into a picking function and would only need
    // one drawcall, but (!) I would additionally need something like instancing, something
    // to pass in the model matrices for the seperate meshes! otherwise it would not reduce
    // the amount of drawcalls
    // - given the last two points, I want to make this seperate from VboMesh, but, my initial
    // thinking it should be part of Vbo, is not optimal I believe, it should be seperate,
    // I want an Ibo struct in addition to an Vbo struct, which I then point to from VboMesh,
    // that also reflects better the notion that attribute data is 'fixed', whereas indices can
    // be streamed in for every frame, or may be completely replaced
    // - the reason I don't want to put it into the struct Vbo is because I will need to have to
    // implement all the alloc/management functions for the ibo, and I don't want to have to deal
    // with the conflicting names of the vbo functions
    // - when I do this, I could wrap the vbo pointer above together with offset, occupied and
    // capacity in a attributes struct, and then have the same, but with an ibo pointer, in an
    // indices struct
    // - when I do this I also should check that a mesh can be created without indices, because
    // often my simple solids can be render just fine with glDrawArrays, and I should just make
    // sure that everthing works when I just not call vbomesh_append_indices at all
    struct VboMeshIndexBuffer {
        GLuint id; // index buffer
        GLenum usage;
#ifndef EMSCRIPTEN
        // glDrawElementsBaseVertex is not in OpenGL ES 2.0, so I disable this here when I compile
        // with emscripten so that I get errors when I use it elsewhere
        size_t base; // base vertex index
#endif

        // the unit here is indices, not primitives
        size_t capacity; // size of the buffer
        size_t occupied; // space already used
    } _internal_indices[MAX_VBO_PHASES];
    struct VboMeshIndexBuffer* indices;

    // - to prevent z fighting a random small offset value is put here that can be
    // used in a rendering function to offset the mesh by a small amount
    float z_offset;

#ifndef CUTE_BUILD_ES2
    GLuint vao;
#endif
};

void vbomesh_create(struct Vbo* vbo, GLenum primitive_type, GLenum index_type, GLenum usage, struct VboMesh* mesh);
void vbomesh_destroy(struct Vbo* vbo, struct VboMesh* mesh);

void vbomesh_print(struct VboMesh* mesh);

// put a common pattern to check if a mesh is last in vbo in here so I don't wonder about it
// everytime I come back to this code/have to only document it once, here
bool vbomesh_test_last(struct VboMesh* mesh);

// functions to allocate new space in a mesh
size_t vbomesh_alloc_attributes(struct VboMesh* mesh, size_t n) __attribute__((warn_unused_result));
size_t vbomesh_alloc_indices(struct VboMesh* mesh, size_t n) __attribute__((warn_unused_result));

// clearing just resets the occupied counter to 0
void vbomesh_clear_attributes(struct VboMesh* mesh);
void vbomesh_clear_indices(struct VboMesh* mesh);

// append adds new stuff at the end of occupied, allocates new capacity if neccessary
size_t vbomesh_append_attributes(struct VboMesh* mesh, int32_t i, uint32_t components_size, GLenum components_type, size_t n, void* data) __attribute__((warn_unused_result));
size_t vbomesh_append_indices(struct VboMesh* mesh, size_t n, void* data) __attribute__((warn_unused_result));

// mapping whole mesh into host memory, probably untested
void* vbomesh_map(struct VboMesh* mesh, size_t offset, size_t length, GLbitfield access) __attribute__((warn_unused_result));
GLboolean vbomesh_unmap(struct VboMesh* mesh);

#endif

/* struct VboMesh* vbomesh_clone(struct VboMesh* mesh); */

/* void vbomesh_quad(struct mesh* mesh, uint32_t a, uint32_t b, uint32_t c, uint32_t d); */

/* struct mesh* vbomesh_union(struct mesh* a, struct mesh* b); */

/* struct mesh* vbomesh_copy(struct mesh* mesh, struct vbo* to_vbo, uint64_t to_offset); */

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

// a buffer has a SIZE
// space in a buffer is RESERVED for meshes
// a mesh represents a certain SIZE of occupied buffer space
// space is USED by actual geometry data

// buffer: 0+++++++++++++++++++++++++++++++++++++++++1--------------------2
// mesh1 : 3**********4=======5
// mesh2 :                     6***************7=====8
// free  :            9-------10              11--------------------------12

// 0 is just 0, it never changes
// 1 is buffer->occupied, it changes when a new mesh is created,
//   the last meshs size grows, data is appended to the last mesh that
//   is bigger then the remaining space in the last mesh
// 2 is buffer->size, it changes when the buffer grows, the last mesh
//   grows beyond the buffersize, a new mesh is created bigger then
//   the remaining free space in a buffer
// 3 and 6 are mesh1->offset and mesh2->offset, they never change
// 4 and 7 are mesh1->offset + mesh1->used and mesh2->offset + mesh2->used
//   they change when data is added to the mesh
// 5 and 8 are mesh1->offset + mesh1->size and mesh2->offset + mesh2->size,
//   they can only change for the last mesh in the buffer, when data is added
//   that makes the used space grow beyond the meshes size
// range 9-10 is returned by vbomesh_freespace(mesh1), it is equal to
//   mesh1->size - mesh1->used
// range 11-10 is a special case, vbomesh_freespace(mesh2) should return the
//   remaining space in the buffer since mesh2 is the last mesh,
//   finding the last mesh can be done by going through all meshes of the
//   buffer in reverse order (most likely the first test is going to be
//   successful) and checking if mesh->offset + mesh->size equals
//   buffer->occupied
