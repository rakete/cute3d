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

#ifndef GLSL_H
#define GLSL_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "ogl.h"
#include "math_types.h"
#include "matrix.h"
#include "quaternion.h"
#include "io.h"

void debug_info_log( GLuint object,
                     PFNGLGETSHADERIVPROC glGet__iv,
                     PFNGLGETSHADERINFOLOGPROC glGet__InfoLog );

#define GLSL( source ) "#version 130\n#extension GL_ARB_uniform_buffer_object:require\n" #source "\0"

GLuint compile_source(GLenum type, const char* source, GLsizei length);
GLuint compile_file(GLenum type, const char* filename);
GLuint link_program(GLuint vertex_shader, GLuint fragment_shader);
GLuint make_program(const char *vertex_source, const char* fragment_source);

#endif
