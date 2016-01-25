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

void glsl_debug_info_log( GLuint object,
                          PFNGLGETSHADERIVPROC glGet__iv,
                          PFNGLGETSHADERINFOLOGPROC glGet__InfoLog );


// I need this compatibilty crap if I want to be able to deploy stuff on webgl/android
// GLSL ES compatibilty is a MESS, targeted versions are:
// GLSL ES 1  : 100
// GLSL GL 3  : 130
// GLSL ES 3  : 300 es (only for playing around so far)
// maybe I can change the latter to 150 to get geometry shader, but I would not count on it,
// for now I'll keep everything at version 100 manually so that I catch problems with GLSL ES
// in my shader code, its just supported fine on my intel notebook
//
// https://github.com/mattdesl/lwjgl-basics/wiki/GLSL-Versions
// http://bitiotic.com/blog/2013/09/24/opengl-es-shading-language-potholes-and-problems/
// http://stackoverflow.com/questions/2631324/opengl-shading-language-backwards-compatibility
//
// "#extension GL_ARB_enhanced_layouts:require\n"
// "#extension GL_ARB_ES2_compatibility:require\n"
// "#extension GL_ARB_uniform_buffer_object:require\n"
// #if __VERSION__ < 130\n#define in attribute\n#define out varying\n#endif\n
#define GLSL_VERT_COMPAT                        \
    "#version 100\n"                            \
    "\n"                                        \
    "precision highp float;\n"                  \
    "\n"                                        \
    "#if __VERSION__ == 100\n"                  \
    "#define shader_in attribute\n"             \
    "#define shader_out varying\n"              \
    "#define smooth\n"                          \
    "#endif\n"                                  \
    "\n"                                        \
    "#if __VERSION__ == 130\n"                  \
    "#define shader_in in\n"                    \
    "#define shader_out out\n"                  \
    "#endif\n"                                  \
    "\n"                                        \
    "#if __VERSION__ == 300\n"                  \
    "#define shader_in in\n"                    \
    "#define shader_out out\n"                  \
    "#endif\n"                                  \
    "\0"

#define GLSL_FRAG_COMPAT                        \
    "#version 100\n"                            \
    "\n"                                        \
    "precision highp float;\n"                  \
    "\n"                                        \
    "#if __VERSION__ == 100\n"                  \
    "#define shader_in varying\n"               \
    "#define shader_out varying\n"              \
    "#define smooth\n"                          \
    "#endif\n"                                  \
    "\n"                                        \
    "#if __VERSION__ == 130\n"                  \
    "#define shader_in in\n"                    \
    "#define shader_out out\n"                  \
    "\n"                                        \
    "#define texture2D texture\n"               \
    "#define texture3D texture\n"               \
    "#define texture2DRect texture\n"           \
    "#define textureCube texture\n"             \
    "#endif\n"                                  \
    "\n"                                        \
    "#if __VERSION__ == 300\n"                  \
    "#define shader_in in\n"                    \
    "#define shader_out out\n"                  \
    "#endif\n"                                  \
    "\0"

#define GLSL( source )                          \
    "//CUTE\n"                                  \
    #source "\0"

GLuint glsl_compile_source(GLenum type, const char* source);
GLuint glsl_compile_file(GLenum type, const char* filename);
GLuint glsl_link_program(GLuint vertex_shader, GLuint fragment_shader);
GLuint glsl_make_program(const char *vertex_source, const char* fragment_source);

#endif
