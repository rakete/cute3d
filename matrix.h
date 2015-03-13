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

#ifndef MATRIX_H
#define MATRIX_H

#include "stdlib.h"
#include "stdio.h"
#include "math.h"

#include "math_types.h"
#include "quaternion.h"

// vector
void vec_copy(const Vec v, Vec r);

void vec_add(const Vec v, const Vec w, Vec r);
VecP vadd(const Vec v, Vec w);

void vec_add3f(const Vec v, const Vec3f w, Vec r);
VecP vadd3f(const Vec v, Vec w);

void vec_3fadd3f(const Vec3f v, const Vec3f w, Vec3f r);
//VecP v3fadd3f(const Vec3f v, Vec w3f);

void vec_sub(const Vec v, const Vec w, Vec r);
VecP vsub(const Vec v, Vec w);

void vec_sub1f(const Vec v, float w, Vec r);
VecP vsub1f(Vec v, float w);

void vec_mul(const Vec v, const Vec w, Mat m);
VecP vmul(const Vec v, Mat w);

void vec_mul1f(const Vec v, float w, Vec r);
VecP vmul1f(Vec v, float w);

void vec_invert(const Vec v, Vec r);
VecP vinvert(Vec v);

void vec_dot(const Vec v, const Vec w, float* r);
float vdot(const Vec v, const Vec w);

void vec_cross(const Vec v, const Vec w, Vec r);
VecP vcross(const Vec v, Vec w);

void vec_length(const Vec v, float* r);
float vlength(const Vec v);

void vec_normalize(const Vec v, Vec r);
VecP vnormalize(Vec v);

void vec_angle(const Vec v, const Vec w, float* r);
float vangle(const Vec v, const Vec w);

void vec_nullp(const Vec v, bool* r);
bool vnullp(const Vec v);

void vec_perpendicular(const Vec v, Vec r);
VecP vperpendicular(const Vec v);

void vec_basis(const Vec x, Vec y, Vec z);

void vec_print(const char* title, const Vec v);

// matrix creation
void mat_copy(const Mat m, Mat r);

void mat_basis(const Vec x, Mat r);

void mat_perspective(float left, float right, float top, float bottom, float zNear, float zFar, Mat m);
void mat_orthographic(float left, float right, float top, float bottom, float zNear, float zFar, Mat m);

void mat_identity(Mat m);
MatP midentity(Mat m);

void mat_scaling(const Vec v, Mat r);
//MatP mscaling(Mat v);

void mat_translating(const Vec v, Mat r);
//MatP mtranslating(Mat v);

// matrix op
void mat_invert(const Mat m, double* det, Mat r);
MatP minvert(Mat m, double* det);

void mat_mul(const Mat m, const Mat n, Mat r);
MatP mmul(const Mat m, Mat n);

void mat_mul_vec(const Mat m, const Vec v, Vec r);
MatP mmul_vec(const Mat m, Vec v);

void mat_translate(const Mat m, const Vec v, Mat r);
//MatP mtranslate(const Vec v, Mat m);

void mat_rotate(const Mat m, const Quat q, Mat r);
MatP mrotate(Mat m, const Quat q);

void mat_scale(const Mat m, const Vec v, Mat r);
//MatP mscale(const Vec v, Mat m);

void mat_transpose(const Mat m, Mat r);
MatP mtranspose(Mat m);

void mat_print(const char* title, const Mat m);

#endif
