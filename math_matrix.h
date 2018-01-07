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

#ifndef MATH_MATRIX_H
#define MATH_MATRIX_H

#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"

#include "math.h"

#include "math_types.h"
#include "math_quaternion.h"

// - I don't like this code anymore
// - some refactoring should be done:
// - originally the plan was to only have one Vec4f type, but now I am stuck with three Vec(4f), Vec3f and Vec2f
// - I tried making Vec3f and Vec2f the same as Vec(4f), but I did not like that, it worked, but I actually do
// get type warnings telling me about mismatched sizes here and there, eventually I should look into that again
// later
// - most stuff in here should assume Vec3f and work with that, in fact it does now
// - whenever I need something to handle 3f and 4f seperately, I should encode that in the function names with
// 3f and 4f, not like before where 4f was the default and did not have a suffix, and I had specially named 3f
// functions, now every time I have two seperate functions for 3f and 4f I want to make this explicit in their
// name
// - so, no suffix only for functions that work with both 3f and 4f
// - I am not going to handle 2f vectors here ever, apart from copying them
// - I don't like the encoded float typename everywhere, the f in 3f and 4f, it should just be 3 and 4

// vector creation
void vec_copy4f(const Vec4f v, Vec4f r);
void vec_copy3f(const Vec3f v, Vec3f r);
void vec_copy2f(const Vec4f v, Vec2f r);

void vec_perpendicular(const Vec4f v, Vec4f r);
void vec_basis(const Vec3f x, Vec3f y, Vec3f z);

// vector operations
void vec_add(const Vec3f v, const Vec3f w, Vec3f r);
void vec_add1f(const Vec3f v, float w, Vec3f r);
void vec_sub(const Vec3f v, const Vec3f w, Vec3f r);
void vec_sub1f(const Vec3f v, float w, Vec3f r);
void vec_mul1f(const Vec3f v, float w, Vec3f r);

void vec_invert(const Vec3f v, Vec3f r);
float vec_dot(const Vec3f v, const Vec3f w);
void vec_cross(const Vec3f v, const Vec3f w, Vec3f r);
float vec_squared(const Vec3f v);
float vec_length(const Vec3f v);
void vec_normalize(const Vec3f v, Vec3f r);

void vec_angle(const Vec3f v, const Vec3f w, float* r);
void vec_angle_points(const Vec3f a, const Vec3f b, const Vec3f c, float* r);

void vec_rotate(const Vec3f v, const Quat q, Vec3f r);

bool vec_nullp(const Vec4f v);
bool vec_unitp(const Vec4f v);
bool vec_equal(const Vec3f a, const Vec3f b);

float vec_sum(const Vec4f v);
float vec_sign(const Vec4f v);

void vec_print(const char* title, const Vec3f v);

void vec_lerp(const Vec3f a, const Vec3f b, float t, Vec3f r);

void vec_minmax(const Vec3f v, Vec3f min, Vec3f max);

// matrix creation
void mat_copy4f(const Mat m, Mat r);

void mat_basis(const Vec4f x, Mat r);

void mat_perspective(float left, float right, float top, float bottom, float zNear, float zFar, Mat m);
void mat_orthographic(float left, float right, float top, float bottom, float zNear, float zFar, Mat m);

void mat_identity(Mat m);

// matrix operations
void mat_invert(const Mat m, double* det, Mat r);

void mat_mul(const Mat m, const Mat n, Mat r);

void mat_mul_vec(const Mat m, const Vec3f v, Vec3f r);

void mat_translate(const Mat m, const Vec3f v, Mat r);

void mat_rotate(const Mat m, const Quat q, Mat r);

void mat_scale(const Mat m, float s, Mat r);

void mat_transpose(const Mat m, Mat r);

void mat_get_rotation(const Mat m, Mat r);

void mat_get_translation(const Mat m, Mat r);

void mat_print(const char* title, const Mat m);

#endif
