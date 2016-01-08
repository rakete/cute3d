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

int32_t init_vbo() {
    int32_t ret = 0;

    glewInit();
    if( ! glewGetExtension("GL_ARB_copy_buffer") &&
        ! glewGetExtension("GL_EXT_copy_buffer") )
    {
        log_fail(stderr, __FILE__, __LINE__, "copy_buffer extension not found!\n");
        ret = 1;
    }

    if( ! glewGetExtension("GL_ARB_vertex_array_object") &&
        ! glewGetExtension("GL_EXT_vertex_array_object") )
    {
        log_fail(stderr, __FILE__, __LINE__, "vertex_array_object extension not found!\n");
        ret = 1;
    }

    /* if( ! glewGetExtension("GL_ARB_buffer_storage" ) ) { */
    /*     printf("ERROR: buffer_storage extension not found!\n"); */
    /*     ret = 1; */
    /* } */


    return ret;
}

int32_t vbo_create(struct Vbo* p) {
    for( int32_t i = 0; i < NUM_VBO_PHASES; i++ ) {
        for( int32_t j = 0; j < NUM_SHADER_ATTRIBUTES; j++ ) {
            p->_internal_buffer[i][j].id = 0;
            p->_internal_buffer[i][j].usage = GL_STATIC_DRAW;
        }
    }
    p->buffer = p->_internal_buffer[0];

    for( int32_t i = 0; i < NUM_SHADER_ATTRIBUTES; i++ ) {
        p->components[i].size = 0;
        p->components[i].type = 0;
        p->components[i].bytes = 0;
    }

    p->capacity = 0;
    p->occupied = 0;

    for( int32_t i = 0; i < NUM_VBO_PHASES; i++ ) {
        p->scheduler.fence[i] = 0;
    }
    p->scheduler.phase = 0;
    p->scheduler.type = VBO_MANY_BUFFER;
    p->scheduler.offset = 0;

    return 1;
}

int32_t vbo_destroy(struct Vbo* p) {
    assert( 0 == 1 );
}

void vbo_print(FILE* f, struct Vbo* vbo) {
    fprintf(f, "vbo->capacity: %d\n", vbo->capacity);
    fprintf(f, "vbo->occupied: %d\n", vbo->occupied);
}

void vbo_add_buffer(struct Vbo* vbo,
                    int32_t i,
                    int32_t component_n,
                    GLenum component_t,
                    GLenum usage)
{
    if( vbo && i < NUM_SHADER_ATTRIBUTES ) {
        ogl_debug( glGenBuffers(1, &vbo->buffer[i].id) );

        vbo->buffer[i].usage = usage;

        vbo->components[i].size = component_n;
        vbo->components[i].type = component_t;
        vbo->components[i].bytes = ogl_sizeof_type(component_t);

        int32_t nbytes = vbo->capacity * component_n * ogl_sizeof_type(component_t);
        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, vbo->buffer[i].id);
                   glBufferData(GL_ARRAY_BUFFER, nbytes, NULL, usage);
                   glBindBuffer(GL_ARRAY_BUFFER, 0); );

    }
}

int32_t vbo_alloc(struct Vbo* vbo, int32_t n) {
    assert( n > 0 );

    if( vbo ) {
        // I set this to -1 so I can check in the for loop for != 0, so that I can decide afterwards if any
        // buffer_resize failed (and returned 0), or if there were no existing vbos (which means resized_bytes
        // is still -1 after the loop)
        int32_t resized_bytes = -1;

        for( int32_t i = 0; i < NUM_SHADER_ATTRIBUTES && resized_bytes != 0; i++ ) {
            if( vbo->buffer[i].id ) {
                int32_t new_bytes = (vbo->capacity + n) * vbo->components[i].size * vbo->components[i].bytes;
                int32_t old_bytes = vbo->capacity * vbo->components[i].size * vbo->components[i].bytes;

                resized_bytes = ogl_buffer_resize(&vbo->buffer[i].id, old_bytes, new_bytes);
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

int32_t vbo_available_capacity(struct Vbo* vbo) {
    int32_t freespace = 0;
    if( vbo ) {
        freespace = vbo->capacity - vbo->occupied;
    }
    return freespace;
}

int32_t vbo_available_bytes(struct Vbo* vbo, int32_t i) {
    int32_t freespace = 0;
    if( vbo && vbo->buffer[i].id ) {
        freespace = vbo_available_capacity(vbo) * vbo->components[i].size * vbo->components[i].bytes;
    }
    return freespace;
}

void vbo_fill_value(struct Vbo* vbo, int32_t i, int32_t offset_n, int32_t size_n, int32_t value) {
    if( vbo && vbo->buffer[i].id && offset_n < vbo->capacity && size_n <= vbo->capacity ) {
        void* array = malloc( ogl_sizeof_type(vbo->components[i].type) * size_n );
        int32_t array_offset = offset_n * vbo->components[i].size;
        int32_t array_size = size_n * vbo->components[i].size;

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

void* vbo_map(struct Vbo* vbo, int32_t i, int32_t offset, int32_t length, GLbitfield access) {
    if( vbo && vbo->buffer[i].id && offset < vbo->capacity ) {
        if( offset + length > vbo->capacity ) {
            vbo_alloc(vbo, offset + length - vbo->capacity);
        }

        if( offset + length <= vbo->capacity ) {
            int32_t offset_bytes = offset * vbo->components[i].size * vbo->components[i].bytes;
            int32_t length_bytes = length * vbo->components[i].size * vbo->components[i].bytes;
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

GLboolean vbo_unmap(struct Vbo* vbo, int32_t i) {
    if( vbo && vbo->buffer[i].id ) {
        GLboolean result = 0;
        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, vbo->buffer[i].id);
                   result = glUnmapBuffer(GL_ARRAY_BUFFER);
                   glBindBuffer(GL_ARRAY_BUFFER, 0); );

        return result;
    }

    return 0;
}


int32_t vbomesh_create(struct Vbo* vbo, GLenum primitive_type, GLenum index_type, GLenum usage, struct VboMesh* mesh) {
    if( vbo ) {
        mesh->vbo = vbo;

        mesh->offset = vbo->occupied;
        mesh->capacity = 0;

        for( int32_t i = 0; i < NUM_SHADER_ATTRIBUTES; i++ ) {
            mesh->occupied[i] = 0;
        }

        mesh->primitives.type = primitive_type;
        mesh->primitives.size = ogl_sizeof_primitive(primitive_type);

        mesh->index.type = index_type;
        mesh->index.bytes = ogl_sizeof_type(index_type);

        for( int32_t i = 0; i < NUM_VBO_PHASES; i++ ) {
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

int32_t vbomesh_destroy(struct Vbo* vbo, struct VboMesh* mesh) {
    assert( vbo );
    assert( mesh );
    assert( 0 == 1 );

    if( mesh->offset + mesh->capacity == mesh->vbo->occupied ) {
        mesh->vbo->occupied = mesh->offset;
    }
}

void vbomesh_print(FILE* f, struct VboMesh* mesh) {
    vbo_print(f, mesh->vbo);

    fprintf(f, "\n");

    fprintf(f, "mesh->offset: %d\n", mesh->offset);
    fprintf(f, "mesh->capacity: %d\n", mesh->capacity);

    for( int32_t i = 0; i < NUM_VBO_PHASES; i++ ) {
        for( int32_t j = 0; j < NUM_SHADER_ATTRIBUTES; j++ ) {
            if( mesh->occupied[j] > 0 ) {
                fprintf(f, "mesh->occupied[%d]: %d\n", j, mesh->occupied[j]);
                fprintf(f, "mesh->vbo->buffer[%d][%d]:\n", i, j);
                switch(mesh->vbo->components[j].type) {
                    case GL_FLOAT: {
                        GLfloat* array = (GLfloat*)vbo_map(mesh->vbo, j, mesh->offset, mesh->capacity, GL_MAP_READ_BIT);
                        if( array ) {
                            for( int32_t k = 0; k < mesh->capacity; k++ ) {
                                if( mesh->vbo->components[j].size == 3 ) {
                                    GLfloat a = array[k*mesh->vbo->components[j].size+0];
                                    GLfloat b = array[k*mesh->vbo->components[j].size+1];
                                    GLfloat c = array[k*mesh->vbo->components[j].size+2];
                                    fprintf(f, "[%.2f %.2f %.2f]", a, b, c);
                                } else if( mesh->vbo->components[j].size == 4 ) {
                                    GLfloat a = array[k*mesh->vbo->components[j].size+0];
                                    GLfloat b = array[k*mesh->vbo->components[j].size+1];
                                    GLfloat c = array[k*mesh->vbo->components[j].size+2];
                                    GLfloat d = array[k*mesh->vbo->components[j].size+3];
                                    fprintf(f, "[%.2f %.2f %.2f %.2f]", a, b, c, d);
                                }

                                if( k == mesh->capacity - 1 ) {
                                    fprintf(f, "\n");
                                } else {
                                    fprintf(f, ", ");
                                }
                            }
                        } else {
                            fprintf(f, "NULL\n");
                        }
                        vbo_unmap(mesh->vbo, j);
                        break;
                    }
                    case GL_INT: {
                        fprintf(f, "ERROR: GL_INT not implemented in vbomesh_print32_t\n");
                        break;
                    }
                }
            }
        }
    }

    fprintf(f, "mesh->primitives.type: %d\n", mesh->primitives.type);
    fprintf(f, "mesh->primitives.size: %d\n", mesh->primitives.size);

    fprintf(f, "mesh->index.type: %d\n", mesh->index.type);
    fprintf(f, "mesh->index.bytes: %d\n", mesh->index.bytes);
    fprintf(f, "mesh->indices->capacity: %d\n", mesh->indices->capacity);
    fprintf(f, "mesh->indices->occupied: %d\n", mesh->indices->occupied);

    fprintf(f, "mesh->indices:\n");
    switch(mesh->index.type) {
        case GL_UNSIGNED_INT: {
            GLuint* array = (GLuint*)vbomesh_map(mesh, 0, mesh->indices->capacity, GL_MAP_READ_BIT);
            if( array ) {
                int32_t primitive_size = mesh->primitives.size;
                for( int32_t k = 0; k < mesh->indices->capacity; k+=primitive_size ) {
                    printf("[");
                    for( int32_t l = 0; l < primitive_size; l++ ) {
                        fprintf(f, "%d", array[k+l]);
                        if( l < primitive_size - 1 ) {
                            printf(", ");
                        }
                    }
                    fprintf(f, "]");
                    if( k == mesh->indices->capacity - primitive_size ) {
                        fprintf(f, "\n");
                    } else {
                        fprintf(f, ", ");
                    }
                }
            }
            vbomesh_unmap(mesh);
            break;
        }
    }
}

int32_t vbomesh_alloc_attributes(struct VboMesh* mesh, int32_t n) {
    assert( n > 0 );

    if( mesh == NULL ) {
        return 0;
    }

    if( mesh->capacity == 0 ) {
        mesh->offset = mesh->vbo->occupied;
    }

    if( mesh->offset + mesh->capacity == mesh->vbo->occupied ) {
        int32_t resized_n = vbo_alloc(mesh->vbo, n);
        if( resized_n > 0 ) {
            mesh->vbo->occupied += n;
            mesh->capacity += n;
        }

        return resized_n;
    }

    return 0;
}

int32_t vbomesh_alloc_indices(struct VboMesh* mesh, int32_t n) {
    assert( n > 0 );

    if( mesh && mesh->indices->id ) {
        int32_t size_bytes = mesh->indices->capacity * mesh->index.bytes;
        int32_t alloc_bytes = n * mesh->index.bytes;
        int32_t resized_bytes = ogl_buffer_resize(&mesh->indices->id, size_bytes, size_bytes + alloc_bytes);

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
    for( int32_t i = 0; i < NUM_SHADER_ATTRIBUTES; i++ ) {
        mesh->occupied[i] = 0;
    }
}

void vbomesh_clear_indices(struct VboMesh* mesh) {
    for( int32_t i = 0; i < NUM_VBO_PHASES; i++ ) {
        mesh->_internal_indices[i].occupied = 0;
    }
}

int32_t vbomesh_append_buffer_generic(struct VboMesh* mesh, int32_t i, void* data, int32_t n, int32_t components_size, GLenum components_type) {
    assert( n > 0 );

    if( mesh && mesh->vbo->buffer[i].id ) {
        if( mesh->capacity == 0 ) {
            mesh->offset = mesh->vbo->occupied;
        }

        // only these depend on given size params => generic data append
        int32_t attrib_bytes = components_size * ogl_sizeof_type(components_type);
        int32_t append_bytes = n * attrib_bytes;

        // stuff that relies on vbo size values
        int32_t capacity_bytes = mesh->capacity * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
        int32_t occupied_bytes = mesh->occupied[i] * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
        int32_t offset_bytes = (mesh->offset + mesh->occupied[i]) * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;

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

int32_t vbomesh_append_attributes(struct VboMesh* mesh, int32_t i, void* data, int32_t n) {
    return vbomesh_append_buffer_generic(mesh, i, data, n, mesh->vbo->components[i].size, mesh->vbo->components[i].type);
}

int32_t vbomesh_append_indices(struct VboMesh* mesh, void* data, int32_t n) {
    assert( n > 0 );

    if( mesh && mesh->indices->id ) {

        if( mesh->indices->occupied + n > mesh->indices->capacity ) {
            int32_t alloc = mesh->indices->occupied + n - mesh->indices->capacity;
            vbomesh_alloc_indices(mesh, alloc);
        }

        int32_t append_bytes = n * mesh->index.bytes;
        int32_t offset_bytes = mesh->indices->occupied * mesh->index.bytes;
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

void* vbomesh_map(struct VboMesh* mesh, int32_t offset, int32_t length, GLbitfield access) {
    if( mesh && mesh->indices->id && offset < mesh->indices->capacity ) {
        if( offset + length > mesh->indices->capacity ) {
            int32_t alloc = offset + length - mesh->indices->capacity + 1;
            vbomesh_alloc_indices(mesh, alloc);
        }

        if( offset + length <= mesh->indices->capacity ) {
            int32_t offset_bytes = offset * mesh->index.bytes;
            int32_t length_bytes = length * mesh->index.bytes;
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
