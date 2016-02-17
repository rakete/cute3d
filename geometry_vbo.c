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
    log_assert( p != NULL );

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
    log_assert( 0 == 1 );
}

void vbo_print(struct Vbo* vbo) {
    log_assert( vbo != NULL );

    printf("vbo->capacity: %lu\n", vbo->capacity);
    printf("vbo->occupied: %lu\n", vbo->occupied);
}

void vbo_add_buffer(struct Vbo* vbo,
                    uint32_t i,
                    uint32_t components_size,
                    GLenum components_type,
                    GLenum usage)
{
    log_assert( vbo != NULL );
    log_assert( i < NUM_SHADER_ATTRIBUTES );
    log_assert( components_size == 2 || components_size == 3 || components_size == 4,
                "components_size(%d) == 2 || components_size(%d) == 3 || components_size(%d) == 4: the current implementation assumes attributes to have either 2, 3 or 4 components, you have given components_size %d\n",
                components_size, components_size, components_size, components_size);
    log_assert( components_type == GL_FLOAT || components_type == GL_UNSIGNED_BYTE,
                "components_type(%d) == GL_FLOAT(%d) || components_type(%d) == GL_UNSIGNED_BYTE(%d): the current implementation assumes attributes to have either GL_FLOAT or GL_UNSIGNED_BYTE components, you have given components_type %d\n",
                components_type, GL_FLOAT, components_type, GL_UNSIGNED_BYTE, components_type );

    ogl_debug( glGenBuffers(1, &vbo->buffer[i].id) );

    vbo->buffer[i].usage = usage;

    vbo->components[i].size = components_size;
    vbo->components[i].type = components_type;
    vbo->components[i].bytes = (uint32_t)ogl_sizeof_type(components_type);

    size_t nbytes = vbo->capacity * components_size * ogl_sizeof_type(components_type);
    log_assert( nbytes < PTRDIFF_MAX );
    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, vbo->buffer[i].id);
               glBufferData(GL_ARRAY_BUFFER, (ptrdiff_t)nbytes, NULL, usage);
               glBindBuffer(GL_ARRAY_BUFFER, 0); );

}

size_t vbo_alloc(struct Vbo* vbo, size_t n) {
    log_assert( vbo != NULL );
    log_assert( n > 0 );

    size_t resized_bytes = 0;
    for( int32_t i = 0; i < NUM_SHADER_ATTRIBUTES; i++ ) {
        if( vbo->buffer[i].id ) {
            size_t new_bytes = (vbo->capacity + n) * vbo->components[i].size * vbo->components[i].bytes;
            size_t old_bytes = vbo->capacity * vbo->components[i].size * vbo->components[i].bytes;

            resized_bytes = ogl_buffer_resize(&vbo->buffer[i].id, old_bytes, new_bytes);
            log_assert( resized_bytes == new_bytes - old_bytes );
        }
    }

    // - only increase capacity if all vbos were resized
    // - resized_bytes is different per buffer, so just return n instead
    log_assert( resized_bytes > 0 );
    vbo->capacity += n;
    return n;
}

size_t vbo_available_capacity(struct Vbo* vbo) {
    log_assert( vbo != NULL );
    log_assert( vbo->capacity >= vbo->occupied );

    size_t freespace = 0;
    freespace = vbo->capacity - vbo->occupied;

    return freespace;
}

size_t vbo_available_bytes(struct Vbo* vbo, int32_t i) {
    log_assert( vbo != NULL );
    log_assert( i >= 0 );

    size_t freespace = 0;
    freespace = vbo_available_capacity(vbo) * vbo->components[i].size * vbo->components[i].bytes;

    return freespace;
}

void vbo_fill_value(struct Vbo* vbo, int32_t i, size_t offset_n, size_t size_n, uint8_t value) {
    log_assert( vbo != NULL );
    log_assert( vbo->buffer[i].id > 0 );
    log_assert( offset_n < vbo->capacity );
    log_assert( offset_n > 0 );
    log_assert( size_n <= vbo->capacity );
    log_assert( size_n > 0 );

    void* array = malloc( ogl_sizeof_type(vbo->components[i].type) * size_n );
    size_t array_offset = offset_n * vbo->components[i].size;
    size_t array_size = size_n * vbo->components[i].size;
    log_assert( array_offset < INTPTR_MAX );
    log_assert( array_size < INTPTR_MAX );

    memset(array, (int)value, array_size);

    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, vbo->buffer[i].id);
               glBufferSubData(GL_ARRAY_BUFFER, (intptr_t)array_offset, (intptr_t)array_size, array);
               glBindBuffer(GL_ARRAY_BUFFER, 0); );
}

void* vbo_map(struct Vbo* vbo, int32_t i, size_t offset, size_t length, GLbitfield access) {
    log_assert( offset < vbo->capacity );
    log_assert( vbo != NULL );
    log_assert( i >= 0 );
    log_assert( vbo->buffer[i].id > 0 );

    if( offset + length > vbo->capacity ) {
        size_t alloc = offset + length - vbo->capacity;
        size_t result = vbo_alloc(vbo, alloc);
        log_assert( result == alloc );
    }

    log_assert( offset + length <= vbo->capacity );
    size_t offset_bytes = offset * vbo->components[i].size * vbo->components[i].bytes;
    size_t length_bytes = length * vbo->components[i].size * vbo->components[i].bytes;
    if( length <= offset || length > vbo->capacity ) {
        length_bytes = vbo->capacity * vbo->components[i].size * vbo->components[i].bytes;
    }

    void* pointer = NULL;
    log_assert( offset_bytes < INTPTR_MAX );
    log_assert( length_bytes < INTPTR_MAX );
    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, vbo->buffer[i].id);
               pointer = glMapBufferRange(GL_ARRAY_BUFFER, (intptr_t)offset_bytes, (intptr_t)length_bytes, access);
               glBindBuffer(GL_ARRAY_BUFFER, 0); );

    return pointer;
}

GLboolean vbo_unmap(struct Vbo* vbo, int32_t i) {
    log_assert( vbo != NULL );
    log_assert( i >= 0 );
    log_assert( vbo->buffer[i].id > 0 );

    GLboolean result = 0;
    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, vbo->buffer[i].id);
               result = glUnmapBuffer(GL_ARRAY_BUFFER);
               glBindBuffer(GL_ARRAY_BUFFER, 0); );

    return result;
}


void vbomesh_create(struct Vbo* vbo, GLenum primitive_type, GLenum index_type, GLenum usage, struct VboMesh* mesh) {
    log_assert( vbo != NULL );
    log_assert( mesh != NULL );

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
    log_assert( vbo != NULL );
    log_assert( mesh != NULL );
    log_assert( mesh->vbo != NULL );

    if( vbomesh_test_last(mesh) ) {
        mesh->vbo->occupied = mesh->offset;
    }

    log_assert( 0 == 1 );
}

void vbomesh_print(struct VboMesh* mesh) {
    log_assert( mesh != NULL );
    log_assert( mesh->vbo != NULL );

    vbo_print(mesh->vbo);

    printf("\n");

    printf("mesh->offset: %lu\n", mesh->offset);
    printf("mesh->capacity: %lu\n", mesh->capacity);

    for( int32_t i = 0; i < NUM_VBO_PHASES; i++ ) {
        for( int32_t j = 0; j < NUM_SHADER_ATTRIBUTES; j++ ) {
            if( mesh->occupied[j] > 0 ) {
                printf("mesh->occupied[%d]: %lu\n", j, mesh->occupied[j]);
                printf("mesh->vbo->buffer[%d][%d]:\n", i, j);
                switch(mesh->vbo->components[j].type) {
                    case GL_FLOAT: {
                        GLfloat* array = (GLfloat*)vbo_map(mesh->vbo, j, mesh->offset, mesh->capacity, GL_MAP_READ_BIT);
                        if( array ) {
                            for( size_t k = 0; k < mesh->capacity; k++ ) {
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
                            printf("vbo_map returned NULL\n");
                        }
                        vbo_unmap(mesh->vbo, j);
                        break;
                    }
                    case GL_INT: {
                        log_fail(stderr, __FILE__, __LINE__, "GL_INT case not implemented in vbomesh_print\n");
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
    printf("mesh->indices->capacity: %lu\n", mesh->indices->capacity);
    printf("mesh->indices->occupied: %lu\n", mesh->indices->occupied);

    printf("mesh->indices:\n");
    switch(mesh->index.type) {
        case GL_UNSIGNED_INT: {
            GLuint* array = (GLuint*)vbomesh_map(mesh, 0, mesh->indices->capacity, GL_MAP_READ_BIT);
            if( array ) {
                uint32_t primitive_size = mesh->primitives.size;
                for( size_t k = 0; k < mesh->indices->capacity; k+=primitive_size ) {
                    printf("[");
                    for( uint32_t l = 0; l < primitive_size; l++ ) {
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

bool vbomesh_test_last(struct VboMesh* mesh) {
    log_assert( mesh != NULL );
    log_assert( mesh->vbo != NULL );

    // - return always true when mesh->capacity == 0, because that means the mesh has never been allocated space
    // and can therefore be moved at the end of the vbo by vbomesh_alloc as soon as we use it to actually allocate
    // some space
    // - the part after || is the actual test, when mesh->offset + mesh->capacity equal vbo->occupied no other mesh
    // has been added to the vbo after this one, so we are the last
    return mesh->capacity == 0 || mesh->offset + mesh->capacity == mesh->vbo->occupied;
}

size_t vbomesh_alloc_attributes(struct VboMesh* mesh, size_t n) {
    log_assert( mesh != NULL );
    log_assert( mesh->vbo != NULL );
    log_assert( n > 0 );

    // - only resize if the mesh is the last mesh, otherwise this needs to stay the same size
    if( vbomesh_test_last(mesh) ) {
        size_t resized_n = vbo_alloc(mesh->vbo, n);
        if( resized_n == n ) {
            // if this mesh has never been touched before (when no space has been allocated for it in the vbo), move
            // the mesh to the last position just after allocating
            if( mesh->capacity == 0 ) {
                mesh->offset = mesh->vbo->occupied;
            }

            mesh->vbo->occupied += n;
            mesh->capacity += n;
        } else {
            log_fail(stderr, __FILE__, __LINE__, "failed to allocate attributes\n");
            log_assert( resized_n == 0 );
        }

        return resized_n;
    } else {
        log_warn(stderr, __FILE__, __LINE__, "trying to allocate space for a mesh that is not in last position in vbo\n");
    }

    return 0;
}

size_t vbomesh_alloc_indices(struct VboMesh* mesh, size_t n) {
    log_assert( mesh != NULL );
    log_assert( mesh->indices->id > 0 );
    log_assert( n > 0 );

    size_t size_bytes = mesh->indices->capacity * mesh->index.bytes;
    size_t alloc_bytes = n * mesh->index.bytes;

    log_assert( size_bytes + alloc_bytes < PTRDIFF_MAX );
    size_t resized_bytes = ogl_buffer_resize(&mesh->indices->id, size_bytes, size_bytes + alloc_bytes);

    // - we could return resized_bytes, but all other alloc functions return the number
    // of elements allocated, so we just do the same here
    if( resized_bytes == alloc_bytes ) {
        mesh->indices->capacity += n;
        return n;
    } else {
        log_fail(stderr, __FILE__, __LINE__, "failed to allocate indices\n");
        log_assert( resized_bytes == 0 );
    }

    return 0;
}

void vbomesh_clear_attributes(struct VboMesh* mesh) {
    log_assert( mesh != NULL );

    for( int32_t i = 0; i < NUM_SHADER_ATTRIBUTES; i++ ) {
        mesh->occupied[i] = 0;
    }
}

void vbomesh_clear_indices(struct VboMesh* mesh) {
    log_assert( mesh != NULL );

    for( int32_t i = 0; i < NUM_VBO_PHASES; i++ ) {
        mesh->_internal_indices[i].occupied = 0;
    }
}

size_t vbomesh_append_attributes(struct VboMesh* mesh, int32_t i, void* data, uint32_t components_size, GLenum components_type, size_t n) {
    log_assert( mesh != NULL );
    log_assert( mesh->vbo != NULL );
    log_assert( i >= 0 );
    log_assert( i < NUM_SHADER_ATTRIBUTES );
    log_assert( n > 0 );
    log_assert( mesh->vbo->buffer[i].id > 0,
                "mesh->vbo->buffer[i].id(%d) > 0: most likely cause is not calling vbo_add_buffer for the attribute %d before appending attributes\n",
                mesh->vbo->buffer[i].id, i);
    log_assert( components_size == mesh->vbo->components[i].size,
                "components_size(%d) == mesh->vbo->components[i].size(%d): the component size of the data appended to the vbo (components_size) does not fit the components size stored in the vbo (mesh->vbo->components[i].size)\n",
                components_size, mesh->vbo->components[i].size, i);
    log_assert( n % mesh->vbo->components[i].size == 0,
                "n(%lu) %% mesh->vbo->components[i].size(%d) == 0(%d): the appended data size (n) is not a multiple of the components size stored in the vbo (mesh->vbo->components[i].size)\n",
                n, mesh->vbo->components[i].size, n % mesh->vbo->components[i].size, i);
    log_assert( components_type == mesh->vbo->components[i].type,
                "components_type(%d) == mesh->vbo->components[i].type(%d): the component type of the data appended to the vbo (components_type) does not fit the components type stored in the vbo (mesh->vbo->components[i].type)\n",
                components_type, mesh->vbo->components[i].type, i)

    // only these depend on given size params => generic data append
    size_t attrib_bytes = components_size * ogl_sizeof_type(components_type);
    size_t append_bytes = n * attrib_bytes;

    // stuff that relies on vbo size values
    size_t capacity_bytes = mesh->capacity * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
    size_t occupied_bytes = mesh->occupied[i] * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
    size_t offset_bytes = (mesh->offset + mesh->occupied[i]) * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;

    log_assert( offset_bytes < INTPTR_MAX );
    log_assert( append_bytes < INTPTR_MAX );
    if( occupied_bytes + append_bytes <= capacity_bytes ) {
        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
                   glBufferSubData(GL_ARRAY_BUFFER, (intptr_t)offset_bytes, (intptr_t)append_bytes, data);
                   glBindBuffer(GL_ARRAY_BUFFER, 0); );


        mesh->occupied[i] += n;
        return n;
    } else if(// only when this mesh is the last mesh in the vbo can we append without overwriting
              // other meshes, we check if we are last buy checking if our offset + capacity is equal
              // to the vbo occupied counter
              vbomesh_test_last(mesh) &&
              // this does not work genericly, so we just do not allocate anything at all,
              // if num and type do not fit the stored values in vbo
              mesh->vbo->components[i].size == components_size &&
              mesh->vbo->components[i].type == components_type )
    {
        if( vbo_available_capacity(mesh->vbo) < n ) {
            size_t result = vbomesh_alloc_attributes(mesh,n);
            log_assert( result == n );
        }

        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
                   glBufferSubData(GL_ARRAY_BUFFER, (intptr_t)offset_bytes, (intptr_t)append_bytes, data);
                   glBindBuffer(GL_ARRAY_BUFFER, 0); );

        mesh->occupied[i] += n;
        return n;
    } else {
        log_warn(stderr, __FILE__, __LINE__, "could not grow a mesh that is not in last position in vbo when trying to append\n");
    }

    return 0;
}

size_t vbomesh_append_indices(struct VboMesh* mesh, void* data, size_t n) {
    log_assert( mesh != NULL );
    log_assert( mesh->indices->id > 0 );
    log_assert( n > 0 );

    if( mesh->indices->occupied + n > mesh->indices->capacity ) {
        size_t alloc = mesh->indices->occupied + n - mesh->indices->capacity;
        size_t result = vbomesh_alloc_indices(mesh, alloc);
        log_assert( result == alloc );
    }

    log_assert( mesh->indices->occupied + n <= mesh->indices->capacity );

    size_t append_bytes = n * mesh->index.bytes;
    size_t offset_bytes = mesh->indices->occupied * mesh->index.bytes;
    log_assert( offset_bytes < INTPTR_MAX );
    log_assert( append_bytes < INTPTR_MAX );
    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->indices->id);
               glBufferSubData(GL_ARRAY_BUFFER, (intptr_t)offset_bytes, (intptr_t)append_bytes, data);
               glBindBuffer(GL_ARRAY_BUFFER, 0); );

    mesh->indices->occupied += n;
    return n;
}

void* vbomesh_map(struct VboMesh* mesh, size_t offset, size_t length, GLbitfield access) {
    log_assert( mesh != NULL );
    log_assert( mesh->indices->id > 0 );
    log_assert( offset < mesh->indices->capacity );

    if( offset + length > mesh->indices->capacity ) {
        size_t alloc = offset + length - mesh->indices->capacity + 1;
        size_t result = vbomesh_alloc_indices(mesh, alloc);
        log_assert( result == alloc );
    }

    log_assert( offset + length <= mesh->indices->capacity );

    size_t offset_bytes = offset * mesh->index.bytes;
    size_t length_bytes = length * mesh->index.bytes;
    if( length <= offset ) {
        length_bytes = mesh->indices->capacity * mesh->index.bytes;
    }
    log_assert( offset_bytes < INTPTR_MAX );
    log_assert( length_bytes < INTPTR_MAX );

    void* pointer = NULL;
    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->indices->id);
               pointer = glMapBufferRange(GL_ARRAY_BUFFER, (intptr_t)offset_bytes, (intptr_t)length_bytes, access);
               glBindBuffer(GL_ARRAY_BUFFER, 0); );

    return pointer;
}

GLboolean vbomesh_unmap(struct VboMesh* mesh) {
    log_assert( mesh != NULL );
    log_assert( mesh->indices->id > 0 );

    GLboolean result = 0;
    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->indices->id);
               result = glUnmapBuffer(GL_ARRAY_BUFFER);
               glBindBuffer(GL_ARRAY_BUFFER, 0); );

    return result;
}
