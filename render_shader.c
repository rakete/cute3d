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
    int ret = 0;

    glewInit();
    if( ! glewGetExtension("GL_ARB_uniform_buffer_object") )
    {
        printf("ERROR: uniform_buffer_object extension not found!\n");
        ret = 1;
    }

    return ret;
}

void shader_create_from_files(const char* vertex_file, const char* fragment_file, struct Shader* p) {
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

    for( int i = 0; i < NUM_OGL_ATTRIBUTES; i++ ) {
        strncpy(p->attribute[i].name, "\0", 1);
        p->attribute[i].location = -1;
    }

    for( int i = 0; i < NUM_SHADER_UNIFORMS; i++ ) {
        strncpy(p->uniform[i].name, "\0", 1);
        p->uniform[i].location = -1;
    }
}

void shader_create_from_sources(const char* vertex_source, const char* fragment_source, struct Shader* p) {
    assert( vertex_source );
    assert( fragment_source );

    p->vertex_shader = glsl_compile_source(GL_VERTEX_SHADER, vertex_source, strlen(vertex_source));
    p->fragment_shader = glsl_compile_source(GL_FRAGMENT_SHADER, fragment_source, strlen(fragment_source));

    if( p->vertex_shader > 0 && p->fragment_shader > 0 ) {
        p->program = glsl_link_program(p->vertex_shader, p->fragment_shader);
    } else {
        p->program = 0;
    }

    for( int i = 0; i < NUM_OGL_ATTRIBUTES; i++ ) {
        strncpy(p->attribute[i].name, "\0", 1);
        p->attribute[i].location = -1;
    }

    for( int i = 0; i < NUM_SHADER_UNIFORMS; i++ ) {
        strncpy(p->uniform[i].name, "\0", 1);
        p->uniform[i].location = -1;
    }
}

GLint shader_add_attribute(struct Shader* shader, int attribute_index, const char* name) {
    int name_length = strlen(name);
    assert( name_length > 0 );
    assert( name_length < 256 );

    GLint id = -1;

    if( (! name) && strlen(shader->attribute[attribute_index].name) ) {
        name = shader->attribute[attribute_index].name;
    }

    if( ! name ) {
        return -1;
    }

    if( shader->attribute[attribute_index].location > -1 ) {
        id = shader->attribute[attribute_index].location;
    } else {
        ogl_debug( glUseProgram(shader->program);
                   id = glGetAttribLocation(shader->program, name) );

        if( id > -1 ) {
            shader->attribute[attribute_index].location = id;
            strncpy(shader->attribute[attribute_index].name, name, strlen(name)+1);
        }
    }

    return id;
}

GLint shader_set_uniform(struct Shader* shader, int uniform_index, const char* name, const char* type, void* data) {
    int name_length = strlen(name);
    assert( name_length > 0 );
    assert( name_length < 256 );

    GLint id = -1;

    if( (! name) && strlen(shader->uniform[uniform_index].name) ) {
        name = shader->uniform[uniform_index].name;
    }

    if( ! name ) {
        return -1;
    }

    if( shader->uniform[uniform_index].location > -1 ) {
        id = shader->uniform[uniform_index].location;
    } else {
        ogl_debug( glUseProgram(shader->program);
                   id = glGetUniformLocation(shader->program, name); );

        if( id > -1 ) {
            shader->uniform[uniform_index].location = id;
            strncpy(shader->uniform[uniform_index].name, name, strlen(name)+1);
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

void shader_create_flat(struct Shader* shader) {
    shader_create_from_files("shader/flat.vert", "shader/flat.frag", shader);

    // these guys could go into shader_create
    shader_add_attribute(shader, OGL_VERTICES, "vertex");
    shader_add_attribute(shader, OGL_COLORS, "color");
    shader_add_attribute(shader, OGL_NORMALS, "normal");

    shader_set_uniform(shader, SHADER_MVP_MATRIX, "mvp_matrix", NULL, NULL);
    shader_set_uniform(shader, SHADER_NORMAL_MATRIX, "normal_matrix", NULL, NULL);
    shader_set_uniform(shader, SHADER_LIGHT_DIRECTION, "light_direction", NULL, NULL);
    shader_set_uniform(shader, SHADER_AMBIENT_COLOR, "ambiance", NULL, NULL);
}

void shader_create_gl_lines(struct Shader* shader) {
    const char* vertex_source =
        GLSL( uniform mat4 mvp_matrix;
              in vec3 vertex;
              in vec4 color;
              out vec4 frag_color;
              void main() {
                  gl_Position = mvp_matrix * vec4(vertex,1.0);
                  frag_color = color;
              });

    const char* fragment_source =
        GLSL( in vec4 frag_color;
              void main() {
                  gl_FragColor = frag_color;
              });

    shader_create_from_sources(vertex_source, fragment_source, shader);

    shader_add_attribute(shader, OGL_VERTICES, "vertex");
    shader_add_attribute(shader, OGL_COLORS, "color");

    shader_set_uniform(shader, SHADER_MVP_MATRIX, "mvp_matrix", NULL, NULL);
    //shader_set_uniform(shader, SHADER_DIFFUSE_COLOR, "color", NULL, NULL);
}

void shader_print(struct Shader* const shader) {
    printf("shader->vertex_shader: %d\n", shader->vertex_shader);
    printf("shader->fragment_shader: %d\n", shader->fragment_shader);
    printf("shader->program: %d\n", shader->program);

    for( int i = 0; i < NUM_OGL_ATTRIBUTES; i++ ) {
        if( strlen(shader->attribute[i].name) > 0 ) {
            printf("shader->attribute[%d].name: %s\n", i, shader->attribute[i].name);
            printf("shader->attribute[%d].location: %d\n", i, shader->attribute[i].location);
        }
    }

    for( int i = 0; i < NUM_SHADER_UNIFORMS; i++ ) {
        if( strlen(shader->uniform[i].name) > 0 ) {
            printf("shader->uniform[%d].name: %s\n", i, shader->uniform[i].name);
            printf("shader->uniform[%d].location: %d\n", i, shader->uniform[i].location);
        }
    }
}
