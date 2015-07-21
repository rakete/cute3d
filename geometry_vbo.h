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

#include "render_ogl.h"

#ifndef NUM_PHASES
#define NUM_PHASES 1
#endif

int init_vbo();

GLsizei buffer_resize(GLuint* buffer, GLsizei old_nbytes, GLsizei new_nbytes);

GLsizei sizeof_type(GLenum type);

GLsizei sizeof_primitive(GLenum primitive);

enum buffer_array_type {
    VERTEX_ARRAY = 0,
    NORMAL_ARRAY,
    COLOR_ARRAY,
    TEXCOORD_ARRAY,
    NUM_BUFFERS
};

enum scheduling {
    MANY_BUFFER = 0,
    BIG_BUFFER
};

// meshes are made up of primitives, like triangles, quads, lines, points
// a primitive is made up of elements, those could be vertices, normals, texcoords
// each element itself is just a bunch of numbers, like three floats for a vertex, or two ints for a texcoord
//
// vbos are buffers that contain the components
// for example a vertex array might contain floats of which three at a time make up a single vertex
// there are several different types of arrays that are all managed in struct Vbo
//
// now the elements are just logical units, the buffers that contain the components also
// contain the elements, and just like the components make up elements, the elements make up
// primitives, e.g. three float components make one vertex element, and three vertex elements
// make one triangle primitive
struct Vbo {
    struct Buffer {
        GLuint id;
        GLenum usage;
    } _internal_buffer[NUM_PHASES][NUM_BUFFERS];
    struct Buffer* buffer;

    struct Components {
        GLint size; // the number of components per element (eg a vertex3 element has three components)
        GLenum type; // the gl type of the individual components (probably GL_float)
        GLint bytes; // size of a single component (sizeof GL_float)
    } _internal_components[NUM_PHASES][NUM_BUFFERS];
    struct Components* components;

    GLint capacity; // size of the whole buffer
    GLint reserved; // actual space used by meshes

    struct {
        unsigned int phase;
        int dirty[NUM_PHASES];
        GLsync fence[NUM_PHASES];
        enum scheduling type;
        GLint offset;
    } scheduler;
};

int vbo_create(struct Vbo* p);
int vbo_destroy(struct Vbo* p);

void vbo_print(struct Vbo* vbo);

void vbo_add_buffer(struct Vbo* vbo,
                    int i,
                    GLint component_n,
                    GLenum component_t,
                    GLenum usage);

GLint vbo_alloc(struct Vbo* vbo, GLint n);

GLint vbo_free_elements(struct Vbo* vbo);
GLint vbo_free_bytes(struct Vbo* vbo, int i);

void vbo_fill_value(struct Vbo* vbo, int i, GLint offset_n, GLint size_n, float value);

void* vbo_map(struct Vbo* vbo, int i, GLint offset, GLint length, GLbitfield access);
GLboolean vbo_unmap(struct Vbo* vbo, int i);

void vbo_wait(struct Vbo* vbo);
void vbo_sync(struct Vbo* vbo);

// meshes are made up of primitives
// to construct those primitives a fixed number of elements is combined together, a triangle for
// example might be made up of three vertices, normals, texcoords
// since these come from the arrays that are stored in vbos, there is an additional type of buffer
// called the index buffer that contains not components, but indices into the vbos
struct VboMesh {
    struct Vbo* vbo;

    GLint offset; // offset in vbo buffers
    GLint size; // space used by mesh in vbo

    // information about how many elements are used by this mesh per buffer
    GLint uses[NUM_BUFFERS];

    // information about the index type used in the primitives buffer
    struct {
        GLenum type; // something GL_UNSIGNED_INT
        GLsizei bytes; // sizeof type
    } index;

    // the primitives (like triangles)
    struct {
        GLenum type; // something like GL_TRIANGLES
        GLint size; // how many elements per primitive

        // this is the buffer that contains the actual indices making up the primitives
        struct IndexBuffer {
            GLuint id; // index buffer
            GLenum usage;
            GLint size; // size of the buffer
            GLint used; // space already used
        } _internal_buffer[NUM_PHASES];
        struct IndexBuffer* buffer;
    } primitives;

    int garbage;
};

int vbomesh_create(struct Vbo* vbo, GLenum primitive_type, GLenum index_type, GLenum usage, struct VboMesh* p);
int vbomesh_destroy(struct Vbo* vbo, struct VboMesh* p);

void vbomesh_print(struct VboMesh* mesh);

GLint vbomesh_alloc(struct VboMesh* mesh, GLint n);
GLint vbomesh_alloc_vbo(struct VboMesh* mesh, GLint n);
GLint vbomesh_alloc_primitives(struct VboMesh* mesh, GLint n);

void vbomesh_append(struct VboMesh* mesh, int i, void* data, GLint n);
void vbomesh_append_generic(struct VboMesh* mesh, int i, void* data, GLint n, GLint components_size, GLenum components_type);

void vbomesh_clear_vbo(struct VboMesh* mesh);
void vbomesh_clear_primitives(struct VboMesh* mesh);

void* vbomesh_map(struct VboMesh* mesh, GLint offset, GLint length, GLbitfield access);
GLboolean vbomesh_unmap(struct VboMesh* mesh);

void vbomesh_triangle(struct VboMesh* mesh, GLuint a, GLuint b, GLuint c);
void vbomesh_primitives(struct VboMesh* mesh, void* data, GLint n);

struct VboMesh* vbomesh_clone(struct VboMesh* mesh);

#endif

/* void vbomesh_quad(struct mesh* mesh, GLuint a, GLuint b, GLuint c, GLuint d); */

/* void vbomesh_destroy(struct mesh* mesh); */

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
// a mesh represents a certain SIZE of reserved buffer space
// space is USED by actual geometry data

// buffer: 0+++++++++++++++++++++++++++++++++++++++++1--------------------2
// mesh1 : 3**********4=======5
// mesh2 :                     6***************7=====8
// free  :            9-------10              11--------------------------12

// 0 is just 0, it never changes
// 1 is buffer->reserved, it changes when a new mesh is created,
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
//   buffer->reserved