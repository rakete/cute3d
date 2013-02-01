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
        glBindBuffer(GL_COPY_READ_BUFFER, old_buffer);
 
        glGenBuffers(1, &new_buffer);
        glBindBuffer(GL_COPY_WRITE_BUFFER, new_buffer);
        glBufferData(GL_COPY_WRITE_BUFFER, new_bytes, NULL, GL_STATIC_COPY);
 
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, old_bytes);

        glDeleteBuffers(1, &old_buffer);

        *buffer = new_buffer;

        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
        glBindBuffer(GL_COPY_READ_BUFFER, 0);

        return new_bytes;
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
    for( int i = 0; i < NUM_BUFFERS; i++ ) {
        p->buffer[i].id = 0;
        p->buffer[i].usage = GL_STATIC_DRAW;

        p->components[i].num = 0;
        p->components[i].type = 0;
        p->components[i].bytes = 0;
    }

    p->capacity = 0;
    p->reserved = 0;
    p->alloc = alloc_n;
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

        vbo->components[i].num = component_n;
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
                uint32_t new_bytes = (vbo->capacity + alloc) * vbo->components[i].num * vbo->components[i].bytes;
                uint32_t old_bytes = vbo->capacity * vbo->components[i].num * vbo->components[i].bytes;

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
        freespace = vbo_free_elements(vbo) * vbo->components[i].num * vbo->components[i].bytes;
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
        uint32_t array_offset = offset_n * vbo->components[i].num;
        uint32_t array_size = size_n * vbo->components[i].num;
        
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
            p->buffer[i].used = 0;
        }

        p->faces.primitive = primitive_type;
        p->faces.size = sizeof_primitive(primitive_type);
        
        p->index.type = index_type;
        p->index.bytes = sizeof_type(index_type);
        
        p->elements.buffer = 0;
        p->elements.size = 0;
        p->elements.used = 0;
        p->elements.alloc = vbo->alloc;
        p->elements.usage = usage;

        glGenBuffers(1, &p->elements.buffer);
        glBindBuffer(GL_ARRAY_BUFFER, p->elements.buffer);
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

uint32_t mesh_free_elements(struct Mesh* mesh, int i) {
    return( mesh->size - mesh->buffer[i].used );
}

uint32_t mesh_free_bytes(struct Mesh* mesh, int i) {
}

void mesh_append(struct Mesh* mesh, int i, void* data, uint32_t n) {
    mesh_append_generic(mesh, i, data, n, mesh->vbo->components[i].num, mesh->vbo->components[i].type);
}

void mesh_append_generic(struct Mesh* mesh, int i, void* data, uint32_t n, uint32_t components_num, GLenum components_type) {
    if( mesh && mesh->vbo->buffer[i].id ) {
        // only these depend on given size params => generic data append
        uint32_t attrib_size = components_num * sizeof_type(components_type);
        uint32_t append_bytes = n * attrib_size;

        // stuff that relies on vbo size values
        uint32_t size_bytes = mesh->size * mesh->vbo->components[i].num * mesh->vbo->components[i].bytes;
        uint32_t used_bytes = mesh->buffer[i].used * mesh->vbo->components[i].num * mesh->vbo->components[i].bytes;
        uint32_t offset_bytes = (mesh->offset + mesh->buffer[i].used) * mesh->vbo->components[i].num * mesh->vbo->components[i].bytes;

        if( used_bytes + append_bytes <= size_bytes ) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
            glBufferSubData(GL_ARRAY_BUFFER, offset_bytes, append_bytes, data);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            mesh->buffer[i].used += n;
        } else if( mesh->offset + mesh->size == mesh->vbo->reserved &&
                   mesh->vbo->components[i].num == components_num &&
                   mesh->vbo->components[i].type == components_type )
        {
            // this does not work genericly, so we just not allocate anything at
            // if num and type do not fit the stored values in vbo
            mesh_alloc(mesh,n);

            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
            glBufferSubData(GL_ARRAY_BUFFER, offset_bytes, append_bytes, data);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            mesh->buffer[i].used += n;
        }
    }
}

void mesh_triangle(struct Mesh* mesh, GLuint a, GLuint b, GLuint c) {
    if( mesh && mesh->elements.buffer ) {
        void* data = malloc(3 * mesh->index.bytes);

        if( mesh->index.type == GL_UNSIGNED_INT ) {
            ((GLuint*)data)[0] = a;
            ((GLuint*)data)[1] = b;
            ((GLuint*)data)[2] = c;
        } else if( mesh->index.type == GL_UNSIGNED_SHORT ) {
            ((GLushort*)data)[0] = (GLushort)a;
            ((GLushort*)data)[1] = (GLushort)b;
            ((GLushort*)data)[2] = (GLushort)c;
        }

        uint32_t triangle_bytes = mesh->faces.size * mesh->index.bytes;
        uint32_t size_bytes = mesh->size * triangle_bytes;

        if( mesh->elements.used + 1 > mesh->elements.size ) {
            uint32_t alloc_bytes = mesh->elements.alloc * triangle_bytes;
            uint32_t resized_bytes = buffer_resize(&mesh->elements.buffer, size_bytes, size_bytes + alloc_bytes);
            if( resized_bytes > 0 ) {
                mesh->elements.size += mesh->elements.alloc;
            }
        }

        uint32_t offset_bytes = mesh->elements.used * triangle_bytes;
        if( mesh->elements.used + 1 <= mesh->elements.size ) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->elements.buffer);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset_bytes, triangle_bytes, data);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            mesh->elements.used += 1;
        }

        free(data);
    }
}

void mesh_faces(struct Mesh* mesh, void* data, uint32_t n) {
    if( mesh && mesh->elements.buffer ) {
        uint32_t triangle_bytes = mesh->faces.size * mesh->index.bytes;
        uint32_t size_bytes = mesh->size * triangle_bytes;

        if( mesh->elements.used + n > mesh->elements.size ) {
            uint32_t alloc = 0;
            while( alloc < n ) {
                alloc += mesh->elements.alloc;
            }
            
            uint32_t alloc_bytes = alloc * triangle_bytes;
            
            uint32_t resized_bytes = buffer_resize(&mesh->elements.buffer, size_bytes, size_bytes + alloc_bytes);
            if( resized_bytes > 0 ) {
                mesh->elements.size += alloc;
            }
        }
            
        uint32_t append_bytes = n * triangle_bytes;
        uint32_t offset_bytes = mesh->elements.used * triangle_bytes;
        if( mesh->elements.used + n <= mesh->elements.size ) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->elements.buffer);
            glBufferSubData(GL_ARRAY_BUFFER, offset_bytes, append_bytes, data);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            mesh->elements.used += n;
        }
    }    
}

struct Mesh* mesh_clone(struct Mesh* mesh) {
    /* struct Mesh* clone; */

    /* if( mesh && mesh->vbo->meshes_num < NUM_MESHES ) { */
    /*     clone = malloc( sizeof(struct Mesh) ); */

    /*     clone->vbo = mesh->vbo; */
    /*     if( mesh->size > vbo_free_elements(mesh->vbo) ) { */
    /*         // resize buffer */
    /*         // copy data */
    /*     } else { */
    /*         for( int i = 0; i < NUM_BUFFERS; i++ ) { */
    /*             if( mesh->vbo->active[i] ) { */
    /*                 vbo_bind(mesh->vbo, i, GL_COPY_READ_BUFFER); */
    /*                 vbo_bind(mesh->vbo, i, GL_COPY_WRITE_BUFFER); */
    /*                 uint32_t offset_bytes = mesh->offset * mesh->vbo->components[i].num * mesh->vbo->components[i].bytes; */
    /*                 uint32_t used_bytes = mesh->used * mesh->vbo->components[i].num * mesh->vbo->components[i].bytes; */
    /*                 uint32_t reserved_bytes = mesh->vbo->reserved * mesh->vbo->components[i].num * mesh->vbo->components[i].bytes; */
    /*                 glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, offset_bytes, reserved_bytes, used_bytes); */
    /*             } */
    /*         } */
    /*     } */
        
    /*     clone->offset = mesh->offset; */
    /*     clone->size = mesh->size; */
    /*     clone->used = mesh->used; */

    /*     clone->indices.primitive = mesh->indices.primitive; */
    /*     clone->indices.num = mesh->indices.num; */
    /*     clone->indices.type = mesh->indices.type; */
    /*     clone->indices.bytes = mesh->indices.bytes; */

    /*     glBindBuffer(GL_COPY_READ_BUFFER, mesh->elements.buffer); */

    /*     glGenBuffers(1, &clone->elements.buffer); */
    /*     glBindBuffer(GL_COPY_WRITE_BUFFER, clone->elements.buffer); */
    /*     glBufferData(GL_COPY_WRITE_BUFFER, mesh->elements.size, NULL, GL_STATIC_COPY); */

    /*     glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, mesh->elements.size); */

    /*     clone->elements.size = mesh->elements.size; */
    /*     clone->elements.used = mesh->elements.used; */
    /*     clone->elements.alloc = mesh->elements.alloc; */

    /*     clone->garbage = 0; */
    /* } */

    /* return clone; */
}

/* void mesh_quad(struct Mesh* mesh, GLuint a, GLuint b, GLuint c, GLuint d) { */
/* } */

/* void mesh_destroy(struct Mesh* mesh) { */
/* } */

/* struct Mesh* mesh_union(struct Mesh* a, struct Mesh* b) { */
/* } */

/* struct Mesh* mesh_copy(struct Mesh* mesh, struct vbo* to_vbo, uint32_t to_offset) { */
/* } */

/* int main(int argc, char *argv[]) { */
/*     struct vbo* vbo = vbo_create(); */
/*     vbo_add_buffer(vbo, vertex_array, 3, GL_FLOAT, 3*3*sizeof(float), 3*3*sizeof(float), GL_STATIC_DRAW); */
/*     vbo_add_buffer(vbo, color_array, 4, GL_SHORT, 3*4*sizeof(short), 3*4*sizeof(short), GL_STATIC_DRAW); */
    
/*     float vertices[24] = { 0.0, 1.0, 0.0, */
/*                            1.0, -1.0, 0.0, */
/*                            -1.0, -1.0, 0.0 }; */
/*     short colors[32] = { 255, 0, 0, 255, */
/*                          255, 0, 0, 255, */
/*                          255, 0, 0, 255 }; */

/*     struct Mesh* triangle_mesh = mesh_create(vbo, 3); */
/*     mesh_append(triangle_mesh, vertex_array, vertices, 9); */
/*     mesh_append(triangle_mesh, color_array, colors, 12); */
/*     mesh_triangle(triangle_mesh, 0, 1, 2); */

/*     float more_vertices[3] = { -1.5, 1.0, 0.0 }; */
/*     short more_colors[4] = { 255, 0, 0, 255 }; */

/*     struct Mesh* quad_mesh = triangle_mesh; */
/*     if( ! mesh_freespace(triangle_mesh) ) { */
/*         quad_mesh = mesh_clone(triangle_mesh); */
/*     } */
/*     mesh_append(quad_mesh, vertex_array, more_vertices, 3); */
/*     mesh_append(quad_mesh, color_array, more_colors, 4); */
/*     mesh_triangle(quad_mesh, 3, 0, 2); */
    
/*     return 0; */
/* } */


