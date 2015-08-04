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

#ifndef MATH_QUATERNION_H
#define MATH_QUATERNION_H

#include "stdbool.h"
#include "assert.h"

#include "math.h"
#include "math_types.h"
#include "math_matrix.h"

void quat_copy(const Quat q, Quat r);

void quat_identity(Quat q);
QuatP qidentity(Quat q);

bool quat_from_axis_angle(const Vec3f axis, const float angle, Quat q);
QuatP qfrom_axis_angle(Vec3f axis, const float angle);

bool quat_from_vec_pair(const Vec3f a, const Vec3f b, Quat q);
QuatP qfrom_vec_pair(const Vec3f a, Vec3f b);

void quat_rotate_vec(const Vec vec, const Quat q, Vec r);
void quat_rotate_vec3f(const Vec3f vec, const Quat q, Vec3f r);

bool quat_mul_axis_angle(const Quat q, const Vec3f axis, const float angle, Quat r);
QuatP qmul_axis_angle(const Vec3f axis, const float angle, Quat q);

bool quat_mul_vec_pair(const Quat q, const Vec3f a, const Vec3f b, Quat r);
QuatP qmul_vec_pair(const Vec3f a, const Vec3f b, Quat q);

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

void quat_to_mat(const Quat q, Mat r);
QuatP qto_mat(const Quat q, Mat m);

void quat_to_axis_angle(const Quat q, Vec axis, float* angle);
//VecP qto_axis_angle(Quat q);

void quat_slerp(const Quat qa, const Quat qb, float t, Quat r);
QuatP qslerp(const Quat qa, Quat qb, float t);

#endif
