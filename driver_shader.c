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

const char* global_shader_attribute_names[MAX_SHADER_ATTRIBUTES] = {0};
const char* global_shader_uniform_names[MAX_SHADER_UNIFORMS] = {0};

int32_t init_shader() {
    int32_t ret = 0;

    // - we don't actually need this, but I want to check anyways because ubo's are
    // so useful and I would like to known when I can't use them in the future
    if( ! SDL_GL_ExtensionSupported("GL_ARB_uniform_buffer_object") ) {
        log_fail(__FILE__, __LINE__, "uniform_buffer_object extension not found!\n");
        ret = 1;
    }

    for( size_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        global_shader_attribute_names[i] = "invalid_attribute";
    }

    for( size_t i = 0; i < MAX_SHADER_UNIFORMS; i++ ) {
        global_shader_uniform_names[i] = "invalid_uniform";
    }

    global_shader_attribute_names[SHADER_ATTRIBUTE_VERTICES] = "vertex";
    global_shader_attribute_names[SHADER_ATTRIBUTE_NORMALS] = "normal";
    global_shader_attribute_names[SHADER_ATTRIBUTE_COLORS] = "color";
    global_shader_attribute_names[SHADER_ATTRIBUTE_TEXCOORDS] = "texcoord";

    global_shader_attribute_names[SHADER_ATTRIBUTE_INSTANCE_ID] = "instance_id";
    global_shader_attribute_names[SHADER_ATTRIBUTE_PREV_VERTEX] = "prev_vertex";
    global_shader_attribute_names[SHADER_ATTRIBUTE_NEXT_VERTEX] = "next_vertex";
    global_shader_attribute_names[SHADER_ATTRIBUTE_LINE_THICKNESS] = "line_thickness";
    global_shader_attribute_names[SHADER_ATTRIBUTE_BARYCENTRIC_COORDINATE] = "barycentric_coordinate";

    global_shader_uniform_names[SHADER_UNIFORM_MVP_MATRIX] = "mvp_matrix";
    global_shader_uniform_names[SHADER_UNIFORM_MODEL_MATRIX] = "model_matrix";
    global_shader_uniform_names[SHADER_UNIFORM_VIEW_MATRIX] = "view_matrix";
    global_shader_uniform_names[SHADER_UNIFORM_PROJECTION_MATRIX] = "projection_matrix";
    global_shader_uniform_names[SHADER_UNIFORM_NORMAL_MATRIX] = "normal_matrix";

    global_shader_uniform_names[SHADER_UNIFORM_LIGHT_DIRECTION] = "light_direction";
    global_shader_uniform_names[SHADER_UNIFORM_AMBIENT_COLOR] = "ambient_color";
    global_shader_uniform_names[SHADER_UNIFORM_DIFFUSE_COLOR] = "diffuse_color";
    global_shader_uniform_names[SHADER_UNIFORM_DIFFUSE_TEXTURE] = "diffuse_texture";

    global_shader_uniform_names[SHADER_UNIFORM_ASPECT_RATIO] = "aspect_ratio";
    global_shader_uniform_names[SHADER_UNIFORM_LINE_Z_SCALING] = "line_z_scaling";

    return ret;
}

void shader_create(struct Shader* p) {
    p->name[0] = '\0';
    p->program = 0;
    p->vertex_shader = 0;
    p->fragment_shader = 0;

    for( int32_t j = 0; j < MAX_SHADER_ATTRIBUTES; j++ ) {
        p->attribute[j].location = -1;
        p->attribute[j].name[0] = '\0';
        p->attribute[j].unset = true;
        p->attribute[j].warn_once = true;
    }

    for( int32_t j = 0; j < MAX_SHADER_UNIFORMS; j++ ) {
        p->uniform[j].location = -1;
        p->uniform[j].name[0] = '\0';
        p->uniform[j].unset = true;
        p->uniform[j].warn_once = true;
    }

    p->verified = false;
}

void shader_create_from_files(const char* vertex_file, const char* fragment_file, const char* name, struct Shader* p) {
    size_t name_length = strlen(name);
    log_assert( name_length > 0 );
    log_assert( name_length < 256 );

    p->name[0] = '\0';
    strncat(p->name, name, name_length);

    if( vertex_file && fragment_file ) {
        if( strcmp( name, "default_shader") && strcmp( name, "font_shader") ) {
            log_info(__FILE__, __LINE__, "creating shader \"%s\" from files: \"%s\", \"%s\"\n", name, vertex_file, fragment_file);
        }
        p->vertex_shader = glsl_compile_file(GL_VERTEX_SHADER, vertex_file);
        p->fragment_shader = glsl_compile_file(GL_FRAGMENT_SHADER, fragment_file);
    } else {
        log_warn(__FILE__, __LINE__, "shader files \"%s\" and \"%s\" do not exist, using default shader\n", vertex_file, fragment_file);
        p->vertex_shader = glsl_compile_file(GL_VERTEX_SHADER, "shader/default.vert");
        p->fragment_shader = glsl_compile_file(GL_FRAGMENT_SHADER, "shader/default.frag");
    }

    log_assert( p->vertex_shader > 0 );
    log_assert( p->fragment_shader > 0 );

    p->program = glsl_create_program(p->vertex_shader, p->fragment_shader);
    log_assert( p->program > 0 );

    for( uint32_t attribute_i = 0; attribute_i < MAX_SHADER_ATTRIBUTES; attribute_i++ ) {
        glBindAttribLocation(p->program, attribute_i, global_shader_attribute_names[attribute_i]);
    }

    p->program = glsl_link_program(p->program);
    log_assert( p->program > 0 );

    p->verified = false;

    shader_setup_locations(p);
}

void shader_create_from_sources(const char* vertex_source, const char* fragment_source, const char* name, struct Shader* p) {
    size_t name_length = strlen(name);
    log_assert( name_length > 0 );
    log_assert( name_length < 256 );

    p->name[0] = '\0';
    strncat(p->name, name, name_length);

    log_assert( vertex_source );
    log_assert( fragment_source );

    if( strcmp( name, "default_shader") && strcmp( name, "font_shader") ) {
        log_info(__FILE__, __LINE__, "creating shader \"%s\" from sources\n", name);
    }
    p->vertex_shader = glsl_compile_source(GL_VERTEX_SHADER, vertex_source);
    p->fragment_shader = glsl_compile_source(GL_FRAGMENT_SHADER, fragment_source);

    log_assert( p->vertex_shader > 0 );
    log_assert( p->fragment_shader > 0 );

    p->program = glsl_create_program(p->vertex_shader, p->fragment_shader);
    log_assert( p->program > 0 );

    for( uint32_t attribute_i = 0; attribute_i < MAX_SHADER_ATTRIBUTES; attribute_i++ ) {
        glBindAttribLocation(p->program, attribute_i, global_shader_attribute_names[attribute_i]);
    }

    p->program = glsl_link_program(p->program);
    log_assert( p->program > 0 );

    p->verified = false;

    shader_setup_locations(p);
}

void shader_setup_locations(struct Shader* p) {
    GLint num_active_attributes = 0;
    glGetProgramiv(p->program, GL_ACTIVE_ATTRIBUTES, &num_active_attributes);
    int32_t num_cached_attributes = 0;
    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        const char* attribute_name = global_shader_attribute_names[i];
        GLint location = glGetAttribLocation(p->program, global_shader_attribute_names[i]);

        p->attribute[i].name[0] = '\0';
        p->attribute[i].location = -1;
        p->attribute[i].unset = true;
        p->attribute[i].warn_once = true;

        if( location > -1 ) {
            log_assert( strlen(attribute_name) < 256 );
            strncat(p->attribute[i].name, attribute_name, strlen(attribute_name));
            p->attribute[i].location = location;
            num_cached_attributes += 1;
        }
    }

    if( num_active_attributes > num_cached_attributes) {
        log_warn(__FILE__, __LINE__, "shader \"%s\" has %d unknown attributes\n", p->name, num_active_attributes - num_cached_attributes);
    }

    GLint num_active_uniforms = 0;
    glGetProgramiv(p->program, GL_ACTIVE_UNIFORMS, &num_active_uniforms);
    int32_t num_cached_uniforms = 0;
    for( int32_t i = 0; i < MAX_SHADER_UNIFORMS; i++ ) {
        const char* uniform_name = global_shader_uniform_names[i];
        GLint location = glGetUniformLocation(p->program, global_shader_uniform_names[i]);

        p->uniform[i].name[0] = '\0';
        p->uniform[i].location = -1;
        p->uniform[i].unset = true;
        p->uniform[i].warn_once = true;

        if( location > -1 ) {
            p->uniform[i].name[0] = '\0';
            log_assert( strlen(uniform_name) < 256 );
            strncat(p->uniform[i].name, uniform_name, strlen(uniform_name));
            p->uniform[i].location = location;
            num_cached_uniforms += 1;
        }
    }

    if( num_active_uniforms > num_cached_uniforms) {
        log_warn(__FILE__, __LINE__, "shader \"%s\" has %d unknown uniforms\n", p->name, num_active_uniforms - num_cached_uniforms);
    }

    shader_verify_locations(p);
}

bool shader_verify_locations(struct Shader* p) {
    bool ret = false;

    if( p->verified ) {
        return ret;
    }

    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        if( p->attribute[i].location < 0 && strlen(p->attribute[i].name) > 0 ) {
            log_warn(__FILE__, __LINE__, "shader \"%s\" attribute \"%s\" has a name but no location\n", p->name, p->attribute[i].name);
            ret = true;
        }

        if( p->attribute[i].location > -1 ) {
            if( strlen(p->attribute[i].name) == 0 ) {
                log_warn(__FILE__, __LINE__, "shader \"%s\" attribute location %d cached without a name\n", p->name, p->attribute[i].location);
                ret = true;
            }
        }
    }

    for( int32_t i = 0; i < MAX_SHADER_UNIFORMS; i++ ) {
        if( p->uniform[i].location < 0 && strlen(p->uniform[i].name) > 0 ) {
            log_warn(__FILE__, __LINE__, "shader \"%s\" uniform \"%s\" has a name but no location\n", p->name, p->uniform[i].name);
            ret = true;
        }

        if( p->uniform[i].location > -1 ) {
            if( strlen(p->uniform[i].name) == 0 ) {
                log_warn(__FILE__, __LINE__, "shader \"%s\" uniform location %d cached without a name\n", p->name, p->uniform[i].location);
                ret = true;
            }
        }
    }

    p->verified = true;

    return ret;
}

bool shader_warn_locations(struct Shader* p) {
    bool ret = false;

    if( ! p->verified ) {
        shader_verify_locations(p);
    }

    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        if( p->attribute[i].location > -1 ) {
            if( p->attribute[i].unset && p->attribute[i].warn_once ) {
                log_warn(__FILE__, __LINE__, "shader \"%s\" attribute \"%s\" never set\n", p->name, p->attribute[i].name);
                ret = true;
                p->attribute[i].warn_once = false;
            }
        }
    }

    for( int32_t i = 0; i < MAX_SHADER_UNIFORMS; i++ ) {
        if( p->uniform[i].location > -1 ) {
            if( p->uniform[i].unset && p->uniform[i].warn_once ) {
                log_warn(__FILE__, __LINE__, "shader \"%s\" uniform \"%s\" never set\n", p->name, p->uniform[i].name);
                ret = true;
                p->uniform[i].warn_once = false;
            }
        }
    }

    return ret;
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
        ogl_debug( location = glGetAttribLocation(shader->program, name) );

        // - I had this as assertion, but a warning is better so then I can just hack glsl code and not have to worry if
        // there is a shader_add_attribute/uniform somewhere thats going to fail when I uncomment some input
        if( location > -1 ) {
            shader->attribute[attribute_index].location = location;

            //strncpy(shader->attribute[attribute_index].name, name, strlen(name)+1);
            shader->attribute[attribute_index].name[0] = '\0';
            strncat(shader->attribute[attribute_index].name, name, strlen(name));

            shader->verified = false;
        } else {
            log_warn(__FILE__, __LINE__, "could not add attribute %d location \"%s\" to shader \"%s\"\n", attribute_index, name, shader->name);
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
        ogl_debug( location = glGetUniformLocation(shader->program, name); );

        if( location > - 1 ) {
            shader->uniform[uniform_index].location = location;

            shader->uniform[uniform_index].name[0] = '\0';
            strncat(shader->uniform[uniform_index].name, name, strlen(name));

            shader->verified = false;
        } else {
            log_warn(__FILE__, __LINE__, "could not add uniform %d location \"%s\" to shader \"%s\"\n", uniform_index, name, shader->name);
        }
    }

    return location;
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

GLint shader_set_uniform_matrices(struct Shader* shader, const Mat projection_matrix, const Mat view_matrix, const Mat model_matrix) {
    log_assert( shader != NULL );
    log_assert( projection_matrix != NULL );
    log_assert( view_matrix != NULL );
    log_assert( model_matrix != NULL );
    log_assert( shader->program > 0 );

    if( ! shader->verified ) {
        shader_verify_locations(shader);
    }

    GLint ret = -1;
    GLint mvp_loc = shader->uniform[SHADER_UNIFORM_MVP_MATRIX].location;
    GLint projection_loc = shader->uniform[SHADER_UNIFORM_PROJECTION_MATRIX].location;
    GLint view_loc = shader->uniform[SHADER_UNIFORM_VIEW_MATRIX].location;
    GLint model_loc = shader->uniform[SHADER_UNIFORM_MODEL_MATRIX].location;
    log_assert( mvp_loc > -1 || projection_loc > -1 || view_loc > -1 || model_loc > -1 );

    if( mvp_loc > -1 ) {
        Mat mvp_matrix = {0};
        mat_mul(model_matrix, view_matrix, mvp_matrix);
        mat_mul(mvp_matrix, projection_matrix, mvp_matrix);
        ogl_debug( glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp_matrix) );
        shader->uniform[SHADER_UNIFORM_MVP_MATRIX].unset = false;
        ret = mvp_loc;
    }

    if( mvp_loc < 0 ) {
        if( projection_loc > -1 ) {
            ogl_debug( glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection_matrix) );
            shader->uniform[SHADER_UNIFORM_PROJECTION_MATRIX].unset = false;
            ret = projection_loc;
        }

        if( view_loc > -1 ) {
            ogl_debug( glUniformMatrix4fv(view_loc, 1, GL_FALSE, view_matrix) );
            shader->uniform[SHADER_UNIFORM_VIEW_MATRIX].unset = false;
            ret = view_loc;
        }

        if( model_loc > -1 ) {
            ogl_debug( glUniformMatrix4fv(model_loc, 1, GL_FALSE, model_matrix) );
            shader->uniform[SHADER_UNIFORM_MODEL_MATRIX].unset = false;
            ret = model_loc;
        }
    }

    GLint normal_loc = shader->uniform[SHADER_UNIFORM_NORMAL_MATRIX].location;
    if( normal_loc > -1 ) {
        Mat normal_matrix = {0};
        mat_copy4f(model_matrix, normal_matrix);
        ogl_debug( glUniformMatrix4fv(normal_loc, 1, GL_FALSE, normal_matrix) );
        shader->uniform[SHADER_UNIFORM_NORMAL_MATRIX].unset = false;
        ret = normal_loc;
    }

    return ret;
}

GLint shader_set_uniform_1f(struct Shader* shader, int32_t uniform_index, uint32_t size, GLenum type, void* data) {
    log_assert( shader->program > 0 );
    log_assert( uniform_index >= 0 );
    log_assert( uniform_index <= MAX_SHADER_UNIFORMS );
    log_assert( size <= 4 );
    log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE );
    log_assert( data != NULL );

    if( ! shader->verified ) {
        shader_verify_locations(shader);
    }

    GLint location = shader->uniform[uniform_index].location;
    log_assert( location > -1 );
    log_assert( strlen(shader->uniform[uniform_index].name) > 0 );

    GLfloat float_value = 0.0f;
    switch(type) {
        case GL_FLOAT: {
            float_value = size > 0 ? ((float*)data)[0] : 0.0f;
            break;
        }
        case GL_UNSIGNED_BYTE: {
            float_value = size > 0 ? ((uint8_t*)data)[0] / 255.0f : 0.0f;
            break;
        }
        default: log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE );
    }

    // glUniform changes state of the program, so it needs to be run after glUseProgram
    // that does not apply to glVertexAttribPointer, which only changes global state
    ogl_debug( glUseProgram(shader->program);
               glUniform1f(location, float_value); );
    shader->uniform[uniform_index].unset = false;

    return location;
}

GLint shader_set_uniform_3f(struct Shader* shader, int32_t uniform_index, uint32_t size, GLenum type, void* data) {
    log_assert( shader->program > 0 );
    log_assert( uniform_index >= 0 );
    log_assert( uniform_index <= MAX_SHADER_UNIFORMS );
    log_assert( size <= 4 );
    log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE );
    log_assert( data != NULL );

    if( ! shader->verified ) {
        shader_verify_locations(shader);
    }

    GLint location = shader->uniform[uniform_index].location;
    log_assert( location > -1 );
    log_assert( strlen(shader->uniform[uniform_index].name) > 0 );

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
            float_array[0] = size > 0 ? ((uint8_t*)data)[0] / 255.0f : 0.0f;
            float_array[1] = size > 1 ? ((uint8_t*)data)[1] / 255.0f : 0.0f;
            float_array[2] = size > 2 ? ((uint8_t*)data)[2] / 255.0f : 0.0f;
            break;
        }
        default: log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE );
    }

    if( size < 3 ) {
        log_warn(__FILE__, __LINE__, "setting uniform 3f with only %df data\n", size);
    }

    // glUniform changes state of the program, so it needs to be run after glUseProgram
    // that does not apply to glVertexAttribPointer, which only changes global state
    ogl_debug( glUseProgram(shader->program);
               glUniform3f(location, float_array[0], float_array[1], float_array[2]); );
    shader->uniform[uniform_index].unset = false;

    return location;
}

GLint shader_set_uniform_4f(struct Shader* shader, int32_t uniform_index, uint32_t size, GLenum type, void* data) {
    log_assert( shader != NULL );
    log_assert( shader->program > 0 );
    log_assert( uniform_index >= 0 );
    log_assert( uniform_index <= MAX_SHADER_UNIFORMS );
    log_assert( size <= 4 );
    log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE );
    log_assert( data != NULL );

    if( ! shader->verified ) {
        shader_verify_locations(shader);
    }

    GLint location = shader->uniform[uniform_index].location;
    log_assert( location > -1 );
    log_assert( strlen(shader->uniform[uniform_index].name) > 0 );

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
            float_array[0] = size > 0 ? ((uint8_t*)data)[0] / 255.0f : 0.0f;
            float_array[1] = size > 1 ? ((uint8_t*)data)[1] / 255.0f : 0.0f;
            float_array[2] = size > 2 ? ((uint8_t*)data)[2] / 255.0f : 0.0f;
            float_array[3] = size > 3 ? ((uint8_t*)data)[3] / 255.0f : 0.0f;
            break;
        }
        default: log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE );
    }

    if( size < 4 ) {
        log_warn(__FILE__, __LINE__, "setting uniform 4f with only %df data\n", size);
    }

    // glUniform changes state of the program, so it needs to be run after glUseProgram
    // that does not apply to glVertexAttribPointer, which only changes global state
    ogl_debug( glUseProgram(shader->program);
               glUniform4f(location, float_array[0], float_array[1], float_array[2], float_array[3]); );
    shader->uniform[uniform_index].unset = false;

    return location;
}

GLint shader_set_sampler2D(struct Shader* shader, int32_t uniform_index, GLenum texture_type, GLuint texture_unit, GLuint texture_id) {
    log_assert( shader != NULL );
    log_assert( shader->program > 0 );
    log_assert( uniform_index >= 0 );
    log_assert( uniform_index <= MAX_SHADER_UNIFORMS );
    log_assert( texture_unit < 8 );
    log_assert( texture_id > 0 );
    log_assert( GL_TEXTURE7 == GL_TEXTURE0 + 7 );

    if( ! shader->verified ) {
        shader_verify_locations(shader);
    }

    GLint location = shader->uniform[uniform_index].location;
    log_assert( location > -1 );
    log_assert( strlen(shader->uniform[uniform_index].name) > 0 );

    log_assert( texture_unit < INT_MAX );
    ogl_debug( glUniform1i(location, (GLint)texture_unit) ;
               glActiveTexture(GL_TEXTURE0 + texture_unit);
               glBindTexture(texture_type, texture_id); );
    shader->uniform[uniform_index].unset = false;

    return location;
}

GLint shader_set_attribute(struct Shader* shader, int32_t attribute_i, GLuint buffer, GLint c_num, GLenum c_type, GLsizei stride, const GLvoid* p) {
    log_assert( shader != NULL );
    log_assert( attribute_i >= SHADER_ATTRIBUTE_VERTICES );
    log_assert( attribute_i < MAX_SHADER_ATTRIBUTES );
    log_assert( c_num >= 0 );
    log_assert( c_num <= 4 );

    if( ! shader->verified ) {
        shader_verify_locations(shader);
    }

    if( buffer == 0 || c_num == 0 ) {
        return -1;
    }

    GLint location = shader->attribute[attribute_i].location;
    log_assert( location > -1 );
    log_assert( strlen(shader->attribute[attribute_i].name) > 0 );

    ogl_debug( glEnableVertexAttribArray((GLuint)location);
               glBindBuffer(GL_ARRAY_BUFFER, buffer);
               // - glVertexAttribPointer requires a vao to be bound in core 3.0+, if none is bound,
               // rendering will fail here
               glVertexAttribPointer((GLuint)location, c_num, c_type, GL_TRUE, stride, p); );
    shader->attribute[attribute_i].unset = false;

    return location;
}
