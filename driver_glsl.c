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
    fprintf(stdout, "%s", log);
    free(log);
}

GLuint glsl_compile_source(GLenum type, const char* prefix_source, const char* shader_source) {
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
        log_warn(__FILE__, __LINE__, "%s\n does not look like cute3d glsl code\n", shader_source);
    }

    size_t source_length = strlen(shader_source);
    log_assert( source_length > 0 );
    log_assert( source_length < INT_MAX );

    size_t prefix_length = strlen(prefix_source);
    log_assert( prefix_length > 0 );
    log_assert( prefix_length < INT_MAX );

    // - the final shader source is made up from the compatibilty source defined
    // in GLSL_VERT_COMPAT and GLSL_FRAG_COMPAT, and the actual shader_source
    // both have their length and their pointer are put into source_array, their
    // length are put into length_array, glShaderSource takes those and assembles
    // the final source and puts it into shader, glCompileShader finally produces
    // the compiled shader
    // - see above, instead of defines now using files
    const GLchar* source_array[2] = {prefix_source, shader_source};
    GLint length_array[2] = {(GLint)prefix_length, (GLint)source_length};

    GLuint shader = 0;
    shader = glCreateShader(type);
    log_assert( shader > 0 );

    glShaderSource(shader, 2, source_array, length_array);
    glCompileShader(shader);

    // check for compilation errors and inform the user about them
    GLint shader_ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
    if ( ! shader_ok ) {
        log_fail(__FILE__, __LINE__, "failed to compile:\n%s%s\n\n\n", prefix_source, shader_source);
        glsl_debug_info_log(shader);
        glDeleteShader(shader);
        goto finish;
    }

finish:
    return shader;
}

GLuint glsl_compile_file(GLenum type, const char* filename) {
    FILE* file = fopen(filename, "rb");

    if( ! file ) {
        log_fail(__FILE__, __LINE__, "could not open file %s\n", filename);
        return 0;
    }

    long filepos;
    fseek(file, 0, SEEK_END);
    filepos = ftell(file);
    fseek(file, 0, SEEK_SET);

    log_assert( filepos > 0 );

    GLchar* shader_source = malloc((size_t)filepos+1);
    GLuint id = 0;
    if( shader_source == NULL ) {
        fclose(file);
        goto finish_shader;
    } else {
        fread(shader_source, (size_t)filepos, 1, file);
        fclose(file);
        shader_source[filepos] = '\0';
    }

    size_t prefix_filename_length = strlen(filename) + 12;
    char* prefix_dirname = malloc(prefix_filename_length);
    for( size_t i = 0; i < prefix_filename_length; i++ ) {
        prefix_dirname[i] = '\0';
    }

    const char* last_slash_in_filename = strrchr(filename, '/');
    if( last_slash_in_filename != NULL ) {
        memcpy(prefix_dirname, filename, strlen(filename) - strlen(last_slash_in_filename));
    } else {
        prefix_dirname[0] = '\0';
    }

    // I need this compatibilty crap if I want to be able to deploy stuff on webgl/android
    // GLSL ES compatibilty is a MESS, targeted versions are:
    // GLSL ES 1  : 100
    // GLSL GL 3  : 130
    // GLSL ES 3  : 300 es (only for playing around so far)
    // maybe I can change the latter to 150 to get geometry shader, but I would not count on it,
    // for now I'll keep everything at version 100 manually so that I catch problems with GLSL ES
    // in my shader code, its just supported fine on my intel notebook
    //
    // https://github.com/mattdesl/lwjgl-basics/wiki/GLSL-Versions
    // http://bitiotic.com/blog/2013/09/24/opengl-es-shading-language-potholes-and-problems/
    // http://stackoverflow.com/questions/2631324/opengl-shading-language-backwards-compatibility
    //
    // "#extension GL_ARB_enhanced_layouts:require\n"
    // "#extension GL_ARB_ES2_compatibility:require\n"
    // "#extension GL_ARB_uniform_buffer_object:require\n"
    // #if __VERSION__ < 130\n#define in attribute\n#define out varying\n#endif\n
    //
    // - compat stuff used to be in defines GLSL_COMPAT_VERT and GLSL_COMPAT_FRAG, but now its loaded
    // from files shader/prefix.vert and shader/prefix.frag
    FILE* prefix_file = NULL;
    if( type == GL_VERTEX_SHADER ) {
        char prefix_vert_filename[prefix_filename_length];
        snprintf(prefix_vert_filename, prefix_filename_length, "%s/%s", prefix_dirname, "prefix.vert");

        prefix_file = fopen(prefix_vert_filename, "rb");

        if( ! prefix_file ) {
            log_fail(__FILE__, __LINE__, "could not open file %s\n", prefix_vert_filename);
            goto finish_dirname;
        }
    } else if( type == GL_FRAGMENT_SHADER ) {
        char prefix_frag_filename[prefix_filename_length];
        snprintf(prefix_frag_filename, prefix_filename_length, "%s/%s", prefix_dirname, "prefix.frag");

        prefix_file = fopen(prefix_frag_filename, "rb");

        if( ! prefix_file ) {
            log_fail(__FILE__, __LINE__, "could not open file %s\n", prefix_frag_filename);
            goto finish_dirname;
        }
    }
    log_assert( prefix_file != NULL );

    long prefix_length;
    fseek(prefix_file, 0, SEEK_END);
    prefix_length = ftell(prefix_file);
    fseek(prefix_file, 0, SEEK_SET);

    log_assert( prefix_length > 0 );
    log_assert( prefix_length < INT_MAX );

    GLchar* prefix_source = malloc((size_t)prefix_length+1);
    if( prefix_source == NULL ) {
        fclose(prefix_file);
        goto finish_prefix;
    } else {
        fread(prefix_source, (size_t)prefix_length, 1, prefix_file);
        fclose(prefix_file);
        prefix_source[prefix_length] = '\0';
    }

    id = glsl_compile_source(type, prefix_source, shader_source);
    if( ! id ) {
        log_fail(__FILE__, __LINE__, "compilation failed in: prefix + %s\n", filename);
    }

finish_prefix:
    free(prefix_source);
finish_dirname:
    free(prefix_dirname);
finish_shader:
    free(shader_source);
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
        fprintf(stdout, "failed to link shader program:\n");
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
