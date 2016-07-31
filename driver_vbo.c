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

#include "driver_vbo.h"

int32_t init_vbo() {
    int32_t ret = 0;

    if( ! SDL_GL_ExtensionSupported("GL_ARB_copy_buffer") &&
        ! SDL_GL_ExtensionSupported("GL_EXT_copy_buffer") )
    {
        log_fail(__FILE__, __LINE__, "copy_buffer extension not found!\n");
        ret = 1;
    }

    if( ! SDL_GL_ExtensionSupported ("GL_ARB_vertex_array_object") &&
        ! SDL_GL_ExtensionSupported("GL_EXT_vertex_array_object") )
    {
        log_fail(__FILE__, __LINE__, "vertex_array_object extension not found!\n");
        ret = 1;
    }

    if( ! SDL_GL_ExtensionSupported("GL_ARB_buffer_storage" ) ) {
        log_fail(__FILE__, __LINE__, "buffer_storage extension not found!\n");
        ret = 1;
    }

#if CUTE_BUILD_ES2
    // - opengl es2 does not have mapping of buffers! that sucks because I use it,
    // so I need to check for this extension
    if( ! SDL_GL_ExtensionSupported("GL_OES_mapbuffer") ) {
        log_fail(__FILE__, __LINE__, "mapbuffer extension not found!\n");
        ret = 1;
    }
#endif

    return ret;
}

void vbo_create(struct Vbo* p) {
    log_assert( p != NULL );

    for( int32_t i = 0; i < MAX_VBO_PHASES; i++ ) {
        for( int32_t j = 0; j < MAX_SHADER_ATTRIBUTES; j++ ) {
            p->_internal_buffer[i][j].id = 0;
            p->_internal_buffer[i][j].usage = GL_STATIC_DRAW;
        }
    }
    p->buffer = p->_internal_buffer[0];

    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        p->components[i].size = 0;
        p->components[i].type = 0;
        p->components[i].bytes = 0;
    }

    p->capacity = 0;
    p->occupied = 0;

    /* for( int32_t i = 0; i < MAX_VBO_PHASES; i++ ) { */
    /*     p->scheduler.fence[i] = 0; */
    /* } */
    /* p->scheduler.phase = 0; */
    /* p->scheduler.type = VBO_MANY_BUFFER; */
    /* p->scheduler.offset = 0; */
}

void vbo_destroy(struct Vbo* p) {
    log_assert( 0 == 1 );
}

void vbo_print(struct Vbo* vbo) {
    log_assert( vbo != NULL );

    for( int32_t i = 0; i < MAX_VBO_PHASES; i++ ) {
        for( int32_t j = 0; j < MAX_SHADER_ATTRIBUTES; j++ ) {
            printf("vbo->_internal_buffer[%d][%d].id: %u\n", i, j, vbo->_internal_buffer[i][j].id);
            printf("vbo->_internal_buffer[%d][%d].usage: %d\n", i, j, vbo->_internal_buffer[i][j].usage);
        }
    }
    printf("\n");

    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        printf("vbo->components[%d].size: %u\n", i, vbo->components[i].size);
        printf("vbo->components[%d].type: %d\n", i, vbo->components[i].type);
        printf("vbo->components[%d].bytes: %u\n", i, vbo->components[i].bytes);
    }
    printf("\n");

    printf("vbo->capacity: %zu\n", vbo->capacity);
    printf("vbo->occupied: %zu\n", vbo->occupied);
    printf("\n");

    /* printf("vbo->scheduler.phase: %u\n", vbo->scheduler.phase); */
    /* for( int32_t i = 0; i < MAX_VBO_PHASES; i++ ) { */
    /*     printf("vbo->scheduler.dirty[%d]: %d\n", i, vbo->scheduler.dirty[i]); */
    /* } */
    /* printf("vbo->scheduler.type: %d\n", vbo->scheduler.type); */
    /* printf("vbo->scheduler.offset: %zu\n", vbo->scheduler.offset); */
}

void vbo_add_buffer(struct Vbo* vbo,
                    uint32_t i,
                    uint32_t components_size,
                    GLenum components_type,
                    GLenum usage)
{
    log_assert( vbo != NULL );
    log_assert( i < MAX_SHADER_ATTRIBUTES, "%d %d\n", i, MAX_SHADER_ATTRIBUTES );
    log_assert( components_size > 0 && components_size <= 4,
                "components_size > 0 && components_size(%d) <= 4: the current implementation assumes attributes to have 0-4 components, you have given components_size %d\n",
                components_size, components_size, components_size, components_size);
    log_assert( components_type == GL_FLOAT || components_type == GL_UNSIGNED_BYTE,
                "components_type(%d) == GL_FLOAT(%d) || components_type(%d) == GL_UNSIGNED_BYTE(%d): the current implementation assumes attributes to have either GL_FLOAT or GL_UNSIGNED_BYTE components, you have given components_type %d\n",
                components_type, GL_FLOAT, components_type, GL_UNSIGNED_BYTE, components_type );

    ogl_debug( glGenBuffers(1, &vbo->buffer[i].id) );
    log_assert( vbo->buffer[i].id > 0 );

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
    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
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

void* vbo_map(struct Vbo* vbo, int32_t i, size_t offset, size_t length, GLbitfield access) {
    log_assert( offset < vbo->capacity );
    log_assert( vbo != NULL );
    log_assert( i >= 0 );
    log_assert( vbo->buffer[i].id > 0 );
    log_assert( offset + length <= vbo->capacity );

    size_t offset_bytes = offset * vbo->components[i].size * vbo->components[i].bytes;
    size_t length_bytes = length * vbo->components[i].size * vbo->components[i].bytes;

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

void ibo_create(GLenum primitive_type, GLenum index_type, GLenum usage, struct Ibo* ibo) {
    log_assert( ibo != NULL );

    for( int32_t i = 0; i < MAX_VBO_PHASES; i++ ) {
        ibo->_internal_buffer[i].id = 0;
        ibo->_internal_buffer[i].usage = usage;
    }
    ibo->buffer = &ibo->_internal_buffer[0];

    ibo->primitives.type = primitive_type;
    ibo->primitives.size = (uint32_t)ogl_sizeof_primitive(primitive_type);

    ibo->index.type = index_type;
    ibo->index.bytes = (uint32_t)ogl_sizeof_type(index_type);

#ifndef CUTE_BUILD_ES2
    ibo->base = 0;
#endif
    ibo->capacity = 0;
    ibo->occupied = 0;
}

void ibo_print(struct Ibo* ibo) {
    log_assert( ibo != NULL );

    for( int32_t i = 0; i < MAX_VBO_PHASES; i++ ) {
        printf("ibo->_internal_buffer[%d].id: %d\n", i, ibo->_internal_buffer[i].id);
        printf("ibo->_internal_buffer[%d].usage: %d\n", i, ibo->_internal_buffer[i].usage);
    }

    printf("ibo->primitives.type: %d\n", ibo->primitives.type);
    printf("ibo->primitives.size: %d\n", ibo->primitives.size);

    printf("ibo->index.type: %d\n", ibo->index.type);
    printf("ibo->index.bytes: %d\n", ibo->index.bytes);

    printf("ibo->capacity: %zu\n", ibo->capacity);
    printf("ibo->occupied: %zu\n", ibo->occupied);
}

size_t ibo_alloc(struct Ibo* ibo, size_t n) {
    log_assert( ibo != NULL );
    log_assert( n > 0 );

    if( ! ibo->buffer->id ) {
        ogl_debug( glGenBuffers(1, &ibo->buffer->id) );
    }
    log_assert( ibo->buffer->id > 0 );

    size_t size_bytes = ibo->capacity * ibo->index.bytes;
    size_t alloc_bytes = n * ibo->index.bytes;

    log_assert( size_bytes + alloc_bytes < PTRDIFF_MAX );
    size_t resized_bytes = ogl_buffer_resize(&ibo->buffer->id, size_bytes, size_bytes + alloc_bytes);

    // - we could return resized_bytes, but all other alloc functions return the number
    // of elements allocated, so we just do the same here
    if( resized_bytes == alloc_bytes ) {
        ibo->capacity += n;
        return n;
    } else {
        log_fail(__FILE__, __LINE__, "failed to allocate indices\n");
        log_assert( resized_bytes == 0 );
    }

    return 0;
}

size_t ibo_available_capacity(struct Ibo* ibo) {
    log_assert( ibo != NULL );
    log_assert( ibo->capacity >= ibo->occupied );

    size_t freespace = 0;
    freespace = ibo->capacity - ibo->occupied;

    return freespace;
}

void* ibo_map(struct Ibo* ibo, size_t offset, size_t length, GLbitfield access) {
    log_assert( ibo != NULL );
    log_assert( ibo->buffer->id > 0 );
    log_assert( offset + length <= ibo->capacity );

    size_t offset_bytes = offset * ibo->index.bytes;
    size_t length_bytes = length * ibo->index.bytes;
    log_assert( offset_bytes < INTPTR_MAX );
    log_assert( length_bytes < INTPTR_MAX );

    void* pointer = NULL;
    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, ibo->buffer->id);
               pointer = glMapBufferRange(GL_ARRAY_BUFFER, (intptr_t)offset_bytes, (intptr_t)length_bytes, access);
               glBindBuffer(GL_ARRAY_BUFFER, 0); );

    return pointer;
}

GLboolean ibo_unmap(struct Ibo* ibo) {
    log_assert( ibo != NULL );
    log_assert( ibo->buffer->id > 0 );

    GLboolean result = 0;
    ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, ibo->buffer->id);
               result = glUnmapBuffer(GL_ARRAY_BUFFER);
               glBindBuffer(GL_ARRAY_BUFFER, 0); );

    return result;
}

void vbo_mesh_create(struct Vbo* vbo, struct Ibo* ibo, struct VboMesh* mesh) {
    log_assert( vbo != NULL );
    log_assert( ibo != NULL );
    log_assert( mesh != NULL );

    mesh->vbo = vbo;

    mesh->attributes.offset = vbo->occupied;
    mesh->attributes.capacity = 0;

    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        mesh->attributes.occupied[i] = 0;
    }

    mesh->ibo = ibo;

    mesh->indices.offset = ibo->occupied;
    mesh->indices.capacity = 0;
    mesh->indices.occupied = 0;

    // - the value here is difficult, RAND_MAX/0.0002 - 0.0001 is large enough for not getting
    // z fighting from most angles, with smaller values I observed z fighting
    // - but, the bigger problem is that if I set this too large, and the user creates a camera
    // with a near value smaller or around the same size as this offset, things go awfully pear
    // shaped, what happens is that you get seemingly random depth errors where stuff is drawn in
    // front of other stuff when it shouldn't
    mesh->z_offset = (float)(((double)rand()/(double)(RAND_MAX/0.0002)) - 0.0001);

#ifndef CUTE_BUILD_ES2
    mesh->vao = 0;
#endif
}

void vbo_mesh_print(struct VboMesh* mesh) {
    log_assert( mesh != NULL );
    log_assert( mesh->vbo != NULL );
    log_assert( mesh->ibo != NULL );

    printf("/===========================================\n");

    vbo_print(mesh->vbo);
    printf("\n");

    printf("mesh->attributes.offset: %zu\n", mesh->attributes.offset);
    printf("mesh->attributes.capacity: %zu\n", mesh->attributes.capacity);
    printf("\n");

    for( int32_t i = 0; i < MAX_VBO_PHASES; i++ ) {
        for( int32_t j = 0; j < MAX_SHADER_ATTRIBUTES; j++ ) {
            if( mesh->attributes.occupied[j] > 0 ) {
                printf("mesh->attributes.occupied[%d]: %zu\n", j, mesh->attributes.occupied[j]);
                printf("mesh->vbo->buffer[%d][%d]:\n", i, j);
                switch(mesh->vbo->components[j].type) {
                    case GL_FLOAT: {
                        GLfloat* array = (GLfloat*)vbo_map(mesh->vbo, j, mesh->attributes.offset, mesh->attributes.capacity, GL_MAP_READ_BIT);
                        if( array ) {
                            for( size_t k = 0; k < mesh->attributes.capacity; k++ ) {
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

                                if( k == mesh->attributes.capacity - 1 ) {
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
                    case GL_UNSIGNED_BYTE: {
                        uint8_t* array = (uint8_t*)vbo_map(mesh->vbo, j, mesh->attributes.offset, mesh->attributes.capacity, GL_MAP_READ_BIT);
                        if( array ) {
                            for( size_t k = 0; k < mesh->attributes.capacity; k++ ) {
                                if( mesh->vbo->components[j].size == 3 ) {
                                    uint8_t a = array[k*mesh->vbo->components[j].size+0];
                                    uint8_t b = array[k*mesh->vbo->components[j].size+1];
                                    uint8_t c = array[k*mesh->vbo->components[j].size+2];
                                    printf("[%u %u %u]", a, b, c);
                                } else if( mesh->vbo->components[j].size == 4 ) {
                                    uint8_t a = array[k*mesh->vbo->components[j].size+0];
                                    uint8_t b = array[k*mesh->vbo->components[j].size+1];
                                    uint8_t c = array[k*mesh->vbo->components[j].size+2];
                                    uint8_t d = array[k*mesh->vbo->components[j].size+3];
                                    printf("[%u %u %u %u]", a, b, c, d);
                                }

                                if( k == mesh->attributes.capacity - 1 ) {
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
                    default: {
                        log_fail(__FILE__, __LINE__, "type not implemented in vbo_mesh_print\n");
                        break;
                    }
                }
            }
        }
    }

    printf("\n");
    ibo_print(mesh->ibo);
    printf("\n");

    printf("mesh->indices.offset: %zu\n", mesh->indices.offset);
    printf("mesh->indices.capacity: %zu\n", mesh->indices.capacity);
    printf("mesh->indices.occupied: %zu\n", mesh->indices.occupied);

    if( mesh->indices.occupied > 0 && mesh->ibo->buffer->id > 0 ) {
        printf("mesh->ibo->buffer:\n");
        switch(mesh->ibo->index.type) {
            case GL_UNSIGNED_INT: {
                GLuint* array = (GLuint*)ibo_map(mesh->ibo, mesh->indices.offset, mesh->indices.capacity, GL_MAP_READ_BIT);
                if( array ) {
                    uint32_t primitive_size = mesh->ibo->primitives.size;
                    for( size_t k = 0; k < mesh->indices.capacity; k+=primitive_size ) {
                        printf("[");
                        for( uint32_t l = 0; l < primitive_size; l++ ) {
                            printf("%d", array[k+l]);
                            if( l < primitive_size - 1 ) {
                                printf(", ");
                            }
                        }
                        printf("]");
                        if( k == mesh->indices.capacity - primitive_size ) {
                            printf("\n");
                        } else {
                            printf(", ");
                        }
                    }
                }
                ibo_unmap(mesh->ibo);
                break;
            }
        }
    } else {
        printf("mesh->ibo->buffer: 0\n");
    }

    printf("-------------------------------------------/\n");

    printf("\n");
}

bool vbo_mesh_test_last_attributes(struct VboMesh* mesh) {
    log_assert( mesh != NULL );
    log_assert( mesh->vbo != NULL );

    // - return always true when mesh->capacity == 0, because that means the mesh has never been allocated space
    // and can therefore be moved at the end of the vbo by vbo_mesh_alloc as soon as we use it to actually allocate
    // some space
    // - the part after || is the actual test, when mesh->offset + mesh->capacity equal vbo->occupied no other mesh
    // has been added to the vbo after this one, so we are the last
    return mesh->attributes.capacity == 0 || mesh->attributes.offset + mesh->attributes.capacity == mesh->vbo->occupied;
}

bool vbo_mesh_test_last_indices(struct VboMesh* mesh) {
    log_assert( mesh != NULL );
    log_assert( mesh->ibo != NULL );

    return mesh->indices.capacity == 0 || mesh->indices.offset + mesh->indices.capacity == mesh->ibo->occupied;
}

size_t vbo_mesh_alloc_attributes(struct VboMesh* mesh, size_t n) {
    log_assert( mesh != NULL );
    log_assert( mesh->vbo != NULL );
    log_assert( n > 0 );

    // - only resize if the mesh is the last mesh, otherwise this needs to stay the same size
    if( vbo_mesh_test_last_attributes(mesh) ) {
        // - check available space in vbo, test if it is less then what we would need to
        // expand the mesh
        // - if we need more space we allocate at least one VBO_DEFAULT_ALLOC block, maybe more
        // blocks if we have a really large n, i've implemented this block allocation mainly so
        // that I can avoid resizing buffers in es2 (I could also call vbo_alloc manually)
        size_t resized_n = 0;
        size_t available_n = vbo_available_capacity(mesh->vbo);
        if( available_n < n ) {
            size_t alloc_n = VBO_DEFAULT_ALLOC;
            while( alloc_n < n ) {
                alloc_n += VBO_DEFAULT_ALLOC;
            }
            resized_n = vbo_alloc(mesh->vbo, alloc_n);
            log_assert( resized_n == alloc_n );
        }

        if( vbo_available_capacity(mesh->vbo) >= n ) {
            // - if this mesh has never been touched before (when no space has been allocated for it in the vbo), move
            // the mesh to the last position just after allocating
            if( mesh->attributes.capacity == 0 ) {
                mesh->attributes.offset = mesh->vbo->occupied;
            }

            // - increase the vbo occupied counter, vbo_alloc only increases the vbo->capacity, we have to
            // increase occpied here, indicate that our attributes.capacity increased (which we too do here)
            // - I used resized_n here before, but now I've changed to the block allocation logic above, so
            // resized_n is much too large, so I use n
            mesh->vbo->occupied += n;
            mesh->attributes.capacity += n;
        } else {
            log_fail(__FILE__, __LINE__, "failed to allocate attributes\n");
            log_assert( resized_n == 0 );
        }

        return n;
    } else {
        log_warn(__FILE__, __LINE__, "trying to allocate attributes in a mesh that is not in last position in vbo\n");
    }

    return 0;
}

size_t vbo_mesh_alloc_indices(struct VboMesh* mesh, size_t n) {
    log_assert( mesh != NULL );
    log_assert( mesh->ibo != NULL );
    log_assert( n > 0 );

    if( vbo_mesh_test_last_indices(mesh) ) {
        size_t resized_n = 0;
        size_t available_n = ibo_available_capacity(mesh->ibo);
        if( available_n < n ) {
            size_t alloc_n = IBO_DEFAULT_ALLOC;
            while( alloc_n < n ) {
                alloc_n += IBO_DEFAULT_ALLOC;
            }
            resized_n = ibo_alloc(mesh->ibo, alloc_n);
            log_assert( resized_n == alloc_n );
        }

        if( ibo_available_capacity(mesh->ibo) >= n ) {
            if( mesh->indices.capacity == 0 ) {
                mesh->indices.offset = mesh->ibo->occupied;
            }

            mesh->ibo->occupied += n;
            mesh->indices.capacity += n;
        } else {
            log_fail(__FILE__, __LINE__, "failed to allocate attributes\n");
            log_assert( resized_n == 0 );
        }

        return n;
    } else {
        log_warn(__FILE__, __LINE__, "trying to allocate indices in a mesh that is not in last position in ibo\n");
    }

    return 0;
}

void vbo_mesh_clear_attributes(struct VboMesh* mesh) {
    log_assert( mesh != NULL );

    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        mesh->attributes.occupied[i] = 0;
    }
}

void vbo_mesh_clear_indices(struct VboMesh* mesh) {
    log_assert( mesh != NULL );

    mesh->indices.occupied = 0;
}

size_t vbo_mesh_append_attributes(struct VboMesh* mesh, int32_t i, uint32_t components_size, GLenum components_type, size_t n, void* data) {
    log_assert( mesh != NULL );
    log_assert( mesh->vbo != NULL );
    log_assert( i >= 0 );
    log_assert( i < MAX_SHADER_ATTRIBUTES );
    log_assert( n > 0 );
    log_assert( mesh->vbo->buffer[i].id > 0,
                "mesh->vbo->buffer[i].id(%d) > 0: most likely cause is not calling vbo_add_buffer for the attribute %d before appending attributes\n",
                mesh->vbo->buffer[i].id, i);
    log_assert( components_size == mesh->vbo->components[i].size,
                "components_size(%d) == mesh->vbo->components[i].size(%d): the component size of the data appended to the vbo (components_size) does not fit the components size stored in the vbo (mesh->vbo->components[i].size)\n",
                components_size, mesh->vbo->components[i].size, i);
    log_assert( components_type == mesh->vbo->components[i].type,
                "components_type(%d) == mesh->vbo->components[i].type(%d): the component type of the data appended to the vbo (components_type) does not fit the components type stored in the vbo (mesh->vbo->components[i].type)\n",
                components_type, mesh->vbo->components[i].type, i);

    // - first check if we need to allocate more space, do that if we have to
    // - it is important to remember that vbo_mesh_alloc_attributes changes the state offset/capacity state we rely
    // on later, I had bugs that resulted from that when in the vbo_mesh_append_indices function I relied on
    // mesh->ibo->offset instead on mesh->indices.offset, it made the offset calculation only work correctly if done
    // before calling vbo_mesh_alloc_indices
    if( mesh->attributes.occupied[i] + n > mesh->attributes.capacity ) {
        size_t alloc_n = mesh->attributes.occupied[i] + n - mesh->attributes.capacity;
        size_t result_n = vbo_mesh_alloc_attributes(mesh, alloc_n);
        log_assert( result_n == alloc_n );
    }

    // - now we should have enough space and we can just write the data into the buffer, and then increase the occupied
    // counter also
    if( mesh->attributes.occupied[i] + n <= mesh->attributes.capacity ) {
        size_t offset_bytes = (mesh->attributes.offset + mesh->attributes.occupied[i]) * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
        size_t append_bytes = n * components_size * ogl_sizeof_type(components_type);
        log_assert( offset_bytes < INTPTR_MAX );
        log_assert( append_bytes < INTPTR_MAX );

        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
                   glBufferSubData(GL_ARRAY_BUFFER, (intptr_t)offset_bytes, (intptr_t)append_bytes, data);
                   glBindBuffer(GL_ARRAY_BUFFER, 0); );

        mesh->attributes.occupied[i] += n;
        return n;
    } else {
        log_warn(__FILE__, __LINE__, "could not grow attributes when trying to append to a vbo_mesh\n");
    }

    return 0;
}

size_t vbo_mesh_append_indices(struct VboMesh* mesh, size_t n, void* data) {
    log_assert( mesh != NULL );
    log_assert( mesh->ibo != NULL );
    log_assert( n > 0 );

    if( mesh->indices.occupied + n > mesh->indices.capacity ) {
        size_t alloc_n = mesh->indices.occupied + n - mesh->indices.capacity;
        size_t result_n = vbo_mesh_alloc_indices(mesh, alloc_n);
        log_assert( result_n == alloc_n );
    }

    log_assert( mesh->ibo->buffer->id > 0 );

    if( mesh->indices.occupied + n <= mesh->indices.capacity ) {
        size_t offset_bytes = (mesh->indices.offset + mesh->indices.occupied) * mesh->ibo->index.bytes;
        size_t append_bytes = n * mesh->ibo->index.bytes;
        log_assert( offset_bytes < INTPTR_MAX );
        log_assert( append_bytes < INTPTR_MAX );

        ogl_debug( glBindBuffer(GL_ARRAY_BUFFER, mesh->ibo->buffer->id);
                   glBufferSubData(GL_ARRAY_BUFFER, (intptr_t)offset_bytes, (intptr_t)append_bytes, data);
                   glBindBuffer(GL_ARRAY_BUFFER, 0); );

        mesh->indices.occupied += n;
        return n;
    } else {
        log_warn(__FILE__, __LINE__, "could not grow indices when trying to append to a vbo_mesh\n");
    }

    return 0;
}
