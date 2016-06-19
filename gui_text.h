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

#ifndef GUI_TEXT_H
#define GUI_TEXT_H

#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "wchar.h"

#include "math_types.h"
#include "math_camera.h"
#include "math_color.h"

#include "driver_ogl.h"

#include "gui_canvas.h"

#define MAX_TEXT_PUT_SIZE 2048

void text_put(struct Canvas* canvas,
              int32_t layer,
              Vec4f cursor,
              int32_t projection,
              const Mat model_matrix,
              const Color color,
              float scale,
              const char* font_name,
              const wchar_t* unicode_text);

void text_put_world(struct Canvas* canvas,
                    int32_t layer,
                    Vec4f cursor,
                    const Mat model_matrix,
                    const Color color,
                    float scale,
                    const char* font_name,
                    const wchar_t* unicode_text);

void text_put_screen(struct Canvas* canvas,
                     int32_t layer,
                     Vec4f cursor,
                     int32_t x, int32_t y,
                     const Color color,
                     float scale,
                     const char* font_name,
                     const wchar_t* unicode_text);

void text_printf(struct Canvas* canvas,
                 int32_t layer,
                 Vec4f cursor,
                 int32_t x, int32_t y,
                 const Color color,
                 float scale,
                 const char* font_name,
                 const wchar_t* format,
                 ...);

double text_show_fps(struct Canvas* canvas,
                     int32_t layer,
                     Vec4f cursor,
                     int32_t x, int32_t y,
                     const Color color,
                     float scale,
                     const char* font_name,
                     double delta);

void text_show_time(struct Canvas* canvas,
                    int32_t layer,
                    Vec4f cursor,
                    int32_t x, int32_t y,
                    const Color color,
                    float scale,
                    const char* font_name,
                    double time);


#endif
