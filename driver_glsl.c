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

#include "driver_glsl.h"

void glsl_debug_info_log(GLuint object) {
    GLint log_length;
    char *log;

    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);

    log_assert( log_length > 0 );
    log = malloc((size_t)log_length);
    glGetShaderInfoLog(object, log_length, NULL, log);
    fprintf(stderr, "%s", log);
    free(log);
}

GLuint glsl_compile_source(GLenum type, const char* shader_source) {
    log_assert( type == GL_FRAGMENT_SHADER || type == GL_VERTEX_SHADER );

    // check for cute header in source and warn if this looks like it is
    // not something that was distributed with cute
    if( ! (strlen(shader_source) > 6 &&
           shader_source[0] == '/' &&
           shader_source[1] == '/' &&
           shader_source[2] == 'C' &&
           shader_source[3] == 'U' &&
           shader_source[4] == 'T' &&
           shader_source[5] == 'E') )
    {
        log_warn(stderr, __FILE__, __LINE__, "%s\n does not look like cute3d glsl code\n", shader_source);
    }

    size_t test_length = 0;
    const GLchar* compat_source = NULL;
    if( type == GL_VERTEX_SHADER ) {
        test_length = strlen(GLSL_VERT_COMPAT);
        compat_source = GLSL_VERT_COMPAT;
    } else if( type == GL_FRAGMENT_SHADER ) {
        test_length = strlen(GLSL_FRAG_COMPAT);
        compat_source = GLSL_FRAG_COMPAT;
    }
    log_assert( test_length > 0 );
    log_assert( test_length < INT_MAX );
    GLint compat_length = (GLint)test_length;

    test_length = strlen(shader_source);
    log_assert( test_length > 0 );
    log_assert( test_length < INT_MAX );
    GLint source_length = (GLint)test_length;

    // the final shader source is made up from the compatibilty source defined
    // in GLSL_VERT_COMPAT and GLSL_FRAG_COMPAT, and the actual shader_source
    // both have their length and their pointer are put into source_array, their
    // length are put into length_array, glShaderSource takes those and assembles
    // the final source and puts it into shader, glCompileShader finally produces
    // the compiled shader
    const GLchar* source_array[2] = {compat_source, shader_source};
    GLint length_array[2] = {compat_length, source_length};

    GLuint shader = glCreateShader(type);
    log_assert( shader > 0 );

    glShaderSource(shader, 2, source_array, length_array);
    glCompileShader(shader);

    // check for compilation errors and inform the user about them
    GLint shader_ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
    if ( ! shader_ok ) {
        log_fail(stderr, __FILE__, __LINE__, "failed to compile:\n %s%s\n", compat_source, shader_source);
        glsl_debug_info_log(shader);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint glsl_compile_file(GLenum type, const char* filename) {
    FILE* file = fopen(filename, "rb");

    if( ! file ) {
        log_fail(stderr, __FILE__, __LINE__, "could not open file %s\n", filename);
        return 0;
    }

    long filepos;
    fseek(file, 0, SEEK_END);
    filepos = ftell(file);
    fseek(file, 0, SEEK_SET);

    log_assert( filepos > 0 );
    size_t length = (size_t)filepos;

    GLchar* source = malloc(length);
    if( source == NULL ) {
        fclose(file);
        return 0;
    } else {
        fread(source, length, 1, file);
        fclose(file);
    }

    log_info(stderr, __FILE__, __LINE__, "compiling: %s\n", filename);
    GLuint id = glsl_compile_source(type, source);
    if( ! id ) {
        log_fail(stderr, __FILE__, __LINE__, "compilation failed in: %s\n", filename);
    }
    return id;
}

GLuint glsl_create_program(GLuint vertex_shader, GLuint fragment_shader) {
    log_assert( vertex_shader > 0 );
    log_assert( fragment_shader > 0 );

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    return program;
}


GLuint glsl_link_program(GLuint program) {
    glLinkProgram(program);

    GLint program_ok;
    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
    if( ! program_ok ) {
        fprintf(stderr, "failed to link shader program:\n");
        glsl_debug_info_log(program);
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

/* GLuint glsl_make_program(const char *vertex_source, const char* fragment_source) { */
/*     GLuint vertex = glsl_compile_source(GL_VERTEX_SHADER, vertex_source); */
/*     GLuint fragment = glsl_compile_source(GL_FRAGMENT_SHADER, fragment_source); */

/*     return glsl_link_program(vertex, fragment); */
/* } */
