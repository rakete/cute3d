#ifndef OGL_H
#define OGL_H

#include "GL/glew.h"
#include "GL/gl.h"

#include "assert.h"
#include "stdio.h"

// hack to get rid of annoying clang warning
#define ogl_stringify(arg) #arg

#ifdef DEBUG
#define ogl_debug(line) do {                                            \
        line;                                                           \
        GLenum error = glGetError();                                    \
        if( error != GL_NO_ERROR ) {                                    \
            printf("%s@%d:::::\n%s\n", __FILE__, __LINE__, ogl_stringify(line)); \
        }                                                               \
        switch(error) {                                                 \
            case GL_NO_ERROR: break;                                    \
            case GL_INVALID_ENUM:                                       \
                printf("glGetError: GL_INVALID_ENUM\n");                \
                break;                                                  \
            case GL_INVALID_VALUE:                                      \
                printf("glGetError: GL_INVALID_VALUE\n");               \
                break;                                                  \
            case GL_INVALID_OPERATION:                                  \
                printf("glGetError: GL_INVALID_OPERATION\n");           \
                break;                                                  \
            case GL_INVALID_FRAMEBUFFER_OPERATION:                      \
                printf("glGetError: GL_INVALID_FRAMEBUFFER_OPERATION\n"); \
                break;                                                  \
            case GL_OUT_OF_MEMORY:                                      \
                printf("glGetError: GL_OUT_OF_MEMORY\n");               \
                break;                                                  \
            case GL_STACK_UNDERFLOW:                                    \
                printf("glGetError: GL_STACK_UNDERFLOW\n");             \
                break;                                                  \
            case GL_STACK_OVERFLOW:                                     \
                printf("glGetError: GL_STACK_OVERFLOW\n");              \
                break;                                                  \
            default: break;                                             \
        }                                                               \
        assert(error == GL_NO_ERROR);                                   \
    } while(0)
#else
#define ogl_debug(line) line
#endif

int init_ogl(int width, int height);

#endif
