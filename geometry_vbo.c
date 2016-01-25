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

void vbo_create(struct Vbo* p) {
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
}

void vbo_destroy(struct Vbo* p) {
    assert( 0 == 1 );
}

void vbo_print(FILE* f, struct Vbo* vbo) {
    fprintf(f, "vbo->capacity: %lu\n", vbo->capacity);
    fprintf(f, "vbo->occupied: %lu\n", vbo->occupied);
}

void vbo_add_buffer(struct Vbo* vbo,
                    uint32_t i,
                    uint32_t component_n,
                    GLenum component_t,
                    GLenum usage)
{
    assert( vbo != NULL );
    assert( i < NUM_SHADER_ATTRIBUTES );

    if( vbo && i < NUM_SHADER_ATTRIBUTES ) {
        ogl_debug( glGenBuffers(1, &vbo->buffer[i].id) );

        vbo->buffer[i].usage = usage;

        vbo->components[i].size = component_n;
        vbo->components[i].type = component_t;
        vbo->components[i].bytes = (uint32_t)ogl_sizeof_type(component_t);

        size_t nbytes = vbo->capacity * component_n * ogl_sizeof_type(component_t);
        assert( nbytes < PTRDIFF_MAX );
        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, vbo->buffer[i].id);
                   glBufferData(GL_ARRAY_BUFFER, (ptrdiff_t)nbytes, NULL, usage);
                   glBindBuffer(GL_ARRAY_BUFFER, 0); );

    }
}

size_t vbo_alloc(struct Vbo* vbo, size_t n) {
    if( vbo ) {
        size_t resized_bytes = 0;
        for( int32_t i = 0; i < NUM_SHADER_ATTRIBUTES; i++ ) {
            if( vbo->buffer[i].id ) {
                size_t new_bytes = (vbo->capacity + n) * vbo->components[i].size * vbo->components[i].bytes;
                size_t old_bytes = vbo->capacity * vbo->components[i].size * vbo->components[i].bytes;

                resized_bytes = ogl_buffer_resize(&vbo->buffer[i].id, old_bytes, new_bytes);
            }
        }

        // - only increase capacity if all vbos were resized
        // - resized_bytes is different per buffer, so just return n instead
        if( resized_bytes > 0 ) {
            vbo->capacity += n;
            return n;
        }
    }

    return 0;
}

size_t vbo_available_capacity(struct Vbo* vbo) {
    size_t freespace = 0;
    if( vbo ) {
        freespace = vbo->capacity - vbo->occupied;
    }
    return freespace;
}

size_t vbo_available_bytes(struct Vbo* vbo, int32_t i) {
    size_t freespace = 0;
    if( vbo && vbo->buffer[i].id ) {
        freespace = vbo_available_capacity(vbo) * vbo->components[i].size * vbo->components[i].bytes;
    }
    return freespace;
}

void vbo_fill_value(struct Vbo* vbo, int32_t i, size_t offset_n, size_t size_n, uint8_t value) {
    assert( vbo != NULL );
    assert( vbo->buffer[i].id > 0 );
    assert( offset_n < vbo->capacity );
    assert( offset_n > 0 );
    assert( size_n <= vbo->capacity );
    assert( size_n > 0 );

    void* array = malloc( ogl_sizeof_type(vbo->components[i].type) * size_n );
    size_t array_offset = offset_n * vbo->components[i].size;
    size_t array_size = size_n * vbo->components[i].size;
    assert( array_offset < INTPTR_MAX );
    assert( array_size < INTPTR_MAX );

    memset(array, (int)value, array_size);

    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, vbo->buffer[i].id);
               glBufferSubData(GL_ARRAY_BUFFER, (intptr_t)array_offset, (intptr_t)array_size, array);
               glBindBuffer(GL_ARRAY_BUFFER, 0); );
}

void* vbo_map(struct Vbo* vbo, int32_t i, size_t offset, size_t length, GLbitfield access) {
    assert( offset < vbo->capacity );
    assert( vbo != NULL );
    assert( vbo->buffer[i].id > 0 );

    if( offset + length > vbo->capacity ) {
        vbo_alloc(vbo, offset + length - vbo->capacity);
    }

    assert( offset + length <= vbo->capacity );
    size_t offset_bytes = offset * vbo->components[i].size * vbo->components[i].bytes;
    size_t length_bytes = length * vbo->components[i].size * vbo->components[i].bytes;
    if( length <= offset || length > vbo->capacity ) {
        length_bytes = vbo->capacity * vbo->components[i].size * vbo->components[i].bytes;
    }


    void* pointer = NULL;
    assert( offset_bytes < INTPTR_MAX );
    assert( length_bytes < INTPTR_MAX );
    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, vbo->buffer[i].id);
               pointer = glMapBufferRange(GL_ARRAY_BUFFER, (intptr_t)offset_bytes, (intptr_t)length_bytes, access);
               glBindBuffer(GL_ARRAY_BUFFER, 0); );

    return pointer;
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


void vbomesh_create(struct Vbo* vbo, GLenum primitive_type, GLenum index_type, GLenum usage, struct VboMesh* mesh) {
    mesh->vbo = vbo;

    mesh->offset = vbo->occupied;
    mesh->capacity = 0;

    for( int32_t i = 0; i < NUM_SHADER_ATTRIBUTES; i++ ) {
        mesh->occupied[i] = 0;
    }

    mesh->primitives.type = primitive_type;
    mesh->primitives.size = (uint32_t)ogl_sizeof_primitive(primitive_type);

    mesh->index.type = index_type;
    mesh->index.bytes = (uint32_t)ogl_sizeof_type(index_type);

    for( int32_t i = 0; i < NUM_VBO_PHASES; i++ ) {
        mesh->_internal_indices[i].id = 0;
        mesh->_internal_indices[i].usage = usage;
        mesh->_internal_indices[i].base = 0;
        mesh->_internal_indices[i].capacity = 0;
        mesh->_internal_indices[i].occupied = 0;
    }
    mesh->indices = &mesh->_internal_indices[0];

    ogl_debug( glGenBuffers(1, &mesh->indices->id) );
}

void vbomesh_destroy(struct Vbo* vbo, struct VboMesh* mesh) {
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

    fprintf(f, "mesh->offset: %lu\n", mesh->offset);
    fprintf(f, "mesh->capacity: %lu\n", mesh->capacity);

    for( int32_t i = 0; i < NUM_VBO_PHASES; i++ ) {
        for( int32_t j = 0; j < NUM_SHADER_ATTRIBUTES; j++ ) {
            if( mesh->occupied[j] > 0 ) {
                fprintf(f, "mesh->occupied[%d]: %lu\n", j, mesh->occupied[j]);
                fprintf(f, "mesh->vbo->buffer[%d][%d]:\n", i, j);
                switch(mesh->vbo->components[j].type) {
                    case GL_FLOAT: {
                        GLfloat* array = (GLfloat*)vbo_map(mesh->vbo, j, mesh->offset, mesh->capacity, GL_MAP_READ_BIT);
                        if( array ) {
                            for( size_t k = 0; k < mesh->capacity; k++ ) {
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
    fprintf(f, "mesh->indices->capacity: %lu\n", mesh->indices->capacity);
    fprintf(f, "mesh->indices->occupied: %lu\n", mesh->indices->occupied);

    fprintf(f, "mesh->indices:\n");
    switch(mesh->index.type) {
        case GL_UNSIGNED_INT: {
            GLuint* array = (GLuint*)vbomesh_map(mesh, 0, mesh->indices->capacity, GL_MAP_READ_BIT);
            if( array ) {
                uint32_t primitive_size = mesh->primitives.size;
                for( size_t k = 0; k < mesh->indices->capacity; k+=primitive_size ) {
                    printf("[");
                    for( int32_t l = 0; l < primitive_size; l++ ) {
                        fprintf(f, "%d", array[k+(uint32_t)l]);
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

size_t vbomesh_alloc_attributes(struct VboMesh* mesh, size_t n) {
    assert( n > 0 );

    if( mesh == NULL ) {
        return 0;
    }

    if( mesh->capacity == 0 ) {
        mesh->offset = mesh->vbo->occupied;
    }

    if( mesh->offset + mesh->capacity == mesh->vbo->occupied ) {
        size_t resized_n = vbo_alloc(mesh->vbo, n);
        if( resized_n > 0 ) {
            mesh->vbo->occupied += n;
            mesh->capacity += n;
        }

        return resized_n;
    }

    return 0;
}

size_t vbomesh_alloc_indices(struct VboMesh* mesh, size_t n) {
    assert( n > 0 );

    if( mesh && mesh->indices->id ) {
        size_t size_bytes = mesh->indices->capacity * mesh->index.bytes;
        size_t alloc_bytes = n * mesh->index.bytes;
        assert( size_bytes + alloc_bytes < PTRDIFF_MAX );
        size_t resized_bytes = ogl_buffer_resize(&mesh->indices->id, size_bytes, size_bytes + alloc_bytes);

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

size_t vbomesh_append_buffer_generic(struct VboMesh* mesh, int32_t i, void* data, size_t n, uint32_t components_size, GLenum components_type) {
    assert( n > 0 );

    if( mesh && mesh->vbo->buffer[i].id ) {
        if( mesh->capacity == 0 ) {
            mesh->offset = mesh->vbo->occupied;
        }

        // only these depend on given size params => generic data append
        size_t attrib_bytes = components_size * ogl_sizeof_type(components_type);
        size_t append_bytes = n * attrib_bytes;

        // stuff that relies on vbo size values
        size_t capacity_bytes = mesh->capacity * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
        size_t occupied_bytes = mesh->occupied[i] * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
        size_t offset_bytes = (mesh->offset + mesh->occupied[i]) * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;

        assert( offset_bytes < INTPTR_MAX );
        assert( append_bytes < INTPTR_MAX );
        if( occupied_bytes + append_bytes <= capacity_bytes ) {
            ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
                       glBufferSubData(GL_ARRAY_BUFFER, (intptr_t)offset_bytes, (intptr_t)append_bytes, data);
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
                       glBufferSubData(GL_ARRAY_BUFFER, (intptr_t)offset_bytes, (intptr_t)append_bytes, data);
                       glBindBuffer(GL_ARRAY_BUFFER, 0); );

            mesh->occupied[i] += n;
            return n;
        }
    }

    return 0;
}

size_t vbomesh_append_attributes(struct VboMesh* mesh, int32_t i, void* data, size_t n) {
    return vbomesh_append_buffer_generic(mesh, i, data, n, mesh->vbo->components[i].size, mesh->vbo->components[i].type);
}

size_t vbomesh_append_indices(struct VboMesh* mesh, void* data, size_t n) {
    if( mesh && mesh->indices->id ) {

        if( mesh->indices->occupied + n > mesh->indices->capacity ) {
            size_t alloc = mesh->indices->occupied + n - mesh->indices->capacity;
            vbomesh_alloc_indices(mesh, alloc);
        }

        size_t append_bytes = n * mesh->index.bytes;
        size_t offset_bytes = mesh->indices->occupied * mesh->index.bytes;
        assert( offset_bytes < INTPTR_MAX );
        assert( append_bytes < INTPTR_MAX );
        if( mesh->indices->occupied + n <= mesh->indices->capacity ) {
            ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->indices->id);
                       glBufferSubData(GL_ARRAY_BUFFER, (intptr_t)offset_bytes, (intptr_t)append_bytes, data);
                       glBindBuffer(GL_ARRAY_BUFFER, 0); );

            mesh->indices->occupied += n;
            return n;
        }
    }

    return 0;
}

void* vbomesh_map(struct VboMesh* mesh, size_t offset, size_t length, GLbitfield access) {
    if( mesh && mesh->indices->id && offset < mesh->indices->capacity ) {
        if( offset + length > mesh->indices->capacity ) {
            size_t alloc = offset + length - mesh->indices->capacity + 1;
            vbomesh_alloc_indices(mesh, alloc);
        }

        if( offset + length <= mesh->indices->capacity ) {
            size_t offset_bytes = offset * mesh->index.bytes;
            size_t length_bytes = length * mesh->index.bytes;
            if( length <= offset ) {
                length_bytes = mesh->indices->capacity * mesh->index.bytes;
            }
            assert( offset_bytes < INTPTR_MAX );
            assert( length_bytes < INTPTR_MAX );

            void* pointer = NULL;
            ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->indices->id);
                       pointer = glMapBufferRange(GL_ARRAY_BUFFER, (intptr_t)offset_bytes, (intptr_t)length_bytes, access);
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
