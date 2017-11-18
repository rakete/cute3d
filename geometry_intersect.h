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

#ifndef GEOMETRY_INTERSECT_H
#define GEOMETRY_INTERSECT_H

#include "stdlib.h"
#include "math.h"

#include "math_types.h"
#include "math_matrix.h"

#include "geometry_types.h"

enum IntersectPlaneSegmentResult {
    PLANE_SEGMENT_ON_PLANE = -1,
    PLANE_SEGMENT_PARALLEL = 0,
    PLANE_SEGMENT_INTERSECTION,
    PLANE_SEGMENT_ONLY_LINE_INTERSECTION,
};

enum IntersectPlaneSegmentResult intersect_plane_segment(const Vec3f plane_normal, const Vec3f plane_point, const Vec3f a, const Vec3f b, float* interpolation_value, Vec3f result);

enum IntersectPlanePlaneResult {
    PLANE_PLANE_DISJOINT = 0,
    PLANE_PLANE_COINCIDE,
    PLANE_PLANE_INTERSECTION
};

enum IntersectPlanePlaneResult intersect_plane_plane(const Vec3f plane_normal_a, const Vec3f plane_point_a, const Vec3f plane_normal_b, const Vec3f plane_point_b, Vec3f result_a, Vec3f result_b);

size_t intersect_plane_aabb(const Vec3f plane_normal, const Vec3f plane_point, const Vec3f aabb_half_sizes, const Vec3f aabb_center, size_t result_size, float* result);

#endif