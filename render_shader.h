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

#ifndef RENDER_SHADER_H
#define RENDER_SHADER_H

#include "stdio.h"

#include "driver_ogl.h"
#include "driver_glsl.h"

#define NUM_SHADER_UNIFORMS 256

#define SHADER_MVP_MATRIX 127
#define SHADER_MODEL_MATRIX 128
#define SHADER_VIEW_MATRIX 129
#define SHADER_PROJECTION_MATRIX 130
#define SHADER_NORMAL_MATRIX 131

#define SHADER_LIGHT_DIRECTION 140

#define SHADER_AMBIENT_COLOR 150
#define SHADER_DIFFUSE_COLOR 151

struct Shader {
    GLuint vertex_shader, fragment_shader, program;

    struct {
        GLint location;
        char name[256];
    } attribute[NUM_OGL_ATTRIBUTES];

    struct {
        GLint location;
        char name[256];
    } uniform[NUM_SHADER_UNIFORMS];
};

int init_shader();

void shader_create_from_files(const char* vertex_file, const char* fragment_file, struct Shader* p);
void shader_create_from_sources(const char* vertex_source, const char* fragment_source, struct Shader* p);

GLint shader_add_attribute(struct Shader* shader, int attribute_index, const char* name);
GLint shader_set_uniform(struct Shader* shader, int uniform_index, const char* name, const char* type, void* data);

void shader_create_flat(struct Shader* shader);
void shader_create_gl_lines(struct Shader* shader);

void shader_print(struct Shader* const shader);

#endif
