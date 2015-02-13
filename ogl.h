#include "GL/glew.h"
#include "GL/gl.h"

#ifdef DEBUG
#define ogl_debug(line) do {                                            \
        line;                                                           \
        GLenum error = glGetError();                                    \
        printf(#line "\n");                                                  \
        switch(error) {                                                 \
            case GL_NO_ERROR: break;                                    \
            case GL_INVALID_ENUM:                                       \
                printf("%d: " #line "\n", __LINE__);                    \
                printf("glGetError(): GL_INVALID_ENUM\n");              \
                break;                                                  \
            case GL_INVALID_VALUE:                                      \
                printf("%d: " #line "\n", __LINE__);                    \
                printf("glGetError(): GL_INVALID_VALUE\n");             \
                break;                                                  \
            case GL_INVALID_OPERATION:                                  \
                printf("%d: " #line "\n", __LINE__);                    \
                printf("glGetError(): GL_INVALID_OPERATION\n");         \
                break;                                                  \
            case GL_INVALID_FRAMEBUFFER_OPERATION:                      \
                printf("%d: " #line "\n", __LINE__);                    \
                printf("glGetError(): GL_INVALID_FRAMEBUFFER_OPERATION\n"); \
                break;                                                  \
            case GL_OUT_OF_MEMORY:                                      \
                printf("%d: " #line "\n", __LINE__);                    \
                printf("glGetError(): GL_OUT_OF_MEMORY\n");             \
                break;                                                  \
            case GL_STACK_UNDERFLOW:                                    \
                printf("%d: " #line "\n", __LINE__);                    \
                printf("glGetError(): GL_STACK_UNDERFLOW\n");           \
                break;                                                  \
            case GL_STACK_OVERFLOW:                                     \
                printf("%d: " #line "\n", __LINE__);                    \
                printf("glGetError(): GL_STACK_OVERFLOW\n");            \
                break;                                                  \
            default: break;                                             \
        }                                                               \
        assert(error == GL_NO_ERROR);                                   \
    } while(0)
#else
#define ogl_debug(line) line
#endif
