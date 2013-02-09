#ifndef GLS_H
#define GLSL_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "GL/glew.h"
#include "GL/gl.h"

#include "math_types.h"
#include "matrix.h"
#include "quaternion.h"
#include "io.h"

void debug_info_log( GLuint object,
                     PFNGLGETSHADERIVPROC glGet__iv,
                     PFNGLGETSHADERINFOLOGPROC glGet__InfoLog );

#define GLSL( source ) "#version 130\n#extension GL_ARB_uniform_buffer_object:require\n" #source "\0"

GLuint compile_source(GLenum type, const char* source, uint32_t length);
GLuint compile_file(GLenum type, const char* filename);
GLuint link_program(GLuint vertex_shader, GLuint fragment_shader);
GLuint make_program(const char *vertex_source, const char* fragment_source);

#endif
