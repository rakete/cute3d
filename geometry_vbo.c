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

#include "geometry_vbo.h"

int init_vbo() {
    int ret = 1;

    glewInit();
    if( ! glewGetExtension("GL_ARB_copy_buffer") &&
        ! glewGetExtension("GL_EXT_copy_buffer") )
    {
        printf("ERROR: copy_buffer extension not found!\n");
        ret = 0;
    }

    if( ! glewGetExtension("GL_ARB_vertex_array_object") &&
        ! glewGetExtension("GL_EXT_vertex_array_object") )
    {
        printf("ERROR: vertex_array_object extension not found!\n");
        ret = 0;
    }

    /* if( ! glewGetExtension("GL_ARB_buffer_storage" ) ) { */
    /*     printf("ERROR: buffer_storage extension not found!\n"); */
    /*     ret = 0; */
    /* } */


    return ret;
}

// a helper function to resize a vbo, it takes a buffer argument and the old size
// of the buffer and the new size it should be resized to, returns either the amount
// of bytes that were added to the buffer size or 0 if nothing was resized
int buffer_resize(GLuint* buffer, int old_bytes, int new_bytes) {
    assert( buffer != NULL );

    unsigned int new_buffer;
    unsigned int old_buffer = *buffer;

    if( new_bytes > old_bytes ) {
        ogl_debug( glGenBuffers(1, &new_buffer);
                   glBindBuffer(GL_COPY_WRITE_BUFFER, new_buffer);
                   glBufferData(GL_COPY_WRITE_BUFFER, new_bytes, NULL, GL_STATIC_COPY); );

        if( old_bytes > 0 && old_buffer > 0 ) {
            ogl_debug( glBindBuffer(GL_COPY_READ_BUFFER, old_buffer);
                       glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, old_bytes); );
        }

        if( old_buffer ) {
            ogl_debug( glDeleteBuffers(1, &old_buffer) );
        }

        *buffer = new_buffer;

        if( old_bytes > 0 && old_buffer ) {
            ogl_debug( glBindBuffer(GL_COPY_READ_BUFFER, 0) );
        }
        ogl_debug( glBindBuffer(GL_COPY_WRITE_BUFFER, 0) );

        return new_bytes - old_bytes;
    }

    return 0;
}

GLsizei sizeof_type(GLenum type) {
    switch(type) {
        case GL_FLOAT: return sizeof(GLfloat);
        case GL_INT: return sizeof(GLint);
        case GL_UNSIGNED_INT: return sizeof(GLuint);
        case GL_SHORT: return sizeof(GLshort);
        case GL_UNSIGNED_SHORT: return sizeof(GLushort);
        case GL_BYTE: return sizeof(GLbyte);
        case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
        default: return 0;
    }
    return 0;
}

GLsizei sizeof_primitive(GLenum primitive) {
    switch(primitive) {
        case GL_POINTS: return 1;
        case GL_LINES: return 2;
        case GL_TRIANGLES: return 3;
        case GL_PATCHES: return 4;
        default: return 0;
    }
    return 0;
}

int vbo_create(struct Vbo* p) {
    for( int i = 0; i < NUM_VBO_PHASES; i++ ) {
        for( int j = 0; j < NUM_VBO_BUFFERS; j++ ) {
            p->_internal_buffer[i][j].id = 0;
            p->_internal_buffer[i][j].usage = GL_STATIC_DRAW;
        }
    }
    p->buffer = p->_internal_buffer[0];

    for( int i = 0; i < NUM_VBO_BUFFERS; i++ ) {
        p->_internal_components[i].size = 0;
        p->_internal_components[i].type = 0;
        p->_internal_components[i].bytes = 0;
    }
    p->components = p->_internal_components;

    p->capacity = 0;
    p->reserved = 0;

    for( int i = 0; i < NUM_VBO_PHASES; i++ ) {
        p->scheduler.fence[i] = 0;
    }
    p->scheduler.phase = 0;
    p->scheduler.type = VBO_MANY_BUFFER;
    p->scheduler.offset = 0;

    return 1;
}

void vbo_print(struct Vbo* vbo) {
    printf("vbo->capacity: %d\n", vbo->capacity);
    printf("vbo->reserved: %d\n", vbo->reserved);
}

void vbo_add_buffer(struct Vbo* vbo,
                    int i,
                    int component_n,
                    GLenum component_t,
                    GLenum usage)
{
    if( vbo && i < NUM_VBO_BUFFERS ) {
        ogl_debug( glGenBuffers(1, &vbo->buffer[i].id) );

        vbo->buffer[i].usage = usage;

        vbo->components[i].size = component_n;
        vbo->components[i].type = component_t;
        vbo->components[i].bytes = sizeof_type(component_t);

        int nbytes = vbo->capacity * component_n * sizeof_type(component_t);
        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, vbo->buffer[i].id);
                   glBufferData(GL_ARRAY_BUFFER, nbytes, NULL, usage);
                   glBindBuffer(GL_ARRAY_BUFFER, 0); );

    }
}

int vbo_alloc(struct Vbo* vbo, int n) {
    assert( n > 0 );

    if( vbo ) {
        // I set this to -1 so I can check in the for loop for != 0, so that I can decide afterwards if any
        // buffer_resize failed (and returned 0), or if there were no existing vbos (which means resized_bytes
        // is still -1 after the loop)
        int resized_bytes = -1;

        for( int i = 0; i < NUM_VBO_BUFFERS && resized_bytes != 0; i++ ) {
            if( vbo->buffer[i].id ) {
                int new_bytes = (vbo->capacity + n) * vbo->components[i].size * vbo->components[i].bytes;
                int old_bytes = vbo->capacity * vbo->components[i].size * vbo->components[i].bytes;

                resized_bytes = buffer_resize(&vbo->buffer[i].id, old_bytes, new_bytes);
            }
        }

        // - only increase capacity if all vbos were resized
        // - resized_bytes is different per buffer, so just return n instead
        if( resized_bytes > 0 ) {
            vbo->capacity += n;
            return n;
        } else {
            assert( resized_bytes == 0 );
        }
    }

    return 0;
}

int vbo_available_capacity(struct Vbo* vbo) {
    int freespace = 0;
    if( vbo ) {
        freespace = vbo->capacity - vbo->reserved;
    }
    return freespace;
}

int vbo_available_bytes(struct Vbo* vbo, int i) {
    int freespace = 0;
    if( vbo && vbo->buffer[i].id ) {
        freespace = vbo_available_capacity(vbo) * vbo->components[i].size * vbo->components[i].bytes;
    }
    return freespace;
}

void vbo_fill_value(struct Vbo* vbo, int i, int offset_n, int size_n, float value) {
    if( vbo && vbo->buffer[i].id && offset_n < vbo->capacity && size_n <= vbo->capacity ) {
        void* array = malloc( sizeof_type(vbo->components[i].type) * size_n );
        int array_offset = offset_n * vbo->components[i].size;
        int array_size = size_n * vbo->components[i].size;

        switch(vbo->components[i].type) {
            case GL_FLOAT: {
                memset(array, value, array_size);
                break;
            }
            case GL_INT: {
                memset(array, (GLint)ceil(value), array_size);
                break;
            }
            case GL_UNSIGNED_INT: {
                if( value < 0.0 ) {
                    memset(array, 0.0, array_size);
                } else {
                    memset(array, (GLuint)ceil(value), array_size);
                }
                break;
            }
            case GL_SHORT: {
                break;
            }
            case GL_UNSIGNED_SHORT: {
                break;
            }
            case GL_BYTE: {
                break;
            }
            case GL_UNSIGNED_BYTE: {
                break;
            }
        }

        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, vbo->buffer[i].id);
                   glBufferSubData(GL_ARRAY_BUFFER, array_offset, array_size, array);
                   glBindBuffer(GL_ARRAY_BUFFER, 0); );
    }
}

void* vbo_map(struct Vbo* vbo, int i, int offset, int length, GLbitfield access) {
    if( vbo && vbo->buffer[i].id && offset < vbo->capacity ) {
        if( offset + length > vbo->capacity ) {
            vbo_alloc(vbo, offset + length - vbo->capacity);
        }

        if( offset + length <= vbo->capacity ) {
            int offset_bytes = offset * vbo->components[i].size * vbo->components[i].bytes;
            int length_bytes = length * vbo->components[i].size * vbo->components[i].bytes;
            if( length <= offset || length > vbo->capacity ) {
                length_bytes = vbo->capacity * vbo->components[i].size * vbo->components[i].bytes;
            }

            void* pointer = NULL;
            ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, vbo->buffer[i].id);
                       pointer = glMapBufferRange(GL_ARRAY_BUFFER, offset_bytes, length_bytes, access);
                       glBindBuffer(GL_ARRAY_BUFFER, 0); );

            return pointer;
        }
    }

    return NULL;
}

GLboolean vbo_unmap(struct Vbo* vbo, int i) {
    if( vbo && vbo->buffer[i].id ) {
        GLboolean result = 0;
        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, vbo->buffer[i].id);
                   result = glUnmapBuffer(GL_ARRAY_BUFFER);
                   glBindBuffer(GL_ARRAY_BUFFER, 0); );

        return result;
    }

    return 0;
}


int vbomesh_create(struct Vbo* vbo, GLenum primitive_type, GLenum index_type, GLenum usage, struct VboMesh* p) {
    if( vbo ) {
        p->vbo = vbo;

        p->offset = vbo->reserved;
        p->capacity = 0;

        for( int i = 0; i < NUM_VBO_BUFFERS; i++ ) {
            p->reserved[i] = 0;
        }

        p->primitives.type = primitive_type;
        p->primitives.size = sizeof_primitive(primitive_type);

        p->index.type = index_type;
        p->index.bytes = sizeof_type(index_type);

        for( int i = 0; i < NUM_VBO_PHASES; i++ ) {
            p->primitives._internal_buffer[i].id = 0;
            p->primitives._internal_buffer[i].usage = usage;
            p->primitives._internal_buffer[i].capacity = 0;
            p->primitives._internal_buffer[i].reserved = 0;
        }
        p->primitives.buffer = &p->primitives._internal_buffer[0];

        ogl_debug( glGenBuffers(1, &p->primitives.buffer->id) );

        return 1;
    } else {
        return 0;
    }
}

void vbomesh_print(struct VboMesh* mesh) {
    vbo_print(mesh->vbo);

    printf("\n");

    printf("mesh->offset: %d\n", mesh->offset);
    printf("mesh->size: %d\n", mesh->capacity);

    printf("mesh->primitives.type: %d\n", mesh->primitives.type);
    printf("mesh->primitives.size: %d\n", mesh->primitives.size);

    printf("mesh->index.type: %d\n", mesh->index.type);
    printf("mesh->index.bytes: %d\n", mesh->index.bytes);
    printf("mesh->primitives.buffer->size: %d\n", mesh->primitives.buffer->capacity);
    printf("mesh->primitives.buffer->used: %d\n", mesh->primitives.buffer->reserved);

    printf("\n");

    for( int i = 0; i < NUM_VBO_PHASES; i++ ) {
        for( int j = 0; j < NUM_VBO_BUFFERS-1; j++ ) {
            printf("mesh->uses[%d]: %d\n", j, mesh->reserved[j]);
            printf("mesh->vbo->buffer[%d][%d]:\n", i, j);
            switch(mesh->vbo->components[j].type) {
                case GL_FLOAT: {
                    GLfloat* array = (GLfloat*)vbo_map(mesh->vbo, j, mesh->offset, mesh->capacity, GL_MAP_READ_BIT);
                    if( array ) {
                        for( int k = 0; k < mesh->capacity; k++ ) {
                            GLfloat a = array[k*mesh->vbo->components[j].size+0];
                            GLfloat b = array[k*mesh->vbo->components[j].size+1];
                            GLfloat c = array[k*mesh->vbo->components[j].size+2];
                            printf("[%f %f %f]", a, b, c);
                            if( k == mesh->capacity - 1 ) {
                                printf("\n");
                            } else {
                                printf(", ");
                            }
                        }
                    } else {
                        printf("NULL\n");
                    }
                    vbo_unmap(mesh->vbo, j);
                    break;
                }
                case GL_INT: {
                    printf("ERROR: GL_INT not implemented in vbomesh_print\n");
                    break;
                }
            }
        }
    }
}

int vbomesh_alloc_attributes(struct VboMesh* mesh, int n) {
    assert( n > 0 );

    if( mesh && mesh->offset + mesh->capacity == mesh->vbo->reserved ) {
        int resized_n = vbo_alloc(mesh->vbo, n);

        if( resized_n > 0 ) {
            mesh->vbo->reserved += n;
            mesh->capacity += n;
        }
        return resized_n;
    }

    return 0;
}

int vbomesh_alloc_indices(struct VboMesh* mesh, int n) {
    assert( n > 0 );

    if( mesh && mesh->primitives.buffer->id ) {
        int size_bytes = mesh->primitives.buffer->capacity * mesh->index.bytes;
        int alloc_bytes = n * mesh->index.bytes;
        int resized_bytes = buffer_resize(&mesh->primitives.buffer->id, size_bytes, size_bytes + alloc_bytes);

        // - we could return resized_bytes, but all other alloc functions return the number
        //   of elements allocated, so we just do the same here
        if( resized_bytes == alloc_bytes ) {
            mesh->primitives.buffer->capacity += n;
            return n;
        } else {
            assert( resized_bytes == 0 );
        }
    }

    return 0;
}

void vbomesh_clear_attributes(struct VboMesh* mesh) {
    for( int i = 0; i < NUM_VBO_BUFFERS; i++ ) {
        mesh->reserved[i] = 0;
    }
}

void vbomesh_clear_primitives(struct VboMesh* mesh) {
    for( int i = 0; i < NUM_VBO_PHASES; i++ ) {
        mesh->primitives._internal_buffer[i].reserved = 0;
    }
}

void vbomesh_append_buffer_generic(struct VboMesh* mesh, int i, void* data, int n, int components_size, GLenum components_type) {
    if( mesh && mesh->vbo->buffer[i].id ) {
        // only these depend on given size params => generic data append
        int attrib_bytes = components_size * sizeof_type(components_type);
        int append_bytes = n * attrib_bytes;

        // stuff that relies on vbo size values
        int capacity_bytes = mesh->capacity * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
        int reserved_bytes = mesh->reserved[i] * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
        int offset_bytes = (mesh->offset + mesh->reserved[i]) * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;

        if( reserved_bytes + append_bytes <= capacity_bytes ) {
            ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
                       glBufferSubData(GL_ARRAY_BUFFER, offset_bytes, append_bytes, data);
                       glBindBuffer(GL_ARRAY_BUFFER, 0); );


            mesh->reserved[i] += n;
        } else if( mesh->offset + mesh->capacity == mesh->vbo->reserved &&
                   mesh->vbo->components[i].size == components_size &&
                   mesh->vbo->components[i].type == components_type )
        {
            // this does not work genericly, so we just do not allocate anything at all,
            // if num and type do not fit the stored values in vbo
            if( vbo_available_capacity(mesh->vbo) < n ) {
                vbomesh_alloc_attributes(mesh,n);
            }

            ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
                       glBufferSubData(GL_ARRAY_BUFFER, offset_bytes, append_bytes, data);
                       glBindBuffer(GL_ARRAY_BUFFER, 0); );

            mesh->reserved[i] += n;
        }
    }
}

void vbomesh_append_attributes(struct VboMesh* mesh, int i, void* data, int n) {
    vbomesh_append_buffer_generic(mesh, i, data, n, mesh->vbo->components[i].size, mesh->vbo->components[i].type);
}

void vbomesh_append_line(struct VboMesh* mesh, unsigned int a, unsigned int b) {
    if( mesh && mesh->primitives.buffer->id && mesh->primitives.size == 2 ) {
        void* data = malloc(2 * mesh->index.bytes);

        if( mesh->index.type == GL_UNSIGNED_INT ) {
            ((GLuint*)data)[0] = a;
            ((GLuint*)data)[1] = b;
        } else if( mesh->index.type == GL_UNSIGNED_SHORT ) {
            ((GLushort*)data)[0] = (GLushort)a;
            ((GLushort*)data)[1] = (GLushort)b;
        } else {
            printf("ERROR: this mesh->index.type not implemented in vbomesh_line\n");
        }

        if( mesh->primitives.buffer->reserved + 2 > mesh->primitives.buffer->capacity ) {
            int alloc = mesh->primitives.buffer->reserved - mesh->primitives.buffer->capacity + 2;
            vbomesh_alloc_indices(mesh, alloc);
        }

        int line_bytes = 2 * mesh->index.bytes;
        int offset_bytes = mesh->primitives.buffer->reserved * mesh->index.bytes;
        if( mesh->primitives.buffer->reserved + 2 <= mesh->primitives.buffer->capacity ) {
            ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->primitives.buffer->id);
                       glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset_bytes, line_bytes, data);
                       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); );

            mesh->primitives.buffer->reserved += 2;
        }

        free(data);
    }
}

void vbomesh_append_triangle(struct VboMesh* mesh, unsigned int a, unsigned int b, unsigned int c) {
    if( mesh && mesh->primitives.buffer->id && mesh->primitives.size == 3 ) {
        void* data = malloc(3 * mesh->index.bytes);

        if( mesh->index.type == GL_UNSIGNED_INT ) {
            ((GLuint*)data)[0] = a;
            ((GLuint*)data)[1] = b;
            ((GLuint*)data)[2] = c;
        } else if( mesh->index.type == GL_UNSIGNED_SHORT ) {
            ((GLushort*)data)[0] = (GLushort)a;
            ((GLushort*)data)[1] = (GLushort)b;
            ((GLushort*)data)[2] = (GLushort)c;
        } else {
            printf("ERROR: this mesh->index.type not implemented in vbomesh_triangle\n");
        }

        if( mesh->primitives.buffer->reserved + 3 > mesh->primitives.buffer->capacity ) {
            int alloc = mesh->primitives.buffer->reserved - mesh->primitives.buffer->capacity + 3;
            vbomesh_alloc_indices(mesh, alloc);
        }

        int triangle_bytes = 3 * mesh->index.bytes;
        int offset_bytes = mesh->primitives.buffer->reserved * mesh->index.bytes;
        if( mesh->primitives.buffer->reserved + 3 <= mesh->primitives.buffer->capacity ) {
            ogl_debug( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->primitives.buffer->id);
                       glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset_bytes, triangle_bytes, data);
                       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); );

            mesh->primitives.buffer->reserved += 3;
        }

        free(data);
    }
}

void vbomesh_append_indices(struct VboMesh* mesh, void* data, int n) {
    assert( n > 0 );

    if( mesh && mesh->primitives.buffer->id ) {

        if( mesh->primitives.buffer->reserved + n > mesh->primitives.buffer->capacity ) {
            int alloc = mesh->primitives.buffer->reserved + n - mesh->primitives.buffer->capacity;
            vbomesh_alloc_indices(mesh, alloc);
        }

        int append_bytes = n * mesh->index.bytes;
        int offset_bytes = mesh->primitives.buffer->reserved * mesh->index.bytes;
        if( mesh->primitives.buffer->reserved + n <= mesh->primitives.buffer->capacity ) {
            ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->primitives.buffer->id);
                       glBufferSubData(GL_ARRAY_BUFFER, offset_bytes, append_bytes, data);
                       glBindBuffer(GL_ARRAY_BUFFER, 0); );

            mesh->primitives.buffer->reserved += n;
        }
    }
}

void* vbomesh_map(struct VboMesh* mesh, int offset, int length, GLbitfield access) {
    if( mesh && mesh->primitives.buffer->id && offset < mesh->primitives.buffer->capacity ) {
        if( offset + length > mesh->primitives.buffer->capacity ) {
            int alloc = offset + length - mesh->primitives.buffer->capacity + 1;
            vbomesh_alloc_indices(mesh, alloc);
        }

        if( offset + length <= mesh->primitives.buffer->capacity ) {
            int offset_bytes = offset * mesh->index.bytes;
            int length_bytes = length * mesh->index.bytes;
            if( length <= offset ) {
                length_bytes = mesh->primitives.buffer->capacity * mesh->index.bytes;
            }

            void* pointer = NULL;
            ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->primitives.buffer->id);
                       pointer = glMapBufferRange(GL_ARRAY_BUFFER, offset_bytes, length_bytes, access);
                       glBindBuffer(GL_ARRAY_BUFFER, 0); );

            return pointer;
        }

    }

    return NULL;
}

GLboolean vbomesh_unmap(struct VboMesh* mesh) {
    if( mesh && mesh->primitives.buffer->id ) {
        GLboolean result = 0;
        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->primitives.buffer->id);
                   result = glUnmapBuffer(GL_ARRAY_BUFFER);
                   glBindBuffer(GL_ARRAY_BUFFER, 0); );

        return result;
    }

    return 0;
}
