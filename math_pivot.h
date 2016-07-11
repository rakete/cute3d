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

#ifndef MATH_PIVOT_H
#define MATH_PIVOT_H

#include "math_types.h"
#include "math_matrix.h"

struct Pivot {
    Vec4f position;
    Quat orientation;

    float eye_distance;

    const struct Pivot* parent;
};

extern struct Pivot global_null_pivot;

void pivot_create(Vec3f position, Quat orientation, struct Pivot* pivot);
void pivot_attach(struct Pivot* child, const struct Pivot* parent);

int32_t pivot_lookat(struct Pivot* pivot, const Vec4f target);

VecP* pivot_local_axis(const struct Pivot* pivot, Vec3f axis);

MatP* pivot_world_transform(const struct Pivot* pivot, Mat world_transform);
MatP* pivot_local_transform(const struct Pivot* pivot, Mat local_transform);

// - so this computes a transform from pivot1 coords to pivot2 coords, in other words,
// the resulting transform of this function, applied to vertices of pivot1, would move
// them to where they should be if pivot2 was at the origin (0,0,0)
// - between_transform moves pivot1 vertices to where they should be relative to pivot2
MatP* pivot_between_transform(const struct Pivot* pivot1, const struct Pivot* pivot2, Mat between_transform);

QuatP* pivot_between_orientation(const struct Pivot* pivot1, const struct Pivot* pivot2, Quat between_rotation);
VecP* pivot_between_translation(const struct Pivot* pivot1, const struct Pivot* pivot2, Vec3f between_translation);

struct Pivot* pivot_combine(const struct Pivot* pivot1, const struct Pivot* pivot2, struct Pivot* r);

#endif
