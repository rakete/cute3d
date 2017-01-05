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

#ifndef MATH_COLOR_H
#define MATH_COLOR_H

#include "stdlib.h"
#include "stdio.h"
#include "math.h"

#include "math_types.h"
#include "math_matrix.h"

#include "geometry_types.h"

#define RED (Color){255, 0, 0, 255}

void color_copy(const Color c, Color r);
void color_round(const Vec4f a, Color r);
void color_lerp(const Color a, const Color b, float t, Color r);

#endif
