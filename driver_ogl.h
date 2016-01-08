#ifndef DRIVER_OGL_H
#define DRIVER_OGL_H

#include "GL/glew.h"
#include "GL/gl.h"

#include "assert.h"
#include "stdio.h"

#include "driver_log.h"

// hack to get rid of annoying clang warning
#define ogl_stringify(arg) #arg

#ifdef DEBUG
#define ogl_debug(line) do {                                            \
        line;                                                           \
        GLenum error = glGetError();                                    \
        while( error != GL_NO_ERROR ) {                                 \
            printf("%s:%d:::::\n%s\n", __FILE__, __LINE__, ogl_stringify(line)); \
            switch(error) {                                             \
                case GL_NO_ERROR: break;                                \
                case GL_INVALID_ENUM:                                   \
                    printf("glGetError: GL_INVALID_ENUM\n");            \
                    break;                                              \
                case GL_INVALID_VALUE:                                  \
                    printf("glGetError: GL_INVALID_VALUE\n");           \
                    break;                                              \
                case GL_INVALID_OPERATION:                              \
                    printf("glGetError: GL_INVALID_OPERATION\n");       \
                    break;                                              \
                case GL_INVALID_FRAMEBUFFER_OPERATION:                  \
                    printf("glGetError: GL_INVALID_FRAMEBUFFER_OPERATION\n"); \
                    break;                                              \
                case GL_OUT_OF_MEMORY:                                  \
                    printf("glGetError: GL_OUT_OF_MEMORY\n");           \
                    break;                                              \
                case GL_STACK_UNDERFLOW:                                \
                    printf("glGetError: GL_STACK_UNDERFLOW\n");         \
                    break;                                              \
                case GL_STACK_OVERFLOW:                                 \
                    printf("glGetError: GL_STACK_OVERFLOW\n");          \
                    break;                                              \
                default: break;                                         \
            }                                                           \
            error = glGetError();                                       \
        }                                                               \
        assert(error == GL_NO_ERROR);                                   \
    } while(0)
#else
#define ogl_debug(line) line
#endif

#define NUM_OGL_PRIMITIVES GL_TRIANGLE_FAN+1


/* #ifndef NUM_CUTE_ATTRIBUTES */
/* enum CuteAttributeType { */
/*     CUTE_SOMETHING */
/* }; */
/* #endif */

int init_ogl(int width, int height, const float clear_color[4]);

int ogl_buffer_resize(GLuint* buffer, int old_nbytes, int new_nbytes);

GLsizei ogl_sizeof_type(GLenum type);

GLsizei ogl_sizeof_primitive(GLenum primitive);

#endif
