#ifndef MATH_DRAW_H
#define MATH_DRAW_H

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
#include "gui_draw.h"

void draw_arrow(struct Canvas* canvas,
                int32_t layer,
                const Mat model_matrix,
                const Color color,
                float line_thickness,
                const Vec3f v,
                const Vec3f pos,
                float offset,
                float scale);

void draw_vec(struct Canvas* canvas,
              int32_t layer,
              const Mat model_matrix,
              const Color color,
              float line_thickness,
              const Vec3f v,
              const Vec3f pos,
              float arrow,
              float scale);

void draw_quat(struct Canvas* canvas,
               int32_t layer,
               const Mat model_matrix,
               const Color color1,
               const Color color2,
               float line_thickness,
               const Quat q,
               float scale);

void draw_circle(struct Canvas* canvas,
                 int32_t layer,
                 const Mat model_matrix,
                 const Color color,
                 float line_thickness,
                 float radius,
                 float start,
                 float end,
                 float arrow);

void draw_basis(struct Canvas* canvas,
                int32_t layer,
                const Mat model_matrix,
                float line_thickness,
                float scale);

void draw_reticle(struct Canvas* canvas,
                  int32_t layer,
                  const Mat model_matrix,
                  const Color color,
                  float line_thickness,
                  float scale);

void draw_camera(struct Canvas* canvas,
                 int32_t layer,
                 const Mat model_matrix,
                 const Color color,
                 float line_thickness,
                 const struct Camera* camera);

#endif
