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

#ifndef MATH_TYPES_H
#define MATH_TYPES_H

#include "float.h"
#include "limits.h"
#include "stdint.h"

#define PI 3.14159265358979323846264338327f
#define PI_OVER_180 0.017453292519943295769236907684886f
#define PI_OVER_360 0.0087266462599716478846184538424431f

typedef float* VecP;
typedef float* MatP;
typedef float* QuatP;

typedef float Vec[4];
typedef float Vec2f[2];
typedef float Vec3f[3];
typedef float Vec4f[4];

typedef float Mat[16];

typedef float Quat[4];

typedef uint8_t Color[4];

typedef float Texcoord[2];

#define NULL_VEC { 0, 0, 0, 1 }
#define NULL_MAT { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }

#define IDENTITY_VEC { 1, 1, 1, 1 }
#define IDENTITY_MAT { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }

#define X_AXIS { 1, 0, 0, 1 }
#define Y_AXIS { 0, 1, 0, 1 }
#define Z_AXIS { 0, 0, 1, 1 }

#define CUTE_EPSILON 0.00001f

#endif
