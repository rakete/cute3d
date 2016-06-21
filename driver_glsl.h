/* cute3d, a simplistic opengl based engine written in C */
/* Copyright (C) 2013 Andreas Raster */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef DRIVER_GLSL_H
#define DRIVER_GLSL_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "limits.h"

#include "driver_log.h"
#include "driver_ogl.h"

void glsl_debug_info_log(GLuint object);

#define GLSL( source )                          \
    "//CUTE@" __C_FILENAME__ ":" log_tostring(__LINE__) "\n"  \
    #source "\0"

GLuint glsl_compile_source(GLenum type, const char* source);
GLuint glsl_compile_file(GLenum type, const char* filename);

GLuint glsl_create_program(GLuint vertex_shader, GLuint fragment_shader);
GLuint glsl_link_program(GLuint program);

#endif
