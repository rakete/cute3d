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

#include "geometry.h"

int init_geometry() {
    const char* gl_version = (const char*)glGetString(GL_VERSION); 

    printf("%s\n", gl_version);

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

    return ret;
}

GLsizei buffer_resize(GLuint* buffer, GLsizei old_bytes, GLsizei new_bytes) {
    GLuint new_buffer;
    GLuint old_buffer = *buffer;

    if( new_bytes > old_bytes ) {

        glGenBuffers(1, &new_buffer);
        glBindBuffer(GL_COPY_WRITE_BUFFER, new_buffer);
        glBufferData(GL_COPY_WRITE_BUFFER, new_bytes, NULL, GL_STATIC_COPY);
       
        if( old_bytes > 0 && old_buffer > 0 ) {
            glBindBuffer(GL_COPY_READ_BUFFER, old_buffer);
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, old_bytes);
        }

        if( old_buffer ) {
            glDeleteBuffers(1, &old_buffer);
        }

        *buffer = new_buffer;

        if( old_bytes > 0 && old_buffer ) { 
            glBindBuffer(GL_COPY_READ_BUFFER, 0);
        }
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

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

void vbo_create(uint32_t alloc_n, struct Vbo* p) {
    for( int i = 0; i < NUM_PHASES; i++ ) {
        for( int j = 0; j < NUM_BUFFERS; j++ ) {
            p->_internal_buffer[i][j].id = 0;
            p->_internal_buffer[i][j].usage = GL_STATIC_DRAW;
        }
    }
    p->buffer = p->_internal_buffer[0];

    for( int i = 0; i < NUM_PHASES; i++ ) {
        for( int j = 0; j < NUM_BUFFERS; j++ ) {
            p->_internal_components[i][j].size = 0;
            p->_internal_components[i][j].type = 0;
            p->_internal_components[i][j].bytes = 0;
        }
    }
    p->components = p->_internal_components[0];

    p->capacity = 0;
    p->reserved = 0;
    p->alloc = alloc_n;

    for( int i = 0; i < NUM_PHASES; i++ ) {
        p->scheduler.fence[i] = 0;
    }
    p->scheduler.phase = 0;
    p->scheduler.type = many_buffer;
    p->scheduler.offset = 0;

}

void dump_vbo(struct Vbo* vbo, FILE* f) {
    fprintf(f, "vbo->capacity: %d\n", vbo->capacity);
    fprintf(f, "vbo->reserved: %d\n", vbo->reserved);
    fprintf(f, "vbo->alloc: %d\n", vbo->alloc);
}

void vbo_add_buffer(struct Vbo* vbo,
                    int i,
                    uint32_t component_n,
                    GLenum component_t,
                    GLenum usage)
{
    if( vbo && i < NUM_BUFFERS ) {
        glGenBuffers(1, &vbo->buffer[i].id);

        vbo->buffer[i].usage = usage;

        vbo->components[i].size = component_n;
        vbo->components[i].type = component_t;
        vbo->components[i].bytes = sizeof_type(component_t);

        uint32_t nbytes = vbo->capacity * component_n * sizeof_type(component_t);
        glBindBuffer(GL_ARRAY_BUFFER, vbo->buffer[i].id);
        glBufferData(GL_ARRAY_BUFFER, nbytes, NULL, usage);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

uint32_t vbo_alloc(struct Vbo* vbo, uint32_t n) {
    if( vbo ) {
        int resized_bytes = 1;
        uint32_t alloc = vbo->alloc;

        while( alloc < n ) {
            alloc += vbo->alloc;
        }
        
        for( int i = 0; i < NUM_BUFFERS && resized_bytes > 0; i++ ) {
            if( vbo->buffer[i].id ) {
                uint32_t new_bytes = (vbo->capacity + alloc) * vbo->components[i].size * vbo->components[i].bytes;
                uint32_t old_bytes = vbo->capacity * vbo->components[i].size * vbo->components[i].bytes;

                resized_bytes = buffer_resize(&vbo->buffer[i].id, old_bytes, new_bytes);
            }
        }
        
        if( resized_bytes > 0 ) {
            vbo->capacity += alloc;
        }

        return resized_bytes;
    }
    
    return 0;
}

uint32_t vbo_free_elements(struct Vbo* vbo) {
    uint32_t freespace = 0;
    if( vbo ) {
        freespace = vbo->capacity - vbo->reserved;
    }
    return freespace;
}

uint32_t vbo_free_bytes(struct Vbo* vbo, int i) {
    uint32_t freespace = 0;
    if( vbo && vbo->buffer[i].id ) {
        freespace = vbo_free_elements(vbo) * vbo->components[i].size * vbo->components[i].bytes;
    }
    return 0;
}

void vbo_bind(struct Vbo* vbo, int i, GLenum bind_type) {
    if( vbo && i < NUM_BUFFERS && vbo->buffer[i].id ) {
        glBindBuffer(bind_type, vbo->buffer[i].id);
    }
}

void vbo_fill_value(struct Vbo* vbo, int i, uint32_t offset_n, uint32_t size_n, float value) {
    if( vbo && vbo->buffer[i].id && offset_n < vbo->capacity && size_n <= vbo->capacity ) {
        void* array = malloc( sizeof_type(vbo->components[i].type) * size_n );
        uint32_t array_offset = offset_n * vbo->components[i].size;
        uint32_t array_size = size_n * vbo->components[i].size;
        
        switch(vbo->components[i].type) {
            case GL_FLOAT: {
                memset(array, value, array_size);
            }
            case GL_INT: {
                memset(array, (GLint)ceil(value), array_size);
            }
            case GL_UNSIGNED_INT: {
                if( value < 0.0 ) {
                    memset(array, 0.0, array_size);
                } else {
                    memset(array, (GLuint)ceil(value), array_size);
                }
            }
            case GL_SHORT: {
            }
            case GL_UNSIGNED_SHORT: {
            }
            case GL_BYTE: {
            }
            case GL_UNSIGNED_BYTE: {
            }
        }

        vbo_bind(vbo, i, GL_ARRAY_BUFFER);
        glBufferSubData(GL_ARRAY_BUFFER, array_offset, array_size, array);
    }
}

void mesh_create(struct Vbo* vbo, GLenum primitive_type, GLenum index_type, GLenum usage, struct Mesh* p) {
    if( vbo ) {
        p->vbo = vbo;

        p->offset = vbo->reserved;
        p->size = 0;

        for( int i = 0; i < NUM_BUFFERS; i++ ) {
            p->uses[i] = 0;
        }

        p->faces.primitive = primitive_type;
        p->faces.size = sizeof_primitive(primitive_type);
        
        p->index.type = index_type;
        p->index.bytes = sizeof_type(index_type);

        for( int i = 0; i < NUM_PHASES; i++ ) {
            p->_internal_buffer[i].id = 0;
            p->_internal_buffer[i].usage = usage;
        }
        p->buffer = &p->_internal_buffer[0];

        for( int i = 0; i < NUM_PHASES; i++ ) {
            p->_internal_elements[i].size = 0;
            p->_internal_elements[i].used = 0;
            p->_internal_elements[i].alloc = vbo->alloc;
        }
        p->elements = &p->_internal_elements[0];


        glGenBuffers(1, &p->buffer->id);
        glBindBuffer(GL_ARRAY_BUFFER, p->buffer->id);
        uint32_t nbytes = p->faces.size * p->index.bytes;
        glBufferData(GL_ARRAY_BUFFER, nbytes, NULL, usage);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        p->garbage = 0;
    }
}

void dump_mesh(struct Mesh* mesh, FILE* f) {
    dump_vbo(mesh->vbo, f);

    fprintf(f, "\n");
    
    fprintf(f, "mesh->offset: %d\n", mesh->offset);
    fprintf(f, "mesh->size: %d\n", mesh->size);
}

void mesh_patches(struct Mesh* mesh, uint32_t patches_size) {
    mesh->faces.primitive = GL_PATCHES;
    mesh->faces.size = patches_size;
}

uint32_t mesh_alloc(struct Mesh* mesh, uint32_t n) {
    if( mesh && mesh->offset + mesh->size == mesh->vbo->reserved ) {
        if( vbo_free_elements(mesh->vbo) < n ) {
            vbo_alloc(mesh->vbo, n);
        }
        
        if( vbo_free_elements(mesh->vbo) >= n ) {
            mesh->vbo->reserved += n;
            mesh->size += n;

            return n;
        }
    }

    return 0;
}

void mesh_append(struct Mesh* mesh, int i, void* data, uint32_t n) {
    mesh_append_generic(mesh, i, data, n, mesh->vbo->components[i].size, mesh->vbo->components[i].type);
}

void mesh_append_generic(struct Mesh* mesh, int i, void* data, uint32_t n, uint32_t components_size, GLenum components_type) {
    if( mesh && mesh->vbo->buffer[i].id ) {
        // only these depend on given size params => generic data append
        uint32_t attrib_bytes = components_size * sizeof_type(components_type);
        uint32_t append_bytes = n * attrib_bytes;

        // stuff that relies on vbo size values
        uint32_t size_bytes = mesh->size * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
        uint32_t used_bytes = mesh->uses[i] * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
        uint32_t offset_bytes = (mesh->offset + mesh->uses[i]) * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;

        if( used_bytes + append_bytes <= size_bytes ) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
            glBufferSubData(GL_ARRAY_BUFFER, offset_bytes, append_bytes, data);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            mesh->uses[i] += n;
        } else if( mesh->offset + mesh->size == mesh->vbo->reserved &&
                   mesh->vbo->components[i].size == components_size &&
                   mesh->vbo->components[i].type == components_type )
        {
            // this does not work genericly, so we just do not allocate anything at all,
            // if num and type do not fit the stored values in vbo
            mesh_alloc(mesh,n);

            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
            glBufferSubData(GL_ARRAY_BUFFER, offset_bytes, append_bytes, data);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            mesh->uses[i] += n;
        }
    }
}

void* mesh_map(struct Mesh* mesh, int i, GLbitfield access) {
    if( mesh && mesh->vbo->buffer[i].id ) {
        uint32_t offset_bytes = mesh->offset * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
        uint32_t length_bytes = mesh->size * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
        
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
        void* pointer = glMapBufferRange(GL_ARRAY_BUFFER, offset_bytes, length_bytes, access);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return pointer;
    }

    return NULL;
}

GLboolean mesh_unmap(struct Mesh* mesh, int i) {
    if( mesh && mesh->vbo->buffer[i].id ) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
        GLboolean result = glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return result;
    }
    
    return 0;
}

void* mesh_map_elements(struct Mesh* mesh, GLbitfield access) {
    if( mesh && mesh->vbo->buffer[i].id ) {
        uint32_t offset_bytes = mesh->offset * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
        uint32_t length_bytes = mesh->size * mesh->vbo->components[i].size * mesh->vbo->components[i].bytes;
        
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
        void* pointer = glMapBufferRange(GL_ARRAY_BUFFER, offset_bytes, length_bytes, access);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return pointer;
    }

    return NULL;
}

void mesh_triangle(struct Mesh* mesh, GLuint a, GLuint b, GLuint c) {
    if( mesh && mesh->buffer->id ) {
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
            printf("ERROR: this mesh->index.type not implemented in mesh_triangle\n");
        }

        uint32_t size_bytes = mesh->elements->size * mesh->index.bytes;

        if( mesh->elements->used + 1 > mesh->elements->size ) {
            uint32_t alloc_bytes = mesh->elements->alloc * mesh->index.bytes;
            uint32_t resized_bytes = buffer_resize(&mesh->buffer->id, size_bytes, size_bytes + alloc_bytes);
            if( resized_bytes == alloc_bytes ) {
                mesh->elements->size += mesh->elements->alloc;
            }
        }

        uint32_t triangle_bytes = mesh->faces.size * mesh->index.bytes;
        uint32_t offset_bytes = mesh->elements->used * mesh->index.bytes;
        if( mesh->elements->used + 1 <= mesh->elements->size ) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->buffer->id);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset_bytes, triangle_bytes, data);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            mesh->elements->used += 1;
        }

        free(data);
    }
}

void mesh_faces(struct Mesh* mesh, void* data, uint32_t n) {
    if( mesh && mesh->buffer->id ) {
        uint32_t size_bytes = mesh->elements->size * mesh->index.bytes;

        if( mesh->elements->used + n > mesh->elements->size ) {
            uint32_t alloc = 0;
            while( alloc < n ) {
                alloc += mesh->elements->alloc;
            }
            
            uint32_t alloc_bytes = alloc * mesh->index.bytes;
            
            uint32_t resized_bytes = buffer_resize(&mesh->buffer->id, size_bytes, size_bytes + alloc_bytes);
            if( resized_bytes > 0 ) {
                mesh->elements->size += alloc;
            }
        }
            
        uint32_t append_bytes = n * mesh->index.bytes;
        uint32_t offset_bytes = mesh->elements->used * mesh->index.bytes;
        if( mesh->elements->used + n <= mesh->elements->size ) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->buffer->id);
            glBufferSubData(GL_ARRAY_BUFFER, offset_bytes, append_bytes, data);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            mesh->elements->used += n;
        }
    }
}
