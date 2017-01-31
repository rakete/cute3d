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

#ifndef GEOMETRY_POLYGON_H
#define GEOMETRY_POLYGON_H

#include "stdint.h"

#include "math_matrix.h"
#include "math_types.h"

void polygon_corner_area(size_t polygon_size, size_t point_size, const float* polygon, size_t corner_i, float* result);

size_t polygon_corner_remove(size_t polygon_size, size_t point_size, size_t type_size, const void* polygon, size_t corner_i, void* result);

void polygon_compute_normal(size_t polygon_size, size_t point_size, const float* polygon, Vec3f result_normal);

enum PolygonCutType {
    POLYGON_COPLANNAR = 0, // when poly lies on cutting plane
    POLYGON_FRONT, // when poly is entirely on front
    POLYGON_BACK, // when poly is entirely on back
    POLYGON_SPANNING // when polygon is cut
};

struct PolygonCutPoints {
    float distance;
    enum PolygonCutType type;
    int32_t interpolation_index;
    float interpolation_value;
    int32_t num_cuts;
};

enum PolygonCutType polygon_cut(size_t polygon_size, size_t point_size, const float* polygon,
                                const Vec3f plane_normal, const Vec3f plane_point,
                                size_t result_size, struct PolygonCutPoints* result_points);

void polygon_triangulate(size_t polygon_size, size_t point_size, const float* polygon, size_t result_size, size_t* result);

#endif
