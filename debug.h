#ifndef DEBUG_H
#define DEBUG_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "GL/glew.h"
#include "GL/gl.h"

#include "math_types.h"
#include "matrix.h"
#include "quaternion.h"

void show_info_log( GLuint object,
                    PFNGLGETSHADERIVPROC glGet__iv,
                    PFNGLGETSHADERINFOLOGPROC glGet__InfoLog );

GLuint debug_compile(const char *vertex_source, const char* fragment_source);

void debug_grid(float projection_matrix[16],
                float view_matrix[16],
                float model_matrix[][16],
                int instances,
                int steps,
                float color[4]);

void debug_normals_array(float projection_matrix[16],
                         float view_matrix[16],
                         float model_matrix[16],
                         float* vertices,
                         float* normals,
                         int n,
                         float color[4]);

void debug_normals_buffer(float projection_matrix[16],
                          float view_matrix[16],
                          float model_matrix[16],
                          GLuint vertices,
                          GLuint normals,
                          int n,
                          float color[4]);

void debug_texture_quad( float projection_matrix[16],
                         float view_matrix[16],
                         float model_matrix[16],
                         GLuint texture_id );

#endif
