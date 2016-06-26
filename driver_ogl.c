#include "driver_ogl.h"

void ogl_error_print(GLenum error) {
    switch(error) {
        case GL_NO_ERROR: break;
        case GL_INVALID_ENUM:
            printf("glGetError: GL_INVALID_ENUM\n");
            break;
        case GL_INVALID_VALUE:
            printf("glGetError: GL_INVALID_VALUE\n");
            break;
        case GL_INVALID_OPERATION:
            printf("glGetError: GL_INVALID_OPERATION\n");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            printf("glGetError: GL_INVALID_FRAMEBUFFER_OPERATION\n");
            break;
        case GL_OUT_OF_MEMORY:
            printf("glGetError: GL_OUT_OF_MEMORY\n");
            break;
        default: break;
    }
}

// a helper function to resize a vbo, it takes a buffer argument and the old size
// of the buffer and the new size it should be resized to, returns either the amount
// of bytes that were added to the buffer size or 0 if nothing was resized
size_t ogl_buffer_resize(GLuint* buffer, size_t old_bytes, size_t new_bytes) {
    log_assert( buffer != NULL );
    log_assert( old_bytes < PTRDIFF_MAX );
    log_assert( new_bytes < PTRDIFF_MAX );
    log_assert( new_bytes > old_bytes );

    GLuint new_buffer;
    GLuint old_buffer = *buffer;

    ogl_debug( glGenBuffers(1, &new_buffer);
               glBindBuffer(GL_COPY_WRITE_BUFFER, new_buffer);
               glBufferData(GL_COPY_WRITE_BUFFER, (ptrdiff_t)new_bytes, NULL, GL_STATIC_COPY); );

    if( old_bytes > 0 && old_buffer > 0 ) {
        ogl_debug( glBindBuffer(GL_COPY_READ_BUFFER, old_buffer);
                   glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, (ptrdiff_t)old_bytes); );
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

size_t ogl_sizeof_type(GLenum type) {
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
}

size_t ogl_sizeof_primitive(GLenum primitive) {
    switch(primitive) {
        case GL_POINTS: return 1;
        case GL_LINES: return 2;
        case GL_TRIANGLES: return 3;
        default: return 0;
    }
}
