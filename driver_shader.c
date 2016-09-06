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
const char* global_shader_sampler_names[MAX_SHADER_SAMPLER] = {0};

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

    for( size_t i = 0; i < MAX_SHADER_SAMPLER; i++ ) {
        global_shader_sampler_names[i] = "invalid_sampler";
    }

    global_shader_attribute_names[SHADER_ATTRIBUTE_VERTEX] = "vertex";
    global_shader_attribute_names[SHADER_ATTRIBUTE_VERTEX_NORMAL] = "vertex_normal";
    global_shader_attribute_names[SHADER_ATTRIBUTE_VERTEX_TEXCOORD] = "vertex_texcoord";
    global_shader_attribute_names[SHADER_ATTRIBUTE_DIFFUSE_COLOR] = "diffuse_color";

    global_shader_attribute_names[SHADER_ATTRIBUTE_AMBIENT_COLOR] = "ambient_color";
    global_shader_attribute_names[SHADER_ATTRIBUTE_SPECULAR_COLOR] = "specular_color";
    global_shader_attribute_names[SHADER_ATTRIBUTE_INSTANCE_ID] = "instance_id";
    global_shader_attribute_names[SHADER_ATTRIBUTE_PREV_VERTEX] = "prev_vertex";
    global_shader_attribute_names[SHADER_ATTRIBUTE_NEXT_VERTEX] = "next_vertex";
    global_shader_attribute_names[SHADER_ATTRIBUTE_LINE_THICKNESS] = "line_thickness";
    global_shader_attribute_names[SHADER_ATTRIBUTE_BARYCENTRIC_COORDINATE] = "barycentric_coordinate";
    global_shader_attribute_names[SHADER_ATTRIBUTE_SMOOTH_NORMAL] = "smooth_normal";

    global_shader_uniform_names[SHADER_UNIFORM_MVP_MATRIX] = "mvp_matrix";
    global_shader_uniform_names[SHADER_UNIFORM_MODEL_MATRIX] = "model_matrix";
    global_shader_uniform_names[SHADER_UNIFORM_VIEW_MATRIX] = "view_matrix";
    global_shader_uniform_names[SHADER_UNIFORM_PROJECTION_MATRIX] = "projection_matrix";
    global_shader_uniform_names[SHADER_UNIFORM_NORMAL_MATRIX] = "normal_matrix";

    global_shader_uniform_names[SHADER_UNIFORM_AMBIENT_LIGHT] = "ambient_light";
    global_shader_uniform_names[SHADER_UNIFORM_DIFFUSE_LIGHT] = "diffuse_light";
    global_shader_uniform_names[SHADER_UNIFORM_SPECULAR_LIGHT] = "specular_light";
    global_shader_uniform_names[SHADER_UNIFORM_LIGHT_DIRECTION] = "light_direction";
    global_shader_uniform_names[SHADER_UNIFORM_LIGHT_POSITION] = "light_position";
    global_shader_uniform_names[SHADER_UNIFORM_LIGHT_ATTENUATION] = "light_attenuation";
    global_shader_uniform_names[SHADER_UNIFORM_MATERIAL_SHININESS] = "material_shininess";
    global_shader_uniform_names[SHADER_UNIFORM_MATERIAL_COEFFICIENTS] = "material_coefficients";
    global_shader_uniform_names[SHADER_UNIFORM_EYE_POSITION] = "eye_position";

    global_shader_uniform_names[SHADER_UNIFORM_ASPECT_RATIO] = "aspect_ratio";
    global_shader_uniform_names[SHADER_UNIFORM_LINE_Z_SCALING] = "line_z_scaling";
    global_shader_uniform_names[SHADER_UNIFORM_ENABLE_TEXTURE] = "enable_texture";

    global_shader_sampler_names[SHADER_SAMPLER_DIFFUSE_TEXTURE] = "diffuse_texture";

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

    for( int32_t j = 0; j < MAX_SHADER_SAMPLER; j++ ) {
        p->sampler[j].location = -1;
        p->sampler[j].name[0] = '\0';
        p->sampler[j].unset = true;
        p->sampler[j].warn_once = true;
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
        log_info(__FILE__, __LINE__, "creating shader \"%s\" from files: \"%s\", \"%s\"\n", name, vertex_file, fragment_file);
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

void shader_use_program(const struct Shader* p) {
    if( p != NULL ) {
        ogl_debug( glUseProgram(p->program) );
    } else {
        ogl_debug( glUseProgram(0) );
    }
}

void shader_setup_locations(struct Shader* p) {
    ogl_debug( glUseProgram(p->program); );

    // - go through all attributes and get their locations
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

    // - go through all uniforms, get their location and set defaults
    GLint num_active_uniforms = 0;
    glGetProgramiv(p->program, GL_ACTIVE_UNIFORMS, &num_active_uniforms);
    int32_t num_cached_uniforms = 0;
    for( int32_t i = 0; i < MAX_SHADER_UNIFORMS; i++ ) {
        p->uniform[i].name[0] = '\0';
        p->uniform[i].location = -1;
        p->uniform[i].unset = true;
        p->uniform[i].warn_once = true;

        const char* uniform_name = global_shader_uniform_names[i];
        GLint location = glGetUniformLocation(p->program, global_shader_uniform_names[i]);

        if( location > -1 ) {
            p->uniform[i].name[0] = '\0';
            log_assert( strlen(uniform_name) < 256 );
            strncat(p->uniform[i].name, uniform_name, strlen(uniform_name));
            p->uniform[i].location = location;
            num_cached_uniforms += 1;

            // - set defaults for all uniforms
            // - this might cause problems when an glUniform call and type in the shader don't match
            switch(i) {
                case SHADER_UNIFORM_MVP_MATRIX: ogl_debug(glUniformMatrix4fv(location, 1, GL_FALSE, (Mat)IDENTITY_MAT)); break;
                case SHADER_UNIFORM_MODEL_MATRIX: ogl_debug(glUniformMatrix4fv(location, 1, GL_FALSE, (Mat)IDENTITY_MAT)); break;
                case SHADER_UNIFORM_VIEW_MATRIX: ogl_debug(glUniformMatrix4fv(location, 1, GL_FALSE, (Mat)IDENTITY_MAT)); break;
                case SHADER_UNIFORM_PROJECTION_MATRIX: ogl_debug(glUniformMatrix4fv(location, 1, GL_FALSE, (Mat)IDENTITY_MAT)); break;
                case SHADER_UNIFORM_NORMAL_MATRIX: ogl_debug(glUniformMatrix4fv(location, 1, GL_FALSE, (Mat)IDENTITY_MAT)); break;

                case SHADER_UNIFORM_AMBIENT_LIGHT: ogl_debug(glUniform4f(location, 1.0f, 1.0f, 1.0f, 1.0f)); break;
                case SHADER_UNIFORM_DIFFUSE_LIGHT: ogl_debug(glUniform4f(location, 1.0f, 1.0f, 1.0f, 1.0f)); break;
                case SHADER_UNIFORM_SPECULAR_LIGHT: ogl_debug(glUniform4f(location, 1.0f, 1.0f, 1.0f, 1.0f)); break;
                case SHADER_UNIFORM_LIGHT_DIRECTION: ogl_debug(glUniform3f(location, 0.0f, 0.0f, 1.0f)); break;
                case SHADER_UNIFORM_LIGHT_POSITION: ogl_debug(glUniform3f(location, 0.0f, 1.0f, 0.0f)); break;
                case SHADER_UNIFORM_LIGHT_ATTENUATION: ogl_debug(glUniform1f(location, 1.0f)); break;
                case SHADER_UNIFORM_MATERIAL_SHININESS: ogl_debug(glUniform1f(location, 1.0f)); break;
                case SHADER_UNIFORM_MATERIAL_COEFFICIENTS: ogl_debug(glUniform3f(location, 1.0f, 1.0f, 1.0f)); break;
                case SHADER_UNIFORM_EYE_POSITION: ogl_debug(glUniform3f(location, 0.0f, 0.0f, -1.0f)); break;

                case SHADER_UNIFORM_ASPECT_RATIO: ogl_debug(glUniform1f(location, (float)1920/1080)); break;
                case SHADER_UNIFORM_LINE_Z_SCALING: ogl_debug(glUniform1f(location, 0.0f)); break;
                case SHADER_UNIFORM_ENABLE_TEXTURE: ogl_debug(glUniform1i(location, 0)); break;
                default: break;
            };
        }
    }

    // - difference to the uniforms and attributes is that samplers get set here, with the glUniform
    // call below a fixed texture unit is set that is always going to be the same
    for( int32_t i = 0; i < MAX_SHADER_SAMPLER; i++ ) {
        p->sampler[i].name[0] = '\0';
        p->sampler[i].location = -1;
        p->sampler[i].unset = true;
        p->sampler[i].warn_once = true;

        const char* sampler_name = global_shader_sampler_names[i];
        GLint location = glGetUniformLocation(p->program, global_shader_sampler_names[i]);

        if( location > -1 ) {
            p->sampler[i].name[0] = '\0';
            log_assert( strlen(sampler_name) < 256 );
            strncat(p->sampler[i].name, sampler_name, strlen(sampler_name));
            p->sampler[i].location = location;
            num_cached_uniforms += 1;

            GLuint texture_unit = i % 8;
            glUniform1i(location, texture_unit);
            p->sampler[i].unset = false;
        }
    }

    if( num_active_uniforms > num_cached_uniforms) {
        log_warn(__FILE__, __LINE__, "shader \"%s\" has %d unknown uniforms\n", p->name, num_active_uniforms - num_cached_uniforms);
    }

    ogl_debug( glUseProgram(0); );

    shader_verify_locations(p);
}

void shader_verify_locations(struct Shader* p) {

    if( p->verified ) {
        return;
    }

    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        if( p->attribute[i].location < 0 && strlen(p->attribute[i].name) > 0 ) {
            log_warn(__FILE__, __LINE__, "shader \"%s\" attribute \"%s\" has a name but no location\n", p->name, p->attribute[i].name);
        }

        if( p->attribute[i].location > -1 ) {
            if( strlen(p->attribute[i].name) == 0 ) {
                log_warn(__FILE__, __LINE__, "shader \"%s\" attribute location %d cached without a name\n", p->name, p->attribute[i].location);
            }
        }
    }

    for( int32_t i = 0; i < MAX_SHADER_UNIFORMS; i++ ) {
        if( p->uniform[i].location < 0 && strlen(p->uniform[i].name) > 0 ) {
            log_warn(__FILE__, __LINE__, "shader \"%s\" uniform \"%s\" has a name but no location\n", p->name, p->uniform[i].name);
        }

        if( p->uniform[i].location > -1 ) {
            if( strlen(p->uniform[i].name) == 0 ) {
                log_warn(__FILE__, __LINE__, "shader \"%s\" uniform location %d cached without a name\n", p->name, p->uniform[i].location);
            }
        }
    }

    for( int32_t i = 0; i < MAX_SHADER_SAMPLER; i++ ) {
        if( p->sampler[i].location < 0 && strlen(p->sampler[i].name) > 0 ) {
            log_warn(__FILE__, __LINE__, "shader \"%s\" sampler \"%s\" has a name but no location\n", p->name, p->sampler[i].name);
        }

        if( p->sampler[i].location > -1 ) {
            if( strlen(p->sampler[i].name) == 0 ) {
                log_warn(__FILE__, __LINE__, "shader \"%s\" sampler location %d cached without a name\n", p->name, p->sampler[i].location);
            }
        }
    }

    p->verified = true;
}

void shader_warn_locations(struct Shader* p, GLint* attribute_locations) {
    shader_verify_locations(p);

    for( int32_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++ ) {
        if( p->attribute[i].warn_once ) {
            if( p->attribute[i].location > -1 ) {
                if( p->attribute[i].unset ) {
                    log_warn(__FILE__, __LINE__, "shader \"%s\" attribute \"%s\" never set\n", p->name, p->attribute[i].name);
                }
            } else if( attribute_locations != NULL && attribute_locations[i] > -1 ) {
                log_warn(__FILE__, __LINE__, "the shader \"%s\" is missing a location for the vbo attribute \"%s\"\n", p->name, global_shader_attribute_names[i]);
            }

            p->attribute[i].warn_once = false;
        }
    }

    for( int32_t i = 0; i < MAX_SHADER_UNIFORMS; i++ ) {
        if( p->uniform[i].location > -1 ) {
            if( p->uniform[i].unset && p->uniform[i].warn_once ) {
                log_warn(__FILE__, __LINE__, "shader \"%s\" uniform \"%s\" never set\n", p->name, p->uniform[i].name);
                p->uniform[i].warn_once = false;
            }
        }
    }

#ifdef DEBUG
    for( int32_t i = 0; i < MAX_SHADER_SAMPLER; i++ ) {
        if( p->sampler[i].location > -1 && p->sampler[i].warn_once ) {
            GLuint texture_unit = i % 8;
            log_assert( texture_unit < MAX_SHADER_TEXTURE_UNITS );

            GLint texture_id = 0;
            ogl_debug( glActiveTexture(GL_TEXTURE0 + texture_unit);
                       glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture_id); );

            if( texture_id == 0 ) {
                ogl_debug( glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &texture_id) );
            }

            if( texture_id == 0 ) {
                log_warn(__FILE__, __LINE__, "shader \"%s\" sampler \"%s\" texture unit %d has no texture id bound\n", p->name, p->sampler[i].name, texture_unit);
                p->sampler[i].warn_once = false;
            }
        }
    }
#endif
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

GLint shader_add_sampler(struct Shader* shader, int32_t sampler_index, const char* name) {
    size_t name_length = strlen(name);
    log_assert( name_length > 0 );
    log_assert( name_length < 256 );
    log_assert( shader->program > 0 );

    GLint location = -1;
    if( shader->sampler[sampler_index].location > -1 ) {
        location = shader->sampler[sampler_index].location;
    } else {
        ogl_debug( location = glGetUniformLocation(shader->program, name); );

        if( location > - 1 ) {
            shader->sampler[sampler_index].location = location;

            shader->sampler[sampler_index].name[0] = '\0';
            strncat(shader->sampler[sampler_index].name, name, strlen(name));

            shader->verified = false;
        } else {
            log_warn(__FILE__, __LINE__, "could not add sampler %d location \"%s\" to shader \"%s\"\n", sampler_index, name, shader->name);
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

    for( int32_t i = 0; i < MAX_SHADER_SAMPLER; i++ ) {
        if( strlen(shader->sampler[i].name) > 0 ) {
            fprintf(f, "shader->sampler[%d].name: %s\n", i, shader->sampler[i].name);
            fprintf(f, "shader->sampler[%d].location: %d\n", i, shader->sampler[i].location);
        }
    }
}

GLint shader_set_uniform_matrices(struct Shader* shader, GLuint program, const Mat projection_matrix, const Mat view_matrix, const Mat model_matrix) {
    log_assert( shader != NULL );
    log_assert( projection_matrix != NULL );
    log_assert( view_matrix != NULL );
    log_assert( model_matrix != NULL );
    log_assert( shader->program > 0 );

    shader_verify_locations(shader);

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
        if( program > 0 ) {
            log_assert( shader->program == program );
            ogl_debug( glUseProgram(program);
                       glUniformMatrix4fv(normal_loc, 1, GL_FALSE, normal_matrix);
                       glUseProgram(0) );
        } else {
            ogl_debug( glUniformMatrix4fv(normal_loc, 1, GL_FALSE, normal_matrix) );
        }
        shader->uniform[SHADER_UNIFORM_NORMAL_MATRIX].unset = false;
        ret = normal_loc;
    }

    return ret;
}

GLint shader_set_uniform_1f(struct Shader* shader, GLuint program, int32_t uniform_index, uint32_t size, GLenum type, void* data) {
    log_assert( shader->program > 0 );
    log_assert( uniform_index >= 0 );
    log_assert( uniform_index <= MAX_SHADER_UNIFORMS );
    log_assert( size <= 4 );
    log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE || type == GL_INT );
    log_assert( data != NULL );

    shader_verify_locations(shader);

    GLint location = shader->uniform[uniform_index].location;
    if( location == -1 ) {
        log_warn(__FILE__, __LINE__, "can not set uniform 1f, shader \"%s\" has no location \"%s\"\n", shader->name, global_shader_uniform_names[uniform_index]);
        return location;
    }

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
        case GL_INT: {
            float_value = size > 0 ? ((int32_t*)data)[0] : 0.0f;
            break;
        }
        default: log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE || type == GL_INT );
    }

    // - glUniform changes state of the program, so it needs to be run after glUseProgram
    // that does not apply to glVertexAttribPointer, which only changes global state
    if( program > 0 ) {
        log_assert( shader->program == program );
        ogl_debug( glUseProgram(program);
                   glUniform1f(location, float_value);
                   glUseProgram(0) );
    } else {
        ogl_debug( glUniform1f(location, float_value); );
    }
    shader->uniform[uniform_index].unset = false;

    return location;
}

GLint shader_set_uniform_1i(struct Shader* shader, GLuint program, int32_t uniform_index, uint32_t size, GLenum type, void* data) {
    log_assert( shader->program > 0 );
    log_assert( uniform_index >= 0 );
    log_assert( uniform_index <= MAX_SHADER_UNIFORMS );
    log_assert( size <= 4 );
    log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE || type == GL_INT );
    log_assert( data != NULL );

    shader_verify_locations(shader);

    GLint location = shader->uniform[uniform_index].location;
    if( location == -1 ) {
        log_warn(__FILE__, __LINE__, "can not set uniform 1i, shader \"%s\" has no location \"%s\"\n", shader->name, global_shader_uniform_names[uniform_index]);
        return location;
    }

    log_assert( strlen(shader->uniform[uniform_index].name) > 0 );

    GLint int_value = 0;
    switch(type) {
        case GL_FLOAT: {
            int_value = size > 0 ? ((float*)data)[0] : 0;
            break;
        }
        case GL_UNSIGNED_BYTE: {
            int_value = size > 0 ? ((uint8_t*)data)[0] : 0;
            break;
        }
        case GL_INT: {
            int_value = size > 0 ? ((int32_t*)data)[0] : 0;
            break;
        }
        default: log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE || type == GL_INT );
    }

    // - glUniform changes state of the program, so it needs to be run after glUseProgram
    // that does not apply to glVertexAttribPointer, which only changes global state
    if( program > 0 ) {
        log_assert( shader->program == program );
        ogl_debug( glUseProgram(program);
                   glUniform1i(location, int_value);
                   glUseProgram(0) );
    } else {
        ogl_debug( glUniform1i(location, int_value); );
    }
    shader->uniform[uniform_index].unset = false;

    return location;
}

GLint shader_set_uniform_3f(struct Shader* shader, GLuint program, int32_t uniform_index, uint32_t size, GLenum type, void* data) {
    log_assert( shader->program > 0 );
    log_assert( uniform_index >= 0 );
    log_assert( uniform_index <= MAX_SHADER_UNIFORMS );
    log_assert( size <= 4 );
    log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE );
    log_assert( data != NULL );

    shader_verify_locations(shader);

    GLint location = shader->uniform[uniform_index].location;
    if( location == -1 ) {
        log_warn(__FILE__, __LINE__, "can not set uniform 3f, shader \"%s\" has no location \"%s\"\n", shader->name, global_shader_uniform_names[uniform_index]);
        return location;
    }

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
    if( program > 0 ) {
        log_assert( shader->program == program );
        ogl_debug( glUseProgram(program);
                   glUniform3f(location, float_array[0], float_array[1], float_array[2]); ;
                   glUseProgram(0); );
    } else {
        ogl_debug( glUniform3f(location, float_array[0], float_array[1], float_array[2]); );
    }
    shader->uniform[uniform_index].unset = false;

    return location;
}

GLint shader_set_uniform_4f(struct Shader* shader, GLuint program, int32_t uniform_index, uint32_t size, GLenum type, void* data) {
    log_assert( shader != NULL );
    log_assert( shader->program > 0 );
    log_assert( uniform_index >= 0 );
    log_assert( uniform_index <= MAX_SHADER_UNIFORMS );
    log_assert( size <= 4 );
    log_assert( type == GL_FLOAT || type == GL_UNSIGNED_BYTE );
    log_assert( data != NULL );

    shader_verify_locations(shader);

    GLint location = shader->uniform[uniform_index].location;
    if( location == -1 ) {
        log_warn(__FILE__, __LINE__, "can not set uniform 4f, shader \"%s\" has no location \"%s\"\n", shader->name, global_shader_uniform_names[uniform_index]);
        return location;
    }

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
    if( program > 0 ) {
        log_assert( shader->program == program );
        ogl_debug( glUseProgram(program);
                   glUniform4f(location, float_array[0], float_array[1], float_array[2], float_array[3]);
                   glUseProgram(0) );
    } else {
        ogl_debug( glUniform4f(location, float_array[0], float_array[1], float_array[2], float_array[3]) );
    }
    shader->uniform[uniform_index].unset = false;

    return location;
}

GLint shader_set_attribute(struct Shader* shader, int32_t attribute_i, GLuint buffer, GLint c_num, GLenum c_type, GLsizei stride, const GLvoid* p) {
    log_assert( shader != NULL );
    log_assert( attribute_i >= SHADER_ATTRIBUTE_VERTEX );
    log_assert( attribute_i < MAX_SHADER_ATTRIBUTES );
    log_assert( c_num >= 0 );
    log_assert( c_num <= 4 );

    shader_verify_locations(shader);

    if( buffer == 0 || c_num == 0 ) {
        return -1;
    }

    GLint location = shader->attribute[attribute_i].location;
    if( location == -1 ) {
        log_warn(__FILE__, __LINE__, "can not set attribute, shader \"%s\" has no location \"%s\"\n", shader->name, global_shader_attribute_names[attribute_i]);
        return location;
    }

    log_assert( location > -1 );
    log_assert( strlen(shader->attribute[attribute_i].name) > 0 );

    // - is here potential for optimization? can I just glEnableVertexAttribArray the same location and
    // get the assigned state back?
    ogl_debug( glEnableVertexAttribArray((GLuint)location);
               glBindBuffer(GL_ARRAY_BUFFER, buffer);
               // - glVertexAttribPointer requires a vao to be bound in core 3.0+, if none is bound,
               // rendering will fail here
               glVertexAttribPointer((GLuint)location, c_num, c_type, GL_TRUE, stride, p); );
    shader->attribute[attribute_i].unset = false;

    return location;
}
