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

#define draw_add_gl_lines_shader(canvas, name) do {                     \
        static int32_t found_gl_lines_shader = -1;                      \
        if( found_gl_lines_shader < 0 ) {                               \
            if( canvas_find_shader(canvas, name) == NUM_CANVAS_SHADER ) { \
                struct Shader gl_lines_shader;                          \
                shader_create_gl_lines(name, &gl_lines_shader);         \
                found_gl_lines_shader = canvas_add_shader(canvas, &gl_lines_shader); \
            }                                                           \
        }                                                               \
    }while(0);

#define draw_add_flat_shader(canvas, name) do {                         \
        static int32_t found_flat_shader = -1;                          \
        if( found_flat_shader < 0 ) {                                   \
            if( canvas_find_shader(canvas, name) == NUM_CANVAS_SHADER ) { \
                struct Shader flat_shader;                              \
                shader_create_flat(name, &gl_lines_shader);             \
                found_flat_shader = canvas_add_shader(canvas, &flat_shader); \
            }                                                           \
        }                                                               \
    }while(0);

void draw_grid(struct Canvas* canvas,
               int32_t layer,
               float width,
               float height,
               uint32_t steps,
               const Color color,
               const Mat model_matrix);

void draw_arrow(struct Canvas* canvas,
                int32_t layer,
                const Vec4f v,
                const Vec4f pos,
                float offset,
                float scale,
                const Color color,
                const Mat model_matrix);

void draw_vec(struct Canvas* canvas,
              int32_t layer,
              const Vec4f v,
              const Vec4f pos,
              float arrow,
              float scale,
              const Color color,
              const Mat model_matrix);

void draw_quat(struct Canvas* canvas,
               int32_t layer,
               const Quat q,
               float scale,
               const Color color1,
               const Color color2,
               const Mat model_matrix);

void draw_circle(struct Canvas* canvas,
                 int32_t layer,
                 float radius,
                 float start,
                 float end,
                 float arrow,
                 const Color color,
                 const Mat model_matrix);

void draw_basis(struct Canvas* canvas,
                int32_t layer,
                float scale,
                const Mat model_matrix);

void draw_reticle(struct Canvas* canvas,
                  int32_t layer,
                  float scale,
                  const Color color,
                  const Mat model_matrix);

void draw_contact(struct Canvas* canvas,
                  int32_t layer,
                  const Vec4f contact_point,
                  const Vec4f contact_normal,
                  float contact_penetration,
                  float scale,
                  const Mat model_matrix);

void draw_normals_array(struct Canvas* canvas,
                        int32_t layer,
                        const float* vertices,
                        const float* normals,
                        int32_t n,
                        float scale,
                        const Color color,
                        const Mat model_matrix);

void draw_camera(struct Canvas* canvas,
                 int32_t layer,
                 const struct Camera* camera,
                 float scale,
                 const Color color,
                 const Mat model_matrix);

/* void draw_texture_quad( float scale, */
/*                         GLuint texture_id, */
/*                         const Mat projection_matrix, */
/*                         const Mat view_matrix, */
/*                         const Mat model_matrix); */

/* void draw_color_quad( float scale, */
/*                       const Color color, */
/*                       const Mat projection_matrix, */
/*                       const Mat view_matrix, */
/*                       const Mat model_matrix); */

/* void draw_line_quad( float scale, */
/*                      const Color color, */
/*                      const Mat projection_matrix, */
/*                      const Mat view_matrix, */
/*                      const Mat model_matrix); */

#endif
