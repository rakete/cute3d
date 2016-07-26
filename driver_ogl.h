#ifndef DRIVER_OGL_H
#define DRIVER_OGL_H

#include "gl_core_3_2.h"

#include "assert.h"
#include "stdio.h"

#include "driver_platform.h"
#include "driver_log.h"

// hack to get rid of annoying clang warning
#define ogl_stringify(arg) #arg

void ogl_error_print(GLenum error);

#ifdef DEBUG
#define ogl_debug(line) do {                                            \
        GLenum error = glGetError();                                    \
        while( error != GL_NO_ERROR ) {                                 \
            log_warn(__FILE__, __LINE__, "REPORTED ISSUE WAS PRESENT BEFORE OGL_DEBUG RAN: %s\n", ogl_stringify(line)); \
            log_fail(__FILE__, __LINE__, "%s\n", ogl_stringify(line)); \
            ogl_error_print(error);                                     \
            error = glGetError();                                       \
        }                                                               \
        line;                                                           \
        error = glGetError();                                           \
        while( error != GL_NO_ERROR ) {                                 \
            log_fail(__FILE__, __LINE__, "%s\n", ogl_stringify(line)); \
            ogl_error_print(error);                                     \
            error = glGetError();                                       \
        }                                                               \
        log_assert(error == GL_NO_ERROR);                               \
    } while(0)
#else
#define ogl_debug(line) line
#endif

#define MAX_OGL_PRIMITIVES GL_TRIANGLE_FAN+1

#define MAX_OGL_ATTRIBUTES 4
#define OGL_VERTEX 0
#define OGL_NORMAL 1
#define OGL_COLOR 2
#define OGL_TEXCOORD 3

/* #ifndef MAX_CUTE_ATTRIBUTES */
/* enum CuteAttributeType { */
/*     CUTE_SOMETHING */
/* }; */
/* #endif */

WARN_UNUSED_RESULT size_t ogl_buffer_resize(GLuint* buffer, size_t old_nbytes, size_t new_nbytes);

WARN_UNUSED_RESULT size_t ogl_sizeof_type(GLenum type);

WARN_UNUSED_RESULT size_t ogl_sizeof_primitive(GLenum primitive);

#endif
