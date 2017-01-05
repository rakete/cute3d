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
    "//CUTE@" __FILE__ ":" log_tostring(__LINE__) "\n"  \
    #source "\0"

GLuint glsl_compile_file(GLenum type, const char* prefix_file, const char* shader_file);
GLuint glsl_compile_source(GLenum type, const char* prefix_source, const char* shader_source);

GLuint glsl_create_program(size_t n, GLuint* shader);
GLuint glsl_link_program(GLuint program);

#endif
