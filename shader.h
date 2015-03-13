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

#ifndef SHADER_H
#define SHADER_H

#include "stdio.h"

#include "ogl.h"
#include "glsl.h"

#ifndef SHADER_ATTRIBUTES
#define SHADER_ATTRIBUTES 3
#endif

#define SHADER_LOCATIONS 256

#define SHADER_MVP_MATRIX 127
#define SHADER_MODEL_MATRIX 128
#define SHADER_VIEW_MATRIX 129
#define SHADER_PROJECTION_MATRIX 130
#define SHADER_NORMAL_MATRIX 131

#define SHADER_LIGHT_DIRECTION 140

#define SHADER_AMBIENT_COLOR 150

struct Shader {
    GLuint vertex_shader, fragment_shader, program;

    struct {
        char name[256];
    } attribute[SHADER_ATTRIBUTES];

    struct {
        GLint id;
        char name[256];
    } location[SHADER_LOCATIONS];
};

int init_shader();

void shader_create(const char* vertex_file, const char* fragment_file, struct Shader* p);

void shader_attribute(struct Shader* shader, int array_index, const char* name);
GLint shader_location(struct Shader* shader, int location_index, const char* name);

GLint shader_uniform(struct Shader* shader, int location_index, const char* name, const char* type, void* data);

#endif
