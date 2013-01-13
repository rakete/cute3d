#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

#include "GL/glew.h"
#include "GL/gl.h"

#ifndef NUM_BUFFERS
#define NUM_BUFFERS 3
#endif

#ifndef NUM_MESHES
#define NUM_MESHES 1024
#endif

int init_geometry();

GLsizei buffer_resize(GLuint* buffer, GLsizei old_nbytes, GLsizei new_nbytes);

GLsizei sizeof_type(GLenum type);

GLsizei sizeof_primitive(GLenum primitive);

enum buffer_array_type {
    vertex_array = 0,
    normal_array,
    color_array
};

enum element {
    vertex_element = 1,
    line_element,
    triangle_element,
    quad_element
};

struct vbo {
    struct {
        GLuint id;
        GLenum usage;
    } buffer[NUM_BUFFERS];

    struct {
        uint32_t num; // the number of components per element
        GLenum type; // the gl type of the individual components
        uint32_t bytes; // size of a single component
    } components[NUM_BUFFERS];

    uint32_t capacity; // size of the whole buffer
    uint32_t reserved; // actual used space by meshes
    uint32_t alloc; // how much should be allocated per once we grow bigger then size
};

void vbo_create(uint32_t size_n,
                uint32_t alloc_n,
                struct vbo* p);

void dump_vbo(struct vbo* vbo, FILE* f);

void vbo_add_buffer(struct vbo* vbo,
                    int i,
                    uint32_t component_n,
                    GLenum component_t,
                    GLenum usage);

uint32_t vbo_alloc(struct vbo* vbo, uint32_t n);

uint32_t vbo_free_elements(struct vbo* vbo);
uint32_t vbo_free_bytes(struct vbo* vbo, int i);

void vbo_bind(struct vbo* vbo, int i, GLenum bind_type);

void vbo_fill_value(struct vbo* vbo, int i, uint32_t offset_n, uint32_t size_n, float value);

struct mesh {
    struct vbo* vbo;

    uint32_t offset; // offset in vbo buffers
    uint32_t size; // space used by mesh in vbo

    struct {
        uint32_t used;
    } buffer[NUM_BUFFERS];
    
    struct {
        GLenum primitive;
        GLenum type;
        uint32_t num;
        uint32_t bytes;
    } indices;

    struct {
        GLuint buffer; // indices buffer
        uint32_t size; // size of the buffer
        uint32_t used; // space already used
        uint32_t alloc; // how much to allocate additionally if we need to resize
    } elements;

    uint32_t garbage;
};

void mesh_create(struct vbo* vbo, GLenum primitive_type, GLenum indices_type, struct mesh* p);

void dump_mesh(struct mesh* mesh, FILE* f);

void mesh_patches(struct mesh* mesh, uint32_t patches_size);

uint32_t mesh_alloc(struct mesh* mesh, uint32_t n);

uint32_t mesh_free_bytes(struct mesh* mesh, int i);
uint32_t mesh_free_elements(struct mesh* mesh, int i);

void mesh_append(struct mesh* mesh, int i, void* data, uint32_t n);

void mesh_triangle(struct mesh* mesh, GLuint a, GLuint b, GLuint c);

struct mesh* mesh_clone(struct mesh* mesh);

/* void mesh_quad(struct mesh* mesh, GLuint a, GLuint b, GLuint c, GLuint d); */

/* void mesh_destroy(struct mesh* mesh); */

/* struct mesh* mesh_union(struct mesh* a, struct mesh* b); */

/* struct mesh* mesh_copy(struct mesh* mesh, struct vbo* to_vbo, uint32_t to_offset); */

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
// clone:
//  copies mesh geometry to end of buffer and returns pointer to new mesh that can
//  be used to append more geometry
// retain:
// triangle/quad:
//   creates a triangle/quad element
// destroy:
// union:

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
// range 9-10 is returned by mesh_freespace(mesh1), it is equal to
//   mesh1->size - mesh1->used
// range 11-10 is a special case, mesh_freespace(mesh2) should return the
//   remaining space in the buffer since mesh2 is the last mesh,
//   finding the last mesh can be done by going through all meshes of the
//   buffer in reverse order (most likely the first test is going to be
//   successful) and checking if mesh->offset + mesh->size equals
//   buffer->reserved
