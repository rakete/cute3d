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
typedef int32_t Vec3i[3];
typedef int32_t Vec4i[4];

typedef float Mat[16];
typedef float Mat4f[16];
typedef float Mat4i[16];

typedef float Quat[4];

typedef float Color[4];

typedef float Texcoord[2];

#define vec(v) vec4f(v)
#define vecf(v) vec4f(v)
#define vec2f(v) (Vec3f){(float)v[0], (float)v[1]}
#define vec3f(v) (Vec3f){(float)v[0], (float)v[1], (float)v[2]}
#define vec4f(v) (Vec4f){(float)v[0], (float)v[1], (float)v[2], (float)v[3]}

#define x_(v) v[0]
#define y_(v) v[1]
#define z_(v) v[2]
#define w_(v) v[3]

#define matf(m) mat4x4f(m)
#define mat4x4f(m)                                                      \
    (Mat){(float)m[0], (float)m[1], (float)m[2], (float)m[3],           \
            (float)m[4], (float)m[5], (float)m[6], (float)m[7],         \
            (float)m[8], (float)m[9], (float)m[10], (float)m[11],       \
            (float)m[12], (float)m[13], (float)m[14], (float)m[15] }


#define NULL_VEC { 0, 0, 0, 1 }
#define NULL_VEC3F { 0, 0, 0 }
#define NULL_MAT { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

#define IDENTITY_MAT { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }

#define X_AXIS { 1, 0, 0, 1 }
#define Y_AXIS { 0, 1, 0, 1 }
#define Z_AXIS { 0, 0, 1, 1 }

#define FLOAT_EPSILON 0.00001f

#endif
