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

#ifndef DRAW_H
#define DRAW_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "ogl.h"
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
