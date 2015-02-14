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

#ifndef SHADER_UNIFORMS
#define SHADER_UNIFORMS 32
#endif

#ifndef SHADER_ATTRIBUTES
#define SHADER_ATTRIBUTES 3
#endif

/* enum uniform_type { */
/*     invalid_uniform = 0, */
/*     uniform1f, */
/*     uniform2f, */
/*     uniform3f, */
/*     uniform4f */
/* }; */

struct Shader {
    GLuint vertex_shader, fragment_shader, program;

    uint32_t active_uniforms;
    struct {
        char name[256];
    } uniform[SHADER_UNIFORMS];

    struct {
        char name[256];
    } attribute[SHADER_ATTRIBUTES];
};

int init_shader();

void shader_create(const char* vertex_file, const char* fragment_file, struct Shader* p);
void shader_attribute(struct Shader* shader, int array_id, char* name);
void shader_uniform(struct Shader* shader, char* name, char* type, void* data);

#endif
