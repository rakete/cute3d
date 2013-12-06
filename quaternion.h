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

#ifndef QUATERNION_H
#define QUATERNION_H

#include "stdbool.h"

#include "math.h"
#include "math_types.h"
#include "matrix.h"

void quat_identity(Quat quat);
bool rotation_quat(const Vec axis, const float angle, Quat quat);

void quat_rotate(const Quat quat, const Vec vec, Vec result);
void quat_rotate3f(const Quat quat, const Vec3f vec, Vec3f result);

void quat_product(const Quat qa, const Quat qb, Quat result);

void quat_dot(const Quat qa, const Quat qb, float* result);
float qdot(const Quat qa, const Quat qb);

void quat_conjugate(const Quat quat, Quat result);
void quat_invert(const Quat quat, Quat result);

void quat_magnitude(const Quat quat, float* result);
float qmagnitude(const Quat quat);

void quat_matrix(const Quat quat, const Matrix m, Matrix result);
float* qmatrix(const Quat quat, Matrix m);


#endif

