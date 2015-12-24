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
    int ret = 0;

    glewInit();
    if( ! glewGetExtension("GL_ARB_copy_buffer") &&
        ! glewGetExtension("GL_EXT_copy_buffer") )
    {
        printf("ERROR: copy_buffer extension not found!\n");
        ret = 1;
    }

    if( ! glewGetExtension("GL_ARB_vertex_array_object") &&
        ! glewGetExtension("GL_EXT_vertex_array_object") )
    {
        printf("ERROR: vertex_array_object extension not found!\n");
        ret = 1;
    }

    /* if( ! glewGetExtension("GL_ARB_buffer_storage" ) ) { */
    /*     printf("ERROR: buffer_storage extension not found!\n"); */
    /*     ret = 1; */
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
        for( int j = 0; j < NUM_OGL_ATTRIBUTES; j++ ) {
            p->_internal_buffer[i][j].id = 0;
            p->_internal_buffer[i][j].usage = GL_STATIC_DRAW;
        }
    }
    p->buffer = p->_internal_buffer[0];

    for( int i = 0; i < NUM_OGL_ATTRIBUTES; i++ ) {
        p->components[i].size = 0;
        p->components[i].type = 0;
        p->components[i].bytes = 0;
    }

    p->capacity = 0;
    p->occupied = 0;

    for( int i = 0; i < NUM_VBO_PHASES; i++ ) {
        p->scheduler.fence[i] = 0;
    }
    p->scheduler.phase = 0;
    p->scheduler.type = VBO_MANY_BUFFER;
    p->scheduler.offset = 0;

    return 1;
}

int vbo_destroy(struct Vbo* p) {
    assert( 0 == 1 );
}

void vbo_print(struct Vbo* vbo) {
    printf("vbo->capacity: %d\n", vbo->capacity);
    printf("vbo->occupied: %d\n", vbo->occupied);
}

void vbo_add_buffer(struct Vbo* vbo,
                    int i,
                    int component_n,
                    GLenum component_t,
                    GLenum usage)
{
    if( vbo && i < NUM_OGL_ATTRIBUTES ) {
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

        for( int i = 0; i < NUM_OGL_ATTRIBUTES && resized_bytes != 0; i++ ) {
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
        freespace = vbo->capacity - vbo->occupied;
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

void vbo_fill_value(struct Vbo* vbo, int i, int offset_n, int size_n, int value) {
    if( vbo && vbo->buffer[i].id && offset_n < vbo->capacity && size_n <= vbo->capacity ) {
        void* array = malloc( sizeof_type(vbo->components[i].type) * size_n );
        int array_offset = offset_n * vbo->components[i].size;
        int array_size = size_n * vbo->components[i].size;

        switch(vbo->components[i].type) {
            case GL_FLOAT: {
                memset(array, (GLfloat)value, array_size);
                break;
            }
            case GL_INT: {
                memset(array, value, array_size);
                break;
            }
            case GL_UNSIGNED_INT: {
                memset(array, (GLuint)value, array_size);
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


int vbomesh_create(struct Vbo* vbo, GLenum primitive_type, GLenum index_type, GLenum usage, struct VboMesh* mesh) {
    if( vbo ) {
        mesh->vbo = vbo;

        mesh->offset = vbo->occupied;
        mesh->capacity = 0;

        for( int i = 0; i < NUM_OGL_ATTRIBUTES; i++ ) {
            mesh->occupied[i] = 0;
        }

        mesh->primitives.type = primitive_type;
        mesh->primitives.size = sizeof_primitive(primitive_type);

        mesh->index.type = index_type;
        mesh->index.bytes = sizeof_type(index_type);

        for( int i = 0; i < NUM_VBO_PHASES; i++ ) {
            mesh->_internal_indices[i].id = 0;
            mesh->_internal_indices[i].usage = usage;
            mesh->_internal_indices[i].base = 0;
            mesh->_internal_indices[i].capacity = 0;
            mesh->_internal_indices[i].occupied = 0;
        }
        mesh->indices = &mesh->_internal_indices[0];

        ogl_debug( glGenBuffers(1, &mesh->indices->id) );

        return 1;
    } else {
        return 0;
    }
}

int vbomesh_destroy(struct Vbo* vbo, struct VboMesh* mesh) {
    assert( vbo );
    assert( mesh );
    assert( 0 == 1 );

    if( mesh->offset + mesh->capacity == mesh->vbo->occupied ) {
        mesh->vbo->occupied = mesh->offset;
    }
}

void vbomesh_print(struct VboMesh* mesh) {
    vbo_print(mesh->vbo);

    printf("\n");

    printf("mesh->offset: %d\n", mesh->offset);
    printf("mesh->capacity: %d\n", mesh->capacity);

    for( int i = 0; i < NUM_VBO_PHASES; i++ ) {
        for( int j = 0; j < NUM_OGL_ATTRIBUTES; j++ ) {
            if( mesh->occupied[j] > 0 ) {
                printf("mesh->occupied[%d]: %d\n", j, mesh->occupied[j]);
                printf("mesh->vbo->buffer[%d][%d]:\n", i, j);
                switch(mesh->vbo->components[j].type) {
                    case GL_FLOAT: {
                        GLfloat* array = (GLfloat*)vbo_map(mesh->vbo, j, mesh->offset, mesh->capacity, GL_MAP_READ_BIT);
                        if( array ) {
                            for( int k = 0; k < mesh->capacity; k++ ) {
                                if( mesh->vbo->components[j].size == 3 ) {
                                    GLfloat a = array[k*mesh->vbo->components[j].size+0];
                                    GLfloat b = array[k*mesh->vbo->components[j].size+1];
                                    GLfloat c = array[k*mesh->vbo->components[j].size+2];
                                    printf("[%.2f %.2f %.2f]", a, b, c);
                                } else if( mesh->vbo->components[j].size == 4 ) {
                                    GLfloat a = array[k*mesh->vbo->components[j].size+0];
                                    GLfloat b = array[k*mesh->vbo->components[j].size+1];
                                    GLfloat c = array[k*mesh->vbo->components[j].size+2];
                                    GLfloat d = array[k*mesh->vbo->components[j].size+3];
                                    printf("[%.2f %.2f %.2f %.2f]", a, b, c, d);
                                }

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

    printf("mesh->primitives.type: %d\n", mesh->primitives.type);
    printf("mesh->primitives.size: %d\n", mesh->primitives.size);

    printf("mesh->index.type: %d\n", mesh->index.type);
    printf("mesh->index.bytes: %d\n", mesh->index.bytes);
    printf("mesh->indices->capacity: %d\n", mesh->indices->capacity);
    printf("mesh->indices->occupied: %d\n", mesh->indices->occupied);

    printf("mesh->indices:\n");
    switch(mesh->index.type) {
        case GL_UNSIGNED_INT: {
            GLuint* array = (GLuint*)vbomesh_map(mesh, 0, mesh->indices->capacity, GL_MAP_READ_BIT);
            if( array ) {
                int primitive_size = mesh->primitives.size;
                for( int k = 0; k < mesh->indices->capacity; k+=primitive_size ) {
                    printf("[");
                    for( int l = 0; l < primitive_size; l++ ) {
                        printf("%d", array[k+l]);
                        if( l < primitive_size - 1 ) {
                            printf(", ");
                        }
                    }
                    printf("]");
                    if( k == mesh->indices->capacity - primitive_size ) {
                        printf("\n");
                    } else {
                        printf(", ");
                    }
                }
            }
            vbomesh_unmap(mesh);
            break;
        }
    }
}

int vbomesh_alloc_attributes(struct VboMesh* mesh, int n) {
    assert( n > 0 );

    if( mesh == NULL ) {
        return 0;
    }

    if( mesh->capacity == 0 ) {
        mesh->offset = mesh->vbo->occupied;
    }

    if( mesh->offset + mesh->capacity == mesh->vbo->occupied ) {
        int resized_n = vbo_alloc(mesh->vbo, n);
        if( resized_n > 0 ) {
            mesh->vbo->occupied += n;
            mesh->capacity += n;
        }

        return resized_n;
    }

    return 0;
}

int vbomesh_alloc_indices(struct VboMesh* mesh, int n) {
    assert( n > 0 );

    if( mesh && mesh->indices->id ) {
        int size_bytes = mesh->indices->capacity * mesh->index.bytes;
        int alloc_bytes = n * mesh->index.bytes;
        int resized_bytes = buffer_resize(&mesh->indices->id, size_bytes, size_bytes + alloc_bytes);

        // - we could return resized_bytes, but all other alloc functions return the number
        //   of elements allocated, so we just do the same here
        if( resized_bytes == alloc_bytes ) {
            mesh->indices->capacity += n;
            return n;
        } else {
            assert( resized_bytes == 0 );
        }
    }

    return 0;
}

void vbomesh_clear_attributes(struct VboMesh* mesh) {
    for( int i = 0; i < NUM_OGL_ATTRIBUTES; i++ ) {
        mesh->occupied[i] = 0;
    }
}

void vbomesh_clear_indices(struct VboMesh* mesh) {
    for( int i = 0; i < NUM_VBO_PHASES; i++ ) {
        mesh->_internal_indices[i].occupied = 0;
    }
}

int vbomesh_append_buffer_generic(struct VboMesh* mesh, int i, void* data, int n, int components_size, GLenum components_type) {
    assert( n > 0 );

    if( mesh && mesh->vbo->buffer[i].id ) {
        if( mesh->capacity == 0 ) {
            mesh->offset = mesh->vbo->occupied;
        }

        // only these depend on given size params => generic data append
        int attrib_bytes = components_size * sizeof_type(components_type);
        int append_bytes = n * attrib_bytes;

        // stuff that relies on vbo size values
        int capacity_bytes = mesh->capacity * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
        int occupied_bytes = mesh->occupied[i] * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
        int offset_bytes = (mesh->offset + mesh->occupied[i]) * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;

        if( occupied_bytes + append_bytes <= capacity_bytes ) {
            ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
                       glBufferSubData(GL_ARRAY_BUFFER, offset_bytes, append_bytes, data);
                       glBindBuffer(GL_ARRAY_BUFFER, 0); );


            mesh->occupied[i] += n;
            return n;
        } else if( // only when this mesh is the last mesh in the vbo can we append without overwriting
                   // other meshes, we check if we are last buy checking if our offset + capacity is equal
                   // to the vbo occupied counter
                   mesh->offset + mesh->capacity == mesh->vbo->occupied &&
                   // this does not work genericly, so we just do not allocate anything at all,
                   // if num and type do not fit the stored values in vbo
                   mesh->vbo->components[i].size == components_size &&
                   mesh->vbo->components[i].type == components_type )
        {
            if( vbo_available_capacity(mesh->vbo) < n ) {
                vbomesh_alloc_attributes(mesh,n);
            }

            ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
                       glBufferSubData(GL_ARRAY_BUFFER, offset_bytes, append_bytes, data);
                       glBindBuffer(GL_ARRAY_BUFFER, 0); );

            mesh->occupied[i] += n;
            return n;
        }
    }

    return 0;
}

int vbomesh_append_attributes(struct VboMesh* mesh, int i, void* data, int n) {
    return vbomesh_append_buffer_generic(mesh, i, data, n, mesh->vbo->components[i].size, mesh->vbo->components[i].type);
}

int vbomesh_append_indices(struct VboMesh* mesh, void* data, int n) {
    assert( n > 0 );

    if( mesh && mesh->indices->id ) {

        if( mesh->indices->occupied + n > mesh->indices->capacity ) {
            int alloc = mesh->indices->occupied + n - mesh->indices->capacity;
            vbomesh_alloc_indices(mesh, alloc);
        }

        int append_bytes = n * mesh->index.bytes;
        int offset_bytes = mesh->indices->occupied * mesh->index.bytes;
        if( mesh->indices->occupied + n <= mesh->indices->capacity ) {
            ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->indices->id);
                       glBufferSubData(GL_ARRAY_BUFFER, offset_bytes, append_bytes, data);
                       glBindBuffer(GL_ARRAY_BUFFER, 0); );

            mesh->indices->occupied += n;
            return n;
        }
    }

    return 0;
}

void* vbomesh_map(struct VboMesh* mesh, int offset, int length, GLbitfield access) {
    if( mesh && mesh->indices->id && offset < mesh->indices->capacity ) {
        if( offset + length > mesh->indices->capacity ) {
            int alloc = offset + length - mesh->indices->capacity + 1;
            vbomesh_alloc_indices(mesh, alloc);
        }

        if( offset + length <= mesh->indices->capacity ) {
            int offset_bytes = offset * mesh->index.bytes;
            int length_bytes = length * mesh->index.bytes;
            if( length <= offset ) {
                length_bytes = mesh->indices->capacity * mesh->index.bytes;
            }

            void* pointer = NULL;
            ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->indices->id);
                       pointer = glMapBufferRange(GL_ARRAY_BUFFER, offset_bytes, length_bytes, access);
                       glBindBuffer(GL_ARRAY_BUFFER, 0); );

            return pointer;
        }

    }

    return NULL;
}

GLboolean vbomesh_unmap(struct VboMesh* mesh) {
    if( mesh && mesh->indices->id ) {
        GLboolean result = 0;
        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->indices->id);
                   result = glUnmapBuffer(GL_ARRAY_BUFFER);
                   glBindBuffer(GL_ARRAY_BUFFER, 0); );

        return result;
    }

    return 0;
}
