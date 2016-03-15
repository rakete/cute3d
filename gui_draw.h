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

#ifndef GUI_DRAW_H
#define GUI_DRAW_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "math_types.h"
#include "math_matrix.h"
#include "math_quaternion.h"
#include "math_camera.h"
#include "math_color.h"

#include "driver_ogl.h"
#include "driver_glsl.h"

#include "gui_canvas.h"

#define draw_add_shader(canvas, shader, name) do {                      \
        static int32_t found_##shader##_shader = -1;                    \
        if( found_##shader##_shader < 0 ) {                             \
            if( canvas_find_shader(canvas, name) == MAX_CANVAS_SHADER ) { \
                struct Shader shader##_shader;                        \
                shader_create_##shader(name, &shader##_shader);     \
                found_##shader##_shader = canvas_add_shader(canvas, &shader##_shader); \
            }                                                           \
        }                                                               \
    }while(0);

void draw_transform_vertices(size_t vertex_size,
                             GLenum component_type,
                             size_t num_vertices,
                             const float vertices_in[vertex_size*num_vertices],
                             const Mat transform,
                             const float vertices_out[vertex_size*num_vertices]);

void draw_grid(struct Canvas* canvas,
               int32_t layer,
               const Mat model_matrix,
               const Color color,
               float width,
               float height,
               uint32_t steps);

void draw_arrow(struct Canvas* canvas,
                int32_t layer,
                const Mat model_matrix,
                const Color color,
                const Vec3f v,
                const Vec3f pos,
                float offset,
                float scale);

void draw_vec(struct Canvas* canvas,
              int32_t layer,
              const Mat model_matrix,
              const Color color,
              const Vec3f v,
              const Vec3f pos,
              float arrow,
              float scale);

void draw_quat(struct Canvas* canvas,
               int32_t layer,
               const Mat model_matrix,
               const Color color1,
               const Color color2,
               const Quat q,
               float scale);

void draw_circle(struct Canvas* canvas,
                 int32_t layer,
                 const Mat model_matrix,
                 const Color color,
                 float radius,
                 float start,
                 float end,
                 float arrow);

void draw_basis(struct Canvas* canvas,
                int32_t layer,
                const Mat model_matrix,
                float scale);

void draw_reticle(struct Canvas* canvas,
                  int32_t layer,
                  const Mat model_matrix,
                  const Color color,
                  float scale);

void draw_camera(struct Canvas* canvas,
                 int32_t layer,
                 const Mat model_matrix,
                 const Color color,
                 const struct Camera* camera);

#endif
