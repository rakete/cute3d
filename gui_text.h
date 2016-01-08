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
#include "driver_ogl.h"
#include "gui_canvas.h"

void text_put_world(struct Canvas* canvas, int32_t layer, const char* font_name, float scale, const Color color, const wchar_t* text, Mat model_matrix);

void text_put_screen(struct Canvas* canvas, int32_t layer, const char* font_name, float scale, const Color color, const wchar_t* text, int32_t x, int32_t y);

void text_print32_tf(struct Canvas* canvas, int32_t layer, const char* font_name, float scale, const Color color, const wchar_t* format, ...);

void text_show_fps(struct Canvas* canvas, int32_t layer, const char* font_name, float scale, const Color color, double delta);

void text_show_time(struct Canvas* canvas, int32_t layer, const char* font_name, float scale, const Color color, double time);


#endif
