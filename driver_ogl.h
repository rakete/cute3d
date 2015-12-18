#ifndef DRIVER_OGL_H
#define DRIVER_OGL_H

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
        while( error != GL_NO_ERROR ) {                                 \
            printf("%s@%d:::::\n%s\n", __FILE__, __LINE__, ogl_stringify(line)); \
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

#ifndef NUM_OGL_ATTRIBUTES
enum OglAttributeType {
    OGL_VERTICES = 0,
    OGL_NORMALS,
    OGL_COLORS,
    OGL_TEXCOORDS,
    /* OGL_INTERLEAVED_VNCT, */
    /* OGL_INTERLEAVED_NCT, */
    /* OGL_INTERLEAVED_VCT, */
    /* OGL_INTERLEAVED_VNT, */
    /* OGL_INTERLEAVED_VNC, */
    /* OGL_INTERLEAVED_VN, */
    /* OGL_INTERLEAVED_VC, */
    /* OGL_INTERLEAVED_VT, */
    /* OGL_INTERLEAVED_NC, */
    /* OGL_INTERLEAVED_NT, */
    /* OGL_INTERLEAVED_CT, */
    NUM_OGL_ATTRIBUTES
};
#endif

int init_ogl(int width, int height, const float clear_color[4]);

#endif
