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
#include "assert.h"

#include "math.h"
#include "math_types.h"
#include "matrix.h"

void quat_copy(const Quat q, Quat r);

void quat_identity(Quat q);
QuatP qidentity(Quat q);

bool quat_rotating_axis(const Vec axis, const float angle, Quat q);
QuatP qrotating_axis(Vec axis, const float angle);

bool quat_rotating_vec(const Vec a, const Vec b, Quat q);
QuatP qrotating_vec(const Vec a, Vec b);

bool quat_rotate_axis(const Quat q, const Vec axis, const float angle, Quat r);
QuatP qrotate_axis(const Vec axis, const float angle, Quat q);

bool quat_rotate_vec(const Quat q, const Vec a, const Vec b, Quat r);
QuatP qrotate_vec(const Vec a, const Vec b, Quat q);

void quat_apply_vec(const Quat q, const Vec vec, Vec r);
void quat_apply_vec3f(const Quat q, const Vec3f vec, Vec3f r);

void quat_mul(const Quat qa, const Quat qb, Quat r);
QuatP qmul(const Quat qa, Quat qb);

void quat_mul1f(const Quat qa, float b, Quat r);
QuatP qmul1f(Quat qa, float b);

void quat_add(const Quat qa, const Quat qb, Quat r);
QuatP qadd(const Quat qa, Quat qb);

void quat_dot(const Quat qa, const Quat qb, float* r);
float qdot(const Quat qa, const Quat qb);

void quat_conjugate(const Quat q, Quat r);
//QuatP qconjugate(Quat q);

void quat_invert(const Quat q, Quat r);
//QuatP qinvert(Quat q);

void quat_magnitude(const Quat q, float* r);
float qmagnitude(const Quat q);

void quat_normalize(const Quat q, Quat r);
QuatP qnormalize(Quat q);

void quat_mat(const Quat q, Mat r);
QuatP qmat(const Quat q, Mat m);

void quat_axis_angle(const Quat q, Vec axis, float* angle);
//VecP qaxis_angle(Quat q);

void quat_slerp(const Quat qa, const Quat qb, float t, Quat r);
QuatP qslerp(const Quat qa, Quat qb, float t);

#endif
