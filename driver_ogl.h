/* Cute3D, a simple opengl based framework for writing interactive realtime applications */

/* Copyright (C) 2013-2017 Andreas Raster */

/* This file is part of Cute3D. */

/* Cute3D is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* Cute3D is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with Cute3D.  If not, see <http://www.gnu.org/licenses/>. */

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

/* #ifndef MAX_CUTE_ATTRIBUTES */
/* enum CuteAttributeType { */
/*     CUTE_SOMETHING */
/* }; */
/* #endif */

WARN_UNUSED_RESULT size_t ogl_buffer_resize(GLuint* buffer, size_t old_nbytes, size_t new_nbytes);

WARN_UNUSED_RESULT size_t ogl_sizeof_type(GLenum type);

WARN_UNUSED_RESULT size_t ogl_sizeof_primitive(GLenum primitive);

#endif
