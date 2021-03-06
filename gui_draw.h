/* Cute3D, a simple opengl based framework for writing interactive realtime applications */

/* Copyright (C) 2013-2017 Andreas Raster */

/* This file is part of Cute3D. */

/* Cute3D is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* Cute3D is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with Cute3D.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef GUI_DRAW_H
#define GUI_DRAW_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "math_types.h"
#include "math_matrix.h"
#include "math_quaternion.h"
#include "math_camera.h"
#include "math_geometry.h"
#include "math_color.h"

#include "driver_ogl.h"
#include "driver_glsl.h"

#include "gui_canvas.h"

void draw_transform_vertices(size_t vertex_size,
                             GLenum component_type,
                             size_t num_vertices,
                             const float* vertices_in,
                             const Mat transform,
                             const float* vertices_out);

void draw_line(struct Canvas* canvas,
               int32_t layer_i,
               const Mat model_matrix,
               const Color color,
               float line_thickness,
               const Vec3f p,
               const Vec3f q);

void draw_grid(struct Canvas* canvas,
               int32_t layer,
               const Mat model_matrix,
               const Color color,
               float line_thickness,
               float width,
               float height,
               uint32_t steps);

#endif
