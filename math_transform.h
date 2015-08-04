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

#ifndef MATH_TRANSFORM_H
#define MATH_TRANSFORM_H

#include "math_types.h"
#include "math_matrix.h"

struct Pivot {
    Vec position;
    Quat orientation;

    float zoom;
    float eye_distance;
};

void pivot_create(struct Pivot* pivot);
int pivot_lookat(struct Pivot* pivot, const Vec target);

VecP pivot_local_axis(struct Pivot* const pivot, Vec3f axis);

MatP pivot_world_transform(struct Pivot* const pivot, Mat world_transform);
MatP pivot_local_transform(struct Pivot* const pivot, Mat local_transform);

#endif
