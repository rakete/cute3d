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

#ifndef GUI_WIDGETS_H
#define GUI_WIDGETS_H

#include "gui_canvas.h"

void widgets_display_texture(struct Canvas* canvas,
                             int32_t layer_i,
                             int32_t x, int32_t y,
                             int32_t width,
                             int32_t height,
                             const char* name,
                             struct Texture texture);


#endif
