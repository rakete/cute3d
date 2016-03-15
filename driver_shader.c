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

#include "driver_shader.h"

int32_t init_shader() {
    int32_t ret = 0;

    // - we don't actually need this, but I want to check anyways because ubo's are
    // so useful and I would like to known when I can't use them in the future
    if( ! SDL_GL_ExtensionSupported("GL_ARB_uniform_buffer_object") ) {
        log_fail(stderr, __FILE__, __LINE__, "uniform_buffer_object extension not found!\n");
        ret = 1;
    }

    return ret;
}

void shader_create_empty(struct Shader* p) {
    p->name[0] = '\0';
    p->program = 0;
    p->vertex_shader = 0;
    p->fragment_shader = 0;

    for( int32_t j = 0; j < MAX_SHADER_ATTRIBUTES; j++ ) {
        p->attribute[j].location = -1;
        p->attribute[j].name[0] = '\0';
    }

    for( int32_t j = 0; j < MAX_SHADER_UNIFORMS; j++ ) {
        p->uniform[j].location = -1;
        p->uniform[j].name[0] = '\0';
    }
}

void shader_create_from_files(const char* vertex_file, const char* fragment_file, const char* name, struct Shader* p) {
    size_t name_length = strlen(name);
    log_assert( name_length > 0 );
    log_assert( name_length < 256 );

    //strncpy(p->name, name, name_length+1);
    p->name[0] = '\0';
    strncat(p->name, name, name_length);

    if( vertex_file && fragment_file ) {
        p->vertex_shader = glsl_compile_file(GL_VERTEX_SHADER, vertex_file);
        p->fragment_shader = glsl_compile_file(GL_FRAGMENT_SHADER, fragment_file);
    } else {
        log_warn(stderr, __FILE__, __LINE__, "shader files \"%s\" and \"%s\" do not exist, using default shader\n", vertex_file, fragment_file);
        p->vertex_shader = glsl_compile_file(GL_VERTEX_SHADER, "shader/default.vert");
        p->fragment_shader = glsl_compile_file(GL_FRAGMENT_SHADER, "shader/default.frag");
    }

    log_assert( p->vertex_shader > 0 );
    log_assert( p->fragment_shader > 0 );

    p->program = glsl_create_program(p->vertex_shader, p->fragment_shader);
    log_assert( p->program > 0 );

    glBindAttribLocation(p->program, SHADER_LOCATION_VERTICES, SHADER_NAME_VERTICES);
    glBindAttribLocation(p->program, SHADER_LOCATION_NORMALS, SHADER_NAME_NORMALS);
    glBindAttribLocation(p->program, SHADER_LOCATION_COLORS, SHADER_NAME_COLORS);
    glBindAttribLocation(p->program, SHADER_LOCATION_TEXCOORDS, SHADER_NAME_TEXCOORDS);

    p->program = glsl_link_program(p->program);
    log_assert( p->program > 0 );

    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        p->attribute[i].name[0] = '\0';
        p->attribute[i].location = -1;
    }

    for( int32_t i = 0; i < MAX_SHADER_UNIFORMS; i++ ) {
        p->uniform[i].name[0] = '\0';
        p->uniform[i].location = -1;
    }
}

void shader_create_from_sources(const char* vertex_source, const char* fragment_source, const char* name, struct Shader* p) {
    size_t name_length = strlen(name);
    log_assert( name_length > 0 );
    log_assert( name_length < 256 );

    //strncpy(p->name, name, name_length+1);
    p->name[0] = '\0';
    strncat(p->name, name, name_length);

    log_assert( vertex_source );
    log_assert( fragment_source );

    p->vertex_shader = glsl_compile_source(GL_VERTEX_SHADER, vertex_source);
    p->fragment_shader = glsl_compile_source(GL_FRAGMENT_SHADER, fragment_source);

    log_assert( p->vertex_shader > 0 );
    log_assert( p->fragment_shader > 0 );

    p->program = glsl_create_program(p->vertex_shader, p->fragment_shader);
    log_assert( p->program > 0 );

    glBindAttribLocation(p->program, SHADER_LOCATION_VERTICES, SHADER_NAME_VERTICES);
    glBindAttribLocation(p->program, SHADER_LOCATION_NORMALS, SHADER_NAME_NORMALS);
    glBindAttribLocation(p->program, SHADER_LOCATION_COLORS, SHADER_NAME_COLORS);
    glBindAttribLocation(p->program, SHADER_LOCATION_TEXCOORDS, SHADER_NAME_TEXCOORDS);

    p->program = glsl_link_program(p->program);
    log_assert( p->program > 0 );

    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        p->attribute[i].name[0] = '\0';
        p->attribute[i].location = -1;
    }

    for( int32_t i = 0; i < MAX_SHADER_UNIFORMS; i++ ) {
        p->uniform[i].name[0] = '\0';
        p->uniform[i].location = -1;
    }
}

GLint shader_add_attribute(struct Shader* shader, int32_t attribute_index, const char* name) {
    size_t name_length = strlen(name);
    log_assert( name_length > 0 );
    log_assert( name_length < 256 );
    log_assert( shader->program > 0 );

    GLint location = -1;
    if( shader->attribute[attribute_index].location > -1 ) {
        location = shader->attribute[attribute_index].location;
    } else {
        ogl_debug( glUseProgram(shader->program);
                   location = glGetAttribLocation(shader->program, name) );

        // - I had this as assertion, but a warning is better so then I can just hack glsl code and not have to worry if
        // there is a shader_add_attribute/uniform somewhere thats going to fail when I uncomment some input
        if( location > -1 ) {
            shader->attribute[attribute_index].location = location;

            //strncpy(shader->attribute[attribute_index].name, name, strlen(name)+1);
            shader->attribute[attribute_index].name[0] = '\0';
            strncat(shader->attribute[attribute_index].name, name, strlen(name));
        } else {
            log_warn(stderr, __FILE__, __LINE__, "could not add attribute %d location \"%s\" to shader \"%s\"\n", attribute_index, name, shader->name);
        }
    }

    return location;
}

GLint shader_add_uniform(struct Shader* shader, int32_t uniform_index, const char* name) {
    size_t name_length = strlen(name);
    log_assert( name_length > 0 );
    log_assert( name_length < 256 );
    log_assert( shader->program > 0 );

    GLint location = -1;
    if( shader->uniform[uniform_index].location > -1 ) {
        location = shader->uniform[uniform_index].location;
    } else {
        ogl_debug( glUseProgram(shader->program);
                   location = glGetUniformLocation(shader->program, name); );

        if( location > - 1 ) {
            shader->uniform[uniform_index].location = location;

            //strncpy(shader->uniform[uniform_index].name, name, strlen(name)+1);
            shader->uniform[uniform_index].name[0] = '\0';
            strncat(shader->uniform[uniform_index].name, name, strlen(name));
        } else {
            log_warn(stderr, __FILE__, __LINE__, "could not add uniform %d location \"%s\" to shader \"%s\"\n", uniform_index, name, shader->name);
        }
    }

    return location;
}

void shader_create_flat(const char* name, struct Shader* shader) {
    const char* vertex_source =
        GLSL( uniform mat4 mvp_matrix;
              uniform mat4 normal_matrix;
              uniform vec3 light_direction;

              shader_in vec3 vertex;
              shader_in vec4 color;
              shader_in vec3 normal;

              shader_out vec4 frag_color;
              shader_out float intensity;

              void main() {
                  intensity = 0.5 - dot(vec4(normalize(light_direction), 0.0), normal_matrix * vec4(normalize(normal),0.0))/2.0;

                  gl_Position = mvp_matrix * vec4(vertex,1.0);

                  frag_color = color;
              });

    const char* fragment_source =
        GLSL( shader_in vec4 frag_color;
              shader_in float intensity;

              uniform vec4 ambient_color;

              void main() {
                  gl_FragColor = ambient_color*(1.0 - intensity) + frag_color*intensity;
                  //gl_FragColor = ambient_color*intensity;
              });

    shader_create_from_sources(vertex_source, fragment_source, name, shader);

    shader_add_attribute(shader, SHADER_ATTRIBUTE_VERTICES, SHADER_NAME_VERTICES);
    shader_add_attribute(shader, SHADER_ATTRIBUTE_COLORS, SHADER_NAME_COLORS);
    shader_add_attribute(shader, SHADER_ATTRIBUTE_NORMALS, SHADER_NAME_NORMALS);

    shader_add_uniform(shader, SHADER_UNIFORM_MVP_MATRIX, SHADER_NAME_MVP_MATRIX);
    shader_add_uniform(shader, SHADER_UNIFORM_NORMAL_MATRIX, SHADER_NAME_NORMAL_MATRIX);
    shader_add_uniform(shader, SHADER_UNIFORM_LIGHT_DIRECTION, SHADER_NAME_LIGHT_DIRECTION);
    shader_add_uniform(shader, SHADER_UNIFORM_AMBIENT_COLOR, SHADER_NAME_AMBIENT_COLOR);
}

void shader_create_gl_lines(const char* name, struct Shader* shader) {
    const char* vertex_source =
        GLSL( uniform mat4 mvp_matrix;

              shader_in vec3 vertex;
              shader_in vec4 color;

              shader_out vec4 frag_color;
              void main() {
                  gl_Position = mvp_matrix * vec4(vertex,1.0);
                  frag_color = color;
              });

    const char* fragment_source =
        GLSL( shader_in vec4 frag_color;

              void main() {
                  gl_FragColor = frag_color;
              });

    shader_create_from_sources(vertex_source, fragment_source, name, shader);

    shader_add_attribute(shader, SHADER_ATTRIBUTE_VERTICES, "vertex");
    shader_add_attribute(shader, SHADER_ATTRIBUTE_COLORS, "color");

    shader_add_uniform(shader, SHADER_UNIFORM_MVP_MATRIX, "mvp_matrix");
}

void shader_print(FILE* f, const struct Shader* shader) {
    fprintf(f, "shader->vertex_shader: %d\n", shader->vertex_shader);
    fprintf(f, "shader->fragment_shader: %d\n", shader->fragment_shader);
    fprintf(f, "shader->program: %d\n", shader->program);

    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        if( strlen(shader->attribute[i].name) > 0 ) {
            fprintf(f, "shader->attribute[%d].name: %s\n", i, shader->attribute[i].name);
            fprintf(f, "shader->attribute[%d].location: %d\n", i, shader->attribute[i].location);
        }
    }

    for( int32_t i = 0; i < MAX_SHADER_UNIFORMS; i++ ) {
        if( strlen(shader->uniform[i].name) > 0 ) {
            fprintf(f, "shader->uniform[%d].name: %s\n", i, shader->uniform[i].name);
            fprintf(f, "shader->uniform[%d].location: %d\n", i, shader->uniform[i].location);
        }
    }
}

GLint shader_set_uniform_matrices(const struct Shader* shader, const Mat projection_matrix, const Mat view_matrix, const Mat model_matrix) {
    log_assert( shader != NULL );
    log_assert( projection_matrix != NULL );
    log_assert( view_matrix != NULL );
    log_assert( model_matrix != NULL );
    log_assert( shader->program > 0 );

    GLint ret = -1;
    GLint mvp_loc = -1;
    if( shader->uniform[SHADER_UNIFORM_MVP_MATRIX].location > -1) {
        mvp_loc = shader->uniform[SHADER_UNIFORM_MVP_MATRIX].location;
    } else if( strlen(shader->uniform[SHADER_UNIFORM_MVP_MATRIX].name) > 0 ) {
        ogl_debug( mvp_loc = glGetUniformLocation(shader->program, "mvp_matrix") );
        if( mvp_loc > -1 ) {
            log_warn(stderr, __FILE__, __LINE__, "uniform %d location \"%s\" of shader \"%s\" not cached\n", SHADER_UNIFORM_MVP_MATRIX, SHADER_NAME_MVP_MATRIX, shader->name);
        }
    }

    if( mvp_loc > -1 ) {
        Mat mvp_matrix = {0};
        mat_mul(model_matrix, view_matrix, mvp_matrix);
        mat_mul(mvp_matrix, projection_matrix, mvp_matrix);
        ogl_debug( glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp_matrix) );
        ret = mvp_loc;
    } else {
        GLint projection_loc = -1;
        if( shader->uniform[SHADER_UNIFORM_PROJECTION_MATRIX].location > -1 ) {
            projection_loc = shader->uniform[SHADER_UNIFORM_PROJECTION_MATRIX].location;
        } else if( strlen(shader->uniform[SHADER_UNIFORM_PROJECTION_MATRIX].name) > 0 ) {
            ogl_debug( projection_loc = glGetUniformLocation(shader->program, "projection_matrix") );
            if( projection_loc > -1 ) {
                log_warn(stderr, __FILE__, __LINE__, "uniform %d location \"%s\" of shader \"%s\" not cached\n", SHADER_UNIFORM_PROJECTION_MATRIX, SHADER_NAME_PROJECTION_MATRIX, shader->name);
            }
        }

        if( projection_loc > -1 ) {
            ogl_debug( glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection_matrix) );
            ret = projection_loc;
        } else {
            log_warn(stderr, __FILE__, __LINE__, "no projection_matrix location in shader \"%s\"\n", shader->name);
        }

        GLint view_loc = -1;
        if( shader->uniform[SHADER_UNIFORM_VIEW_MATRIX].location > -1 ) {
            view_loc = shader->uniform[SHADER_UNIFORM_VIEW_MATRIX].location;
        } else if( strlen(shader->uniform[SHADER_UNIFORM_VIEW_MATRIX].name) > 0 ) {
            ogl_debug( view_loc = glGetUniformLocation(shader->program, "view_matrix") );
            if( view_loc > -1 ) {
                log_warn(stderr, __FILE__, __LINE__, "uniform %d location \"%s\" of shader \"%s\" not cached\n", SHADER_UNIFORM_VIEW_MATRIX, SHADER_NAME_VIEW_MATRIX, shader->name);
            }
        }

        if( view_loc > -1 ) {
            ogl_debug( glUniformMatrix4fv(view_loc, 1, GL_FALSE, view_matrix) );
            ret = view_loc;
        } else {
            log_warn(stderr, __FILE__, __LINE__, "no view_matrix location in shader \"%s\"\n", shader->name);
        }

        GLint model_loc = -1;
        if( shader->uniform[SHADER_UNIFORM_MODEL_MATRIX].location > -1 ) {
            model_loc = shader->uniform[SHADER_UNIFORM_MODEL_MATRIX].location;
        } else if( strlen(shader->uniform[SHADER_UNIFORM_MODEL_MATRIX].name) > 0 ) {
            ogl_debug( model_loc = glGetUniformLocation(shader->program, "model_matrix") );
            if( model_loc > -1 ) {
                log_warn(stderr, __FILE__, __LINE__, "uniform %d location \"%s\" of shader \"%s\" not cached\n", SHADER_UNIFORM_MODEL_MATRIX, SHADER_NAME_MODEL_MATRIX, shader->name);
            }
        }

        if( model_loc > -1 ) {
            ogl_debug( glUniformMatrix4fv(model_loc, 1, GL_FALSE, model_matrix) );
            ret = model_loc;
        } else {
            log_warn(stderr, __FILE__, __LINE__, "no model_matrix location in shader \"%s\"\n", shader->name);
        }
    }

    GLint normal_loc = -1;
    if( shader->uniform[SHADER_UNIFORM_NORMAL_MATRIX].location > -1 ) {
        normal_loc = shader->uniform[SHADER_UNIFORM_NORMAL_MATRIX].location;
    } else if( strlen(shader->uniform[SHADER_UNIFORM_NORMAL_MATRIX].name) > 0 ) {
        ogl_debug( normal_loc = glGetUniformLocation(shader->program, "normal_matrix") );
        if( normal_loc > -1 ) {
            log_warn(stderr, __FILE__, __LINE__, "uniform %d location \"%s\" of shader \"%s\" not cached\n", SHADER_UNIFORM_NORMAL_MATRIX, SHADER_NAME_NORMAL_MATRIX, shader->name);
        }
    }

    if( normal_loc > -1 ) {
        Mat normal_matrix = {0};
        mat_copy4f(model_matrix, normal_matrix);
        ogl_debug( glUniformMatrix4fv(normal_loc, 1, GL_FALSE, normal_matrix) );
        ret = normal_loc;
    }

    return ret;
}

GLint shader_set_uniform_3f(const struct Shader* shader, int32_t uniform_index, uint32_t size, GLenum type, void* data) {
    log_assert( shader->program > 0 );
    log_assert( uniform_index >= 0 );
    log_assert( uniform_index <= MAX_SHADER_UNIFORMS );
    log_assert( size <= 4 );
    log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE );
    log_assert( data != NULL );

    GLint location = shader->uniform[uniform_index].location;

    if( location > -1 && data != NULL ) {

        // - pad data with zeros so that argument size can be smaller than 3 and not cause a crash
        // - also normalize when input is unsigned bytes (for colors)
        GLfloat float_array[3] = {0};
        switch(type) {
            case GL_FLOAT: {
                float_array[0] = size > 0 ? ((float*)data)[0] : 0.0f;
                float_array[1] = size > 1 ? ((float*)data)[1] : 0.0f;
                float_array[2] = size > 2 ? ((float*)data)[2] : 0.0f;
                break;
            }
            case GL_UNSIGNED_BYTE: {
                float_array[0] = size > 0 ? ((uint8_t*)data)[0] / 255.0 : 0.0f;
                float_array[1] = size > 1 ? ((uint8_t*)data)[1] / 255.0 : 0.0f;
                float_array[2] = size > 2 ? ((uint8_t*)data)[2] / 255.0 : 0.0f;
                break;
            }
            default: log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE );
        }

        if( size < 3 ) {
            log_warn(stderr, __FILE__, __LINE__, "setting uniform 3f with only %df data\n", size);
        }

        // glUniform changes state of the program, so it needs to be run after glUseProgram
        // that does not apply to glVertexAttribPointer, which only changes global state
        ogl_debug( glUseProgram(shader->program);
                   glUniform3f(location, float_array[0], float_array[1], float_array[2]); );

    } else {
        log_warn(stderr, __FILE__, __LINE__, "uniform %d of shader \"%s\"was not set\n", uniform_index, shader->name);
    }

    return location;
}

GLint shader_set_uniform_4f(const struct Shader* shader, int32_t uniform_index, uint32_t size, GLenum type, void* data) {
    log_assert( shader != NULL );
    log_assert( shader->program > 0 );
    log_assert( uniform_index >= 0 );
    log_assert( uniform_index <= MAX_SHADER_UNIFORMS );
    log_assert( size <= 4 );
    log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE );
    log_assert( data != NULL );

    GLint location = shader->uniform[uniform_index].location;

    if( location > -1 && data != NULL ) {

        // - pad data with zeros so that argument size can be smaller than 3 and not cause a crash
        // - also normalize when input is unsigned bytes (for colors)
        GLfloat float_array[4] = {0};
        switch(type) {
            case GL_FLOAT: {
                float_array[0] = size > 0 ? ((float*)data)[0] : 0.0f;
                float_array[1] = size > 1 ? ((float*)data)[1] : 0.0f;
                float_array[2] = size > 2 ? ((float*)data)[2] : 0.0f;
                float_array[3] = size > 3 ? ((float*)data)[3] : 0.0f;
                break;
            }
            case GL_UNSIGNED_BYTE: {
                float_array[0] = size > 0 ? ((uint8_t*)data)[0] / 255.0 : 0.0f;
                float_array[1] = size > 1 ? ((uint8_t*)data)[1] / 255.0 : 0.0f;
                float_array[2] = size > 2 ? ((uint8_t*)data)[2] / 255.0 : 0.0f;
                float_array[3] = size > 3 ? ((uint8_t*)data)[3] / 255.0 : 0.0f;
                break;
            }
            default: log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE );
        }

        if( size < 4 ) {
            log_warn(stderr, __FILE__, __LINE__, "setting uniform 4f with only %df data\n", size);
        }

        // glUniform changes state of the program, so it needs to be run after glUseProgram
        // that does not apply to glVertexAttribPointer, which only changes global state
        ogl_debug( glUseProgram(shader->program);
                   glUniform4f(location, float_array[0], float_array[1], float_array[2], float_array[3]); );
    } else {
        log_warn(stderr, __FILE__, __LINE__, "uniform %d of shader \"%s\"was not set\n", uniform_index, shader->name);
    }

    return location;
}

GLint shader_set_attribute(const struct Shader* shader, int32_t attribute_i, GLuint buffer, size_t n, GLint c_num, GLenum c_type, GLsizei stride, const GLvoid* p) {
    log_assert( attribute_i >= SHADER_ATTRIBUTE_VERTICES );
    log_assert( attribute_i < MAX_SHADER_ATTRIBUTES );
    log_assert( c_num >= 0 );
    log_assert( c_num <= 4 );

    GLint location = -1;
    if( buffer == 0 || n == 0 || c_num == 0 ) {
        return location;
    }

    if( shader == NULL ) {
        switch(attribute_i) {
            case SHADER_ATTRIBUTE_VERTICES: location = SHADER_LOCATION_VERTICES; break;
            case SHADER_ATTRIBUTE_NORMALS: location = SHADER_LOCATION_NORMALS; break;
            case SHADER_ATTRIBUTE_COLORS: location = SHADER_LOCATION_COLORS; break;
            case SHADER_ATTRIBUTE_TEXCOORDS: location = SHADER_LOCATION_TEXCOORDS; break;
            default: log_assert( SHADER_ATTRIBUTE_VERTICES == 0 && attribute_i >= SHADER_ATTRIBUTE_VERTICES && attribute_i < NUM_SHADER_ATTRIBUTES );
        }
    } else if( shader->attribute[attribute_i].location > -1 ) {
        location = shader->attribute[attribute_i].location;
    } else if( strlen(shader->attribute[attribute_i].name) > 0 ) {
        ogl_debug( location = glGetAttribLocation(shader->program, shader->attribute[attribute_i].name) );
        if( location > -1 ) {
            log_warn(stderr, __FILE__, __LINE__, "attribute %d location \"%s\" of shader \"%s\" not cached\n", attribute_i, shader->attribute[attribute_i].name, shader->name);
        }
    } else if( strlen(shader->attribute[attribute_i].name) == 0 ) {
        log_warn(stderr, __FILE__, __LINE__, "attribute %d has no name or location in shader \"%s\"\n", attribute_i, shader->name);
    } else {
        log_warn(stderr, __FILE__, __LINE__, "attribute %d has no location \"%s\" in shader \"%s\"\n", attribute_i, shader->attribute[attribute_i].name, shader->name);
    }

    if( location > -1 ) {
        ogl_debug( glEnableVertexAttribArray((GLuint)location);
                   glBindBuffer(GL_ARRAY_BUFFER, buffer);
                   glVertexAttribPointer((GLuint)location, c_num, c_type, GL_TRUE, stride, p); );
    }

    return location;
}
