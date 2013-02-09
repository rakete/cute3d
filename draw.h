#ifndef DRAW_H
#define DRAW_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "GL/glew.h"
#include "GL/gl.h"

#include "math_types.h"
#include "matrix.h"
#include "quaternion.h"
#include "glsl.h"

void draw_grid(int instances,
                int steps,
                float color[4],
                float projection_matrix[16],
                float view_matrix[16],
                float model_matrix[][16]);

void draw_normals_array(float* vertices,
                         float* normals,
                         int n,
                         float color[4],
                         float projection_matrix[16],
                         float view_matrix[16],
                         float model_matrix[16]);

void draw_normals_buffer(GLuint vertices,
                          GLuint normals,
                          int n,
                          float color[4],
                          float projection_matrix[16],
                          float view_matrix[16],
                          float model_matrix[16]);

void draw_texture_quad(GLuint texture_id,
                        float projection_matrix[16],
                        float view_matrix[16],
                        float model_matrix[16]);

#endif
