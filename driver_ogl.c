#include "driver_ogl.h"

int32_t init_ogl(int32_t width, int32_t height, const float clear_color[4]) {
    ogl_debug({
            const char* gl_version = (const char*)glGetString(GL_VERSION);
            log_info(stderr, __FILE__, __LINE__, "%s\n", gl_version);

            glViewport(0,0,width,height);

            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glEnable(GL_MULTISAMPLE);

            glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
            glClearDepth(1);
        });

    return 0;
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

    if( new_bytes > old_bytes ) {
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

    return 0;
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
    return 0;
}

size_t ogl_sizeof_primitive(GLenum primitive) {
    switch(primitive) {
        case GL_POINTS: return 1;
        case GL_LINES: return 2;
        case GL_TRIANGLES: return 3;
        case GL_PATCHES: return 4;
        default: return 0;
    }
    return 0;
}
