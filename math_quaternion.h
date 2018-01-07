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

#ifndef MATH_QUATERNION_H
#define MATH_QUATERNION_H

#include "math.h"

#include "driver_log.h"

#include "math_types.h"
#include "math_matrix.h"

// quaternion creation
void quat_copy(const Quat q, Quat r);

void quat_identity(Quat q);

void quat_from_euler_angles(float x, float y, float z, Quat q);

void quat_from_axis_angle(const Vec3f axis, const float angle, Quat q);

// - create quaternion that rotates a -> b
void quat_from_vec_pair(const Vec3f a, const Vec3f b, Quat q);

// quaternion operations
void quat_mul_axis_angle(const Quat q, const Vec3f axis, const float angle, Quat r);

void quat_mul(const Quat qa, const Quat qb, Quat r);

void quat_mul1f(const Quat qa, float b, Quat r);

void quat_add(const Quat qa, const Quat qb, Quat r);

float quat_dot(const Quat qa, const Quat qb);

void quat_conjugate(const Quat q, Quat r);

void quat_invert(const Quat q, Quat r);

float quat_magnitude(const Quat q);

void quat_normalize(const Quat q, Quat r);

// quaternion conversion
void quat_to_mat(const Quat q, Mat r);

void quat_to_axis_angle(const Quat q, Vec4f axis, float* angle);

// quaternion interpolation
void quat_slerp(const Quat qa, const Quat qb, float t, Quat r);

#endif
