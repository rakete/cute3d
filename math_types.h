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

#define PI 3.14159265358979323846264338327
#define PI_OVER_180 0.017453292519943295769236907684886
#define PI_OVER_360 0.0087266462599716478846184538424431

typedef float Vec[4];
typedef float Vec3f[3];
typedef float Vec4f[4];
typedef int Vec3i[3];
typedef int Vec4i[4];

typedef float Matrix[16];
typedef float Matrix3f[9];
typedef float Matrix4f[16];
typedef float Matrix3i[9];
typedef float Matrix4i[16];

typedef float Quat[4];

typedef float Color[4];


#endif
