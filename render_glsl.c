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

#include "render_glsl.h"

void glsl_debug_info_log( GLuint object,
                     PFNGLGETSHADERIVPROC glGet__iv,
                     PFNGLGETSHADERINFOLOGPROC glGet__InfoLog )
{
    GLint log_length;
    char *log;

    glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
    log = malloc(log_length);
    glGet__InfoLog(object, log_length, NULL, log);
    fprintf(stderr, "%s", log);
    free(log);
}

GLuint glsl_compile_source(GLenum type, const char* source, GLsizei length) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar**)&source, &length);
    glCompileShader(shader);

    GLint shader_ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
    if ( ! shader_ok ) {
        fprintf(stderr, "Failed to compile: %s\n", source);
        glsl_debug_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint glsl_compile_file(GLenum type, const char* filename) {
    GLsizei length;
    GLchar* source = read_file(filename, &length);

    if(!source) return 0;

    fprintf(stderr, "Compiling: %s\n", filename);
    GLuint id = glsl_compile_source(type, source, length);
    if( ! id ) {
        fprintf(stderr, "Compilation failed in: %s\n", filename);
    }
    return id;
}

GLuint glsl_link_program(GLuint vertex_shader, GLuint fragment_shader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint program_ok;
    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
    if (!program_ok) {
        fprintf(stderr, "Failed to link shader program:\n");
        glsl_debug_info_log(program, glGetProgramiv, glGetProgramInfoLog);
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

GLuint glsl_make_program(const char *vertex_source, const char* fragment_source) {
    GLsizei length_vertex = strlen(vertex_source);
    GLsizei length_fragment = strlen(fragment_source);
    GLuint vertex = glsl_compile_source(GL_VERTEX_SHADER, vertex_source, length_vertex);
    GLuint fragment = glsl_compile_source(GL_FRAGMENT_SHADER, fragment_source, length_fragment);

    return glsl_link_program(vertex, fragment);
}
