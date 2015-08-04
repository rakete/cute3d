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

#ifndef MATH_MATRIX_H
#define MATH_MATRIX_H

#include "stdlib.h"
#include "stdio.h"
#include "math.h"

#include "math_types.h"
#include "math_quaternion.h"

// vector
void vec_axisx(Vec r);
VecP vaxisx(Vec r);

void vec_axisy(Vec r);
VecP vaxisy(Vec r);

void vec_axisz(Vec r);
VecP vaxisz(Vec r);

void vec_copy(const Vec v, Vec r);
VecP vcopy(const Vec v, Vec r);

void vec_copy3f(const Vec3f v, Vec3f r);
VecP vcopy3f(const Vec3f v, Vec3f r);

void vec_copy3fmat(const Vec3f x, const Vec3f y, const Vec3f z, Mat r);
MatP vcopy3fmat(const Vec3f x, const Vec3f y, const Vec3f z, Mat r);

void vec_equal(const Vec a, const Vec b, int* r);
int vequal(const Vec a, const Vec b);

void vec_equal3f(const Vec3f a, const Vec3f b, int* r);
int vequal3f(const Vec3f a, const Vec3f b);

void vec_add(const Vec v, const Vec3f w, Vec r);
VecP vadd(const Vec v, Vec w);

void vec_add3f(const Vec3f v, const Vec3f w, Vec3f r);
VecP vadd3f(const Vec3f v, Vec3f w);

void vec_sub(const Vec v, const Vec3f w, Vec r);
VecP vsub(const Vec v, Vec w);

void vec_sub3f(const Vec3f v, const Vec3f w, Vec3f r);
VecP vsub3f(const Vec3f v, Vec3f w);

void vec_sub1f(const Vec v, float w, Vec r);
VecP vsub1f(Vec v, float w);

void vec_mul(const Vec v, const Vec w, Mat m);
VecP vmul(const Vec v, Mat w);

void vec_mul1f(const Vec3f v, float w, Vec r);
VecP vmul1f(Vec3f v, float w);

void vec_mul4f1f(const Vec v, float w, Vec r);
VecP vmul4f1f(Vec v, float w);

void vec_mul3f1f(const Vec3f v, float w, Vec r);
VecP vmul3f1f(Vec v, float w);

void vec_invert(const Vec v, Vec r);
VecP vinvert(Vec v);

void vec_dot(const Vec3f v, const Vec3f w, float* r);
float vdot(const Vec3f v, const Vec3f w);

void vec_cross(const Vec v, const Vec3f w, Vec r);
VecP vcross(const Vec v, Vec w);

void vec_cross3f(const Vec3f v, const Vec3f w, Vec3f r);
VecP vcross3f(const Vec3f v, Vec3f w);

void vec_length(const Vec3f v, float* r);
float vlength(const Vec3f v);

void vec_normalize(const Vec v, Vec r);
VecP vnormalize(Vec v);

void vec_normalize3f(const Vec3f v, Vec3f r);
VecP vnormalize3f(Vec3f v);

void vec_angle(const Vec3f v, const Vec3f w, float* r);
float vangle(const Vec3f v, const Vec3f w);

void vec_nullp(const Vec v, bool* r);
bool vnullp(const Vec v);

void vec_sum(const Vec v, float* sum);
float vsum(const Vec v);

void vec_sign(const Vec v, int* sign);
int vsign(const Vec v);

void vec_perpendicular(const Vec v, Vec r);
VecP vperpendicular(const Vec v);

void vec_basis(const Vec x, Vec y, Vec z);

void vec_print(const char* title, const Vec v);
void vec_print3f(const char* title, const Vec3f v);

// matrix creation
void mat_copy(const Mat m, Mat r);
void mat_copy3f(const Mat3f m, Mat3f r);

void mat_basis(const Vec x, Mat r);

void mat_perspective(float left, float right, float top, float bottom, float zNear, float zFar, Mat m);
void mat_orthographic(float left, float right, float top, float bottom, float zNear, float zFar, Mat m);

void mat_identity(Mat m);
MatP midentity(Mat m);

void mat_scaling(const Vec v, Mat r);
//MatP mscaling(Mat v);

void mat_translating(const Vec v, Mat r);
//MatP mtranslating(Mat v);

void mat_rotating(const Quat q, Mat r);
//MatP mrotating(Mat q);

// matrix op
void mat_invert(const Mat m, double* det, Mat r);
MatP minvert(Mat m, double* det);

void mat_invert3f(const Mat3f m, double* det, Mat3f r);
MatP minvert3f(Mat3f m, double* det);

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
