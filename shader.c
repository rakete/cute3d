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

#include "shader.h"

int init_shader() {
    int ret = 1;

    glewInit();
    if( ! glewGetExtension("GL_ARB_uniform_buffer_object") )
    {
        printf("ERROR: uniform_buffer_object extension not found!\n");
        ret = 0;
    }

    return ret;
}

struct Shader* shader_create(struct Shader* p, const char* vertex_file, const char* fragment_file) {
    if( vertex_file && fragment_file ) {
        p->vertex_shader = compile_file(GL_VERTEX_SHADER, vertex_file);
        p->fragment_shader = compile_file(GL_FRAGMENT_SHADER, fragment_file);
    } else {
        p->vertex_shader = compile_file(GL_VERTEX_SHADER, "shader/default.vertex");
        p->fragment_shader = compile_file(GL_FRAGMENT_SHADER, "shader/default.fragment");
    }

    if( p->vertex_shader > 0 && p->fragment_shader > 0 ) {
        p->program = link_program(p->vertex_shader, p->fragment_shader);
    } else {
        p->program = 0;
    }

    p->active_uniforms = 0;
    for( int i = 0; i < SHADER_UNIFORMS; i++ ) {
        strncpy(p->uniform[i].name, "\0", 1);
    }

    for( int i = 0; i < SHADER_ATTRIBUTES; i++ ) {
        strncpy(p->attribute[i].name, "\0", 1);
    }
}

void shader_attribute(struct Shader* shader, int array_id, char* name) {
    if( strlen(name) < 256 ) {
        strncpy(shader->attribute[array_id].name, name, strlen(name)+1);
    }
}

void shader_uniform(struct Shader* shader, char* name, char* type, void* data) {
    if( shader->active_uniforms < SHADER_UNIFORMS &&
        strlen(name) < 256 )
    {
        glUseProgram(shader->program);
        GLint id = glGetUniformLocation(shader->program, name);
        if( id >= 0 ) {
            strncpy(shader->uniform[shader->active_uniforms].name, name, strlen(name));
            if( strcmp(type, "1f") == 0 ) { glUniform1f(id, ((float*)data)[0]); }
            if( strcmp(type, "2f") == 0 ) { glUniform2f(id, ((float*)data)[0], ((float*)data)[1]); }
            if( strcmp(type, "3f") == 0 ) { glUniform3f(id, ((float*)data)[0], ((float*)data)[1], ((float*)data)[2]); }
            if( strcmp(type, "4f") == 0 ) { glUniform4f(id, ((float*)data)[0], ((float*)data)[1], ((float*)data)[2], ((float*)data)[3]); }
            
            shader->active_uniforms++;
        }
        glUseProgram(0);
    }
}
