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

#include "render_shader.h"

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

void shader_create(const char* vertex_file, const char* fragment_file, struct Shader* p) {
    if( vertex_file && fragment_file ) {
        p->vertex_shader = glsl_compile_file(GL_VERTEX_SHADER, vertex_file);
        p->fragment_shader = glsl_compile_file(GL_FRAGMENT_SHADER, fragment_file);
    } else {
        p->vertex_shader = glsl_compile_file(GL_VERTEX_SHADER, "shader/default.vert");
        p->fragment_shader = glsl_compile_file(GL_FRAGMENT_SHADER, "shader/default.frag");
    }

    if( p->vertex_shader > 0 && p->fragment_shader > 0 ) {
        p->program = glsl_link_program(p->vertex_shader, p->fragment_shader);
    } else {
        p->program = 0;
    }

    for( int i = 0; i < SHADER_ATTRIBUTES; i++ ) {
        strncpy(p->attribute[i].name, "\0", 1);
        p->attribute[i].location = -1;
    }

    for( int i = 0; i < SHADER_UNIFORMS; i++ ) {
        strncpy(p->uniform[i].name, "\0", 1);
        p->uniform[i].location = -1;
    }
}

GLint shader_attribute(struct Shader* shader, int array_index, const char* name) {
    if( strlen(name) < 256 ) {
        GLint id = -1;

        if( (! name) && strlen(shader->attribute[array_index].name) ) {
            name = shader->attribute[array_index].name;
        }

        if( ! name ) {
            return -1;
        }

        if( shader->attribute[array_index].location > -1 ) {
            id = shader->attribute[array_index].location;
        } else {
            ogl_debug( glUseProgram(shader->program);
                       id = glGetAttribLocation(shader->program, name) );

            if( id > -1 ) {
                shader->attribute[array_index].location = id;
                strncpy(shader->attribute[array_index].name, name, strlen(name)+1);
            }
        }

        return id;
    }

    return -1;
}

GLint shader_uniform(struct Shader* shader, int location_index, const char* name, const char* type, void* data) {
    if( strlen(name) < 256 ) {

        GLint id = -1;

        if( (! name) && strlen(shader->uniform[location_index].name) ) {
            name = shader->uniform[location_index].name;
        }

        if( ! name ) {
            return -1;
        }

        if( shader->uniform[location_index].location > -1 ) {
           id = shader->uniform[location_index].location;
        } else {

            ogl_debug( glUseProgram(shader->program);
                      id = glGetUniformLocation(shader->program, name); );

            if( id > -1 ) {
                shader->uniform[location_index].location = id;
                strncpy(shader->uniform[location_index].name, name, strlen(name)+1);
            }
        }

        if( id > -1 && type && data ) {
            glUseProgram(shader->program);
            if( strcmp(type, "1f") == 0 ) { glUniform1f(id, ((float*)data)[0]); }
            if( strcmp(type, "2f") == 0 ) { glUniform2f(id, ((float*)data)[0], ((float*)data)[1]); }
            if( strcmp(type, "3f") == 0 ) { glUniform3f(id, ((float*)data)[0], ((float*)data)[1], ((float*)data)[2]); }
            if( strcmp(type, "4f") == 0 ) { glUniform4f(id, ((float*)data)[0], ((float*)data)[1], ((float*)data)[2], ((float*)data)[3]); }
        }

        return id;
    }
    return -1;
}
