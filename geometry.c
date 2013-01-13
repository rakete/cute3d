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

void vbo_create(uint32_t capacity_n, uint32_t alloc_n, struct vbo* p) {
    for( int i = 0; i < NUM_BUFFERS; i++ ) {
        p->buffer[i].id = 0;
        p->buffer[i].usage = GL_STATIC_DRAW;

        p->components[i].num = 0;
        p->components[i].type = 0;
        p->components[i].bytes = 0;
    }

    p->capacity = capacity_n;
    p->reserved = 0;
    p->alloc = alloc_n;
}

void dump_vbo(struct vbo* vbo, FILE* f) {
    fprintf(f, "vbo->capacity: %d\n", vbo->capacity);
    fprintf(f, "vbo->reserved: %d\n", vbo->reserved);
    fprintf(f, "vbo->alloc: %d\n", vbo->alloc);
}

void vbo_add_buffer(struct vbo* vbo,
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
    }
}

uint32_t vbo_alloc(struct vbo* vbo, uint32_t n) {
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

uint32_t vbo_free_elements(struct vbo* vbo) {
    uint32_t freespace = 0;
    if( vbo ) {
        freespace = vbo->capacity - vbo->reserved;
    }
    return freespace;
}

uint32_t vbo_free_bytes(struct vbo* vbo, int i) {
    uint32_t freespace = 0;
    if( vbo && vbo->buffer[i].id ) {
        freespace = vbo_free_elements(vbo) * vbo->components[i].num * vbo->components[i].bytes;
    }
    return 0;
}

void vbo_bind(struct vbo* vbo, int i, GLenum bind_type) {
    if( vbo && i < NUM_BUFFERS && vbo->buffer[i].id ) {
        glBindBuffer(bind_type, vbo->buffer[i].id);
    }
}

void vbo_fill_value(struct vbo* vbo, int i, uint32_t offset_n, uint32_t size_n, float value) {
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

void mesh_create(struct vbo* vbo, GLenum primitive_type, GLenum indices_type, struct mesh* p) {
    if( vbo ) {
        p->vbo = vbo;

        p->offset = vbo->reserved;
        p->size = 0;

        for( int i = 0; i < NUM_BUFFERS; i++ ) {
            p->buffer[i].used = 0;
        }

        p->indices.primitive = primitive_type;
        p->indices.num = sizeof_primitive(primitive_type);
        p->indices.type = indices_type;
        p->indices.bytes = sizeof_type(indices_type);
    
        p->elements.buffer = 0;
        p->elements.size = 0;
        p->elements.used = 0;
        p->elements.alloc = vbo->alloc;

        p->garbage = 0;
    }
}

void dump_mesh(struct mesh* mesh, FILE* f) {
    dump_vbo(mesh->vbo, f);

    fprintf(f, "\n");
    
    fprintf(f, "mesh->offset: %d\n", mesh->offset);
    fprintf(f, "mesh->size: %d\n", mesh->size);
}

void mesh_patches(struct mesh* mesh, uint32_t patches_size) {
    mesh->indices.primitive = GL_PATCHES;
    mesh->indices.num = patches_size;
}

uint32_t mesh_alloc(struct mesh* mesh, uint32_t n) {
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

uint32_t mesh_free_elements(struct mesh* mesh, int i) {
    return( mesh->size - mesh->buffer[i].used );
}

uint32_t mesh_free_bytes(struct mesh* mesh, int i) {
}

void mesh_append(struct mesh* mesh, int i, void* data, uint32_t n) {
    if( mesh && mesh->vbo->buffer[i].id ) {
        uint32_t append_bytes = n * mesh->vbo->components[i].num * mesh->vbo->components[i].bytes;
        uint32_t offset_bytes = (mesh->offset + mesh->buffer[i].used) * mesh->vbo->components[i].num * mesh->vbo->components[i].bytes;

        if( mesh_free_elements(mesh,i) >= n ) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
            glBufferSubData(GL_ARRAY_BUFFER, offset_bytes, append_bytes, data);

            mesh->buffer[i].used += n;
        } else if( mesh->offset + mesh->size == mesh->vbo->reserved ) {
            mesh_alloc(mesh,n);

            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo->buffer[i].id);
            glBufferSubData(GL_ARRAY_BUFFER, offset_bytes, append_bytes, data);

            mesh->buffer[i].used += n;
        }
    }
}

void mesh_triangle(struct mesh* mesh, GLuint a, GLuint b, GLuint c) {
    GLuint data[3] =  { a, b, c };

    uint32_t newsize = mesh->size + 3;
    uint32_t size_bytes = mesh->size * mesh->indices.num * sizeof_type(mesh->indices.type);
    uint32_t append_bytes = 3 * sizeof_type(mesh->indices.type);

    if( (mesh->elements.used + 3) > mesh->elements.size ) {
        uint32_t resized_bytes = buffer_resize(&mesh->elements.buffer, size_bytes, size_bytes + append_bytes);
        if( resized_bytes > 0 ) {
            mesh->elements.size += newsize;
        }
    }

    if( mesh->elements.size >= newsize ) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->elements.buffer);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, mesh->elements.used, append_bytes, data);
    }
}

struct mesh* mesh_clone(struct mesh* mesh) {
    /* struct mesh* clone; */

    /* if( mesh && mesh->vbo->meshes_num < NUM_MESHES ) { */
    /*     clone = malloc( sizeof(struct mesh) ); */

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

/* void mesh_quad(struct mesh* mesh, GLuint a, GLuint b, GLuint c, GLuint d) { */
/* } */

/* void mesh_destroy(struct mesh* mesh) { */
/* } */

/* struct mesh* mesh_union(struct mesh* a, struct mesh* b) { */
/* } */

/* struct mesh* mesh_copy(struct mesh* mesh, struct vbo* to_vbo, uint32_t to_offset) { */
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

/*     struct mesh* triangle_mesh = mesh_create(vbo, 3); */
/*     mesh_append(triangle_mesh, vertex_array, vertices, 9); */
/*     mesh_append(triangle_mesh, color_array, colors, 12); */
/*     mesh_triangle(triangle_mesh, 0, 1, 2); */

/*     float more_vertices[3] = { -1.5, 1.0, 0.0 }; */
/*     short more_colors[4] = { 255, 0, 0, 255 }; */

/*     struct mesh* quad_mesh = triangle_mesh; */
/*     if( ! mesh_freespace(triangle_mesh) ) { */
/*         quad_mesh = mesh_clone(triangle_mesh); */
/*     } */
/*     mesh_append(quad_mesh, vertex_array, more_vertices, 3); */
/*     mesh_append(quad_mesh, color_array, more_colors, 4); */
/*     mesh_triangle(quad_mesh, 3, 0, 2); */
    
/*     return 0; */
/* } */


