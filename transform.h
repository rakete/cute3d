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

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "math_types.h"
#include "matrix.h"

struct Pivot {
    Vec position;
    Quat orientation;

    float zoom;
    float eye_distance;
};

void pivot_create(struct Pivot* pivot);
void pivot_lookat(struct Pivot* pivot, const Vec target);

void pivot_world_transform(struct Pivot pivot, Mat world_transform);
void pivot_body_transform(struct Pivot pivot, Mat body_transform);

#endif
