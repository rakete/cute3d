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

typedef void (*matrix_initfunc)(Matrix m);

// vector
void vector_copy(const Vec v, Vec r);

void vector_add(const Vec v, const Vec w, Vec r);
VecP vadd(const Vec v, Vec w);

void vector_add3f(const Vec v, const Vec3f w, Vec r);
void vector3f_add3f(const Vec3f v, const Vec3f w, Vec3f r);

void vector_subtract(const Vec v, const Vec w, Vec r);

void vector_multiply(const Vec v, const Vec w, Matrix m);
VecP vmul(Vec v, Matrix w);

void vector_multiply1f(float v, const Vec w, Vec r);
VecP vmul1f(float v, Vec w);

void vector_invert(const Vec v, Vec r);
VecP vinv(Vec v);

void vector_dot(const Vec v, const Vec w, float* r);
float vdot(const Vec v, const Vec w);

void vector_cross(const Vec v, const Vec w, Vec r);
VecP vcross(const Vec v, Vec w);

void vector_magnitude(const Vec v, VecP r);
float vmagnitude(const Vec v);

void vector_normalize(const Vec v, Vec r);
VecP vnormalize(Vec v);

void vector_angle(const Vec v, const Vec w, VecP r);
float vangle(const Vec v, const Vec w);

void vector_isnull(const Vec v, bool* r);
bool vnullp(const Vec v);

void vector_perpendicular(const Vec v, Vec r);

// matrix creation
void matrix_copy(const Matrix m, Matrix r);

void matrix_perspective(float left, float right, float top, float bottom, float zNear, float zFar, Matrix m);
void matrix_orthographic(float left, float right, float top, float bottom, float zNear, float zFar, Matrix m);

void matrix_identity(Matrix m);
//MatrixP midentity(Matrix m);

void matrix_invert(const Matrix m, Matrix r, double* det);
MatrixP minv(Matrix m, double* det);

// matrix op
void matrix_multiply(const Matrix m, const Matrix n, Matrix r);
MatrixP mmul(const Matrix m, Matrix n);

void matrix_multiply_vec(const Matrix m, const Vec v, Vec r);
MatrixP mmul_vec(const Matrix m, Vec v);

void matrix_translate(const Matrix m, const Vec v, Matrix r);
//MatrixP mtranslate(const Vec v, Matrix m);

void matrix_rotate(const Matrix m, const Quat q, Matrix r);
//MatrixP mrotate(const Quat q, Matrix m);

void matrix_scale(const Matrix m, const Vec v, Matrix r);
//MatrixP mscale(const Vec v, Matrix m);

void matrix_scaling(const matrix_initfunc f, const Vec v, Matrix r);

void matrix_transpose(const Matrix m, Matrix r);
MatrixP mtransp(Matrix m);

#endif
